/* =========================================================================
 * EXECUTIVE SUMMARY
 *
 * 'Project_ID:       kyrolab
 * 'Filename:         $kyrolab.cpp $ 
 * 'Title:            
 * 'System:           Unix
 * 'Entry_Point:      main
 * 'Author:           $Author: Yong Hyun Kim $
 * 'Creation_Date:    2015/09/21
 * 
 * 'Description:
 *    This program features multithreaded process to simulate Game of Life in 2D grid space.
 *    Each thread 'gameofLife' represents a parallel space where a certain number
 *    of seed agents (life forms or particles) enter the stage (2D grid world) and interact with other seed agents
 *    from other parallel spaces. Interaction is defined by some specific rule.
 *
 * 'Inputs:
 *    ''Name:
 *    ''Type:
 *    ''Description:
 * 
 * 'Outputs:
 *    ''Name:
 *    ''Type:
 *    ''Description:
 *
 * 'General Notes:
 *
 * 'End
 * ========================================================================= */

#include "./record.h"
#include "./tensor.h"
#include "./config.h"

using namespace std;

void _print_err()
{
	printf("> Illegal command.\n");
	printf(">  Usage: ./kyrolab <filename>\n");
}

/***************************************************
	Main Entry Point
****************************************************/
int main(int argc, char* argv[])
{
	//Main entry point of Multi-threads proc
	char fname[FILENAME_SIZE];
	int aSeedNum[THREAD_NUM] = {SEED_SIZE0,SEED_SIZE1,SEED_SIZE2,SEED_SIZE3};

	if(argc != 2)
	{
		_print_err();
		return 0;
	}
	sprintf(fname, "%s", argv[1]);

	Grid<int, XSIZE, YSIZE> gridSpace;
	void *pGridSpace;
	pGridSpace = &gridSpace;

	Record record(fname);
	vector<thread> thbank;
	//Create few threads all working with the same record instance.
	for(int i=0; i<THREAD_NUM; i++)
	{
		thbank.emplace_back(gameofLife, ref(record), pGridSpace, i, aSeedNum[i]);
	}
	//Wait for all threads to finish.
	for(auto& th : thbank)
	{
		th.join();
	}

	if(DISPLAY_FINAL_FRAME)
	{
		gridSpace.displayGridContent();
		cout << endl;
	}

	/***************************************************
	For Testing Purpose Only (not part of the program):
	subject to template class
	****************************************************/
	if(DISPLAY_TEST_COPY_CONSTRUCTOR)
	{
		Grid<int> board0;
		board0.displayGridContent();
		cout << endl;

		Grid<int,6,6> board1;
		board1.displayGridContent();
		cout << endl;
		board1.initializeGrid(5);
		board0 = board1;
		board0.displayGridContent();
		cout << endl;

		Grid<int,8,8> board2(board1);
		board2.displayGridContent();
		cout << endl;
	}
	/********************************************/

	return 0;
}


