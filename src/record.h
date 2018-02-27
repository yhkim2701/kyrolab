/* =========================================================================
 *
 * 'Project_ID:       kyrolab
 * 'Filename:         $record.h $ 
 * 'Title:            
 * 'System:           Unix
 * 'Entry_Point:      main
 * 'Author:           $Author: Yong Hyun Kim $
 * 'Creation_Date:    2015/09/21
 * 
 * 'Description:
 *
 * 'General Notes:
 *
 * 'End
 * ========================================================================= */

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <random> //std::default_random_engine
#include <algorithm> //std::shuffle
#include <chrono>

#include "./config.h"

/*
#include <sys/time.h>
#include <sys/socket.h> //Socket definitions
#include <sys/types.h> //Socket types
#include <sys/wait.h> //For waitpid()
#include <arpa/inet.h> //Inet (3) funtions
#include <unistd.h> //UNIX functions      
#include <string.h> //memset
#include <stdlib.h> //exit
*/

using namespace std;

class Record
{
public:
	//Begins a background thread writing record entries to a file
	Record(const std::string& fname);
	//Nicely shut down background thread
	virtual ~Record();
	//Copy construction and assignment prevented
	Record(const Record& src) = delete;
	Record& operator=(const Record& rhs) = delete;
	//Make record entry to the queue
	void logToQueue(const std::string& entry);

private:
	//Function running in the background thread
	void logToFile();
	//Background thread
	std::thread mThread;
	std::mutex mMutex;
	std::condition_variable mCondVar;
	std::queue<std::string> mQueue;
	//Boolean telling the background thread to terminate
	std::atomic<bool> mExit;
	std::string mFileName;
};

void logSeedGridContent(int pSeedGrid[][YSIZE], stringstream& ss);
void gameofLife(Record& record, void* pGridSpace, int id, int seed_num);


