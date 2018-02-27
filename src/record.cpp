/* =========================================================================
 * EXECUTIVE SUMMARY
 *
 * 'Project_ID:       kyrolab
 * 'Filename:         $record.cpp $ 
 * 'Title:            
 * 'System:           Unix
 * 'Entry_Point:      main
 * 'Author:           $Author: Yong Hyun Kim $
 * 'Creation_Date:    2015/09/21
 * 
 * 'Description:
 *    This program allows log messages to be added to a queue from different threads.
 *
 * 'General Notes:
 *
 * 'End
 * ========================================================================= */

#include "./record.h"
#include "./tensor.h"
#include "./config.h"

using namespace std;
std::mutex gMtx;
std::condition_variable gCV;

Record::Record(const std::string& fname) : mExit(false), mFileName(fname)
{
	//Start background thread
	mThread = thread{&Record::logToFile, this};
}

Record::~Record()
{
	{
		unique_lock<mutex> lck(mMutex);
		//Nicely shut down the background thread by setting
		//mExit to true and notifying the thread.
		mExit = true;
		//Notify condition variable to wake up thread.
		mCondVar.notify_all();
	} //This curly bracket is used for releasing lock - automatically call lck.unlock() due to scope.
	//block because the lock on mMutex must be released before calling join().
	mThread.join();
}

void Record::logToQueue(const std::string& entry)
{
	unique_lock<mutex> lck(mMutex);
	mQueue.push(entry);
	mCondVar.notify_all();
}

void Record::logToFile()
{
	//Open log file
	ofstream ofs(mFileName);
	if (ofs.fail())
	{
		cerr << "Error: Failure to open log file." << endl;
		return;
	}

	//Start the loop
	unique_lock<mutex> lck(mMutex);
	while(true)
	{
		if(!mExit)
		{
			//Wait for notification
			//when thread is unblocked by notify call from another thread,
			//wait(lck) calls lck.lock() again, possibly blocking on the lock
			//and then returning.
			mCondVar.wait(lck);
		}

		//Condition variable notified meaning something might be in the queue
		//and/or we need to shut down this thread.
		lck.unlock();
		while(true)
		{
			lck.lock();
			if(mQueue.empty())
			{
				break;
			} else
			{
				ofs << mQueue.front() << endl;
				mQueue.pop();
			}
			//std::this_thread::sleep_for(std::chrono::seconds(rand()%5));
			std::this_thread::sleep_for(std::chrono::microseconds(rand()%100));
			lck.unlock();
		}//end of while
		if(mExit)
		{
			break;
		}
	}//end of while
}

void logSeedGridContent(int pSeedGrid[][YSIZE], stringstream& ss)
{
	for(int j=0; j<YSIZE; j++)
	{
		//cout << "  ";
		for(int i=0; i<XSIZE; i++)
		{
			ss << pSeedGrid[i][j] << " ";
		}
		ss << "\n";
	}
}

void gameofLife(Record& record, void* pGridSpace, int id, int seed_num)
{
	Grid<int, XSIZE, YSIZE>* pSpace = (Grid<int, XSIZE, YSIZE>*) pGridSpace;
	int seedGrid[XSIZE][YSIZE];
	int rx=0, ry=0, emc2=0;
	int largest_x=0, largest_y=0, largest_m=0;
	bool largest_found=false;
	int total_m=0, crit_m=CRITICAL_MASS;

	srand(time(0)); //pseudo-random number generator with time seed
	std::vector<int> vx, vy, vm; //vx: x-coordinate, vy: y-coordinate, vm: magnitude
	std::random_device rd;
	std::mt19937 gamma(rd());
 
	for(int i=0; i<XSIZE; i++) vx.push_back(i);
	for(int j=0; j<YSIZE; j++) vy.push_back(j);
	for(int k=0; k<MSIZE; k++) vm.push_back(rand()%5);
	
	for(int i=0; i<XSIZE; i++) for(int j=0; j<YSIZE; j++) seedGrid[i][j]=0;

	for(int fid=0; fid<FRAME_NUM; fid++)
	{
		std::shuffle(vx.begin(), vx.end(), gamma);
		std::shuffle(vy.begin(), vy.end(), gamma);
		std::shuffle(vm.begin(), vm.end(), gamma);

		for(int it=0; it<seed_num; it++)
		{
			seedGrid[vx[it]][vy[it]] = vm[it];
		}

		{
			unique_lock<std::mutex> lck(gMtx); //lock
			for(int it=0; it<seed_num; it++)
			{
				emc2 = pSpace->getElemAtLock(vx[it], vy[it]);
				pSpace->setElemAtLock(vx[it], vy[it], vm[it]+emc2);
			}

			for(int it=0; it<seed_num; it++)
			{
				largest_x=vx[it];
				largest_y=vy[it];
				largest_m=vm[it];
				largest_found=false;

				for(int dy=-1; dy<=1; dy++)
				{
					for(int dx=-1; dx<=1; dx++)
					{
						rx=vx[it]+dx;
						ry=vy[it]+dy;
						if(rx >= 0 && ry >= 0 && rx < XSIZE && ry < YSIZE)
						{
							emc2 = pSpace->getElemAtLock(rx, ry);
							if(emc2 > vm[it])
							{
								largest_x=rx;
								largest_y=ry;
								largest_m=emc2;
								largest_found=true;
							}
						}
					}
				}
				if(largest_found)
				{
					total_m = largest_m + vm[it];
					if(total_m > crit_m) total_m = crit_m;
					pSpace->setElemAtLock(largest_x, largest_y, total_m);
					pSpace->setElemAtLock(vx[it], vy[it], 0);
				}
			}
			gCV.notify_all(); //unlock
		}

		stringstream ss; //basic_stringstream<char> ss
		ss << ">> Updating World Grid Board after [Player(Thread) = " << id << ", Move = " << fid << "]\n";
		if(SEEDMAP_DISPLAY)
		{
			ss << "\nSeeds:\n";
			logSeedGridContent(seedGrid, ss);
		}
		ss << "\nResult after interactions:\n";
		pSpace->logGridContentLock(ss);
		record.logToQueue(ss.str());
	}
}


