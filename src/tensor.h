/* =========================================================================
 *
 * 'Project_ID:       kyrolab
 * 'Filename:         $tensor.h $ 
 * 'Title:            
 * 'System:           Unix
 * 'Entry_Point:      main
 * 'Author:           $Author: Yong Hyun Kim $
 * 'Creation_Date:    2015/09/21
 * 
 * 'Description: Grid (used for 2nd degree tensor or metric tensor) class definition 
 *               & implementation. This class will be used as the base class for derived
 *               classes such as matrices, 2D grid space, etc.
 *
 * 'General Notes:
 *
 * 'End
 * ========================================================================= */

#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>

using namespace std;

/*-----------------------------------------------------------------------
	Template 2D grid (2nd degree tensor) class definition
-----------------------------------------------------------------------*/
template <typename T, size_t WIDTH0 = 10, size_t HEIGHT0 = 10>
class Grid
{
public:
	Grid() : mCell() {} //zero-initialize cell
	virtual ~Grid(); //destructor

	//copy constructor
	template <typename S, size_t WIDTH1, size_t HEIGHT1>
	Grid(const Grid<S, WIDTH1, HEIGHT1>& src);

	//assignment operation
	template <typename S, size_t WIDTH1, size_t HEIGHT1>
	Grid<T, WIDTH0, HEIGHT0>& operator=(const Grid<S, WIDTH1, HEIGHT1>& rhs);

	size_t getWidth() const {return WIDTH0;}
	size_t getHeight() const {return HEIGHT0;}

	//read, write without mutex
	void setElemAt(size_t x, size_t y, const T& inElem);
	T getElemAt(size_t x, size_t y);
	const T getElemAt(size_t x, size_t y) const;
	void initializeGrid(const T& inElem);
	void displayGridContent();
	void logGridContent(stringstream& ss);

	//read, write with mutex
	void setElemAtLock(size_t x, size_t y, const T& inElem);
	T getElemAtLock(size_t x, size_t y);
	void initializeGridLock(const T& inElem);
	void displayGridContentLock();
	void logGridContentLock(stringstream& ss);

private:
	template <typename S, size_t WIDTH1, size_t HEIGHT1>
	void replicate(const Grid<S, WIDTH1, HEIGHT1>& src);
	T mCell[WIDTH0][HEIGHT0];
	std::mutex mMtx;
	std::condition_variable mCV;
};



/*-----------------------------------------------------------------------
	Constructor, Destructor, Copy, Assignment Implementation
-----------------------------------------------------------------------*/
//destructor
template <typename T, size_t WIDTH0, size_t HEIGHT0>
Grid<T, WIDTH0, HEIGHT0>::~Grid(){}

//copy constructor
template <typename T, size_t WIDTH0, size_t HEIGHT0>
template <typename S, size_t WIDTH1, size_t HEIGHT1>
Grid<T, WIDTH0, HEIGHT0>::Grid(const Grid<S, WIDTH1, HEIGHT1>& src)
{
	replicate(src);
}

//assignment operation
template <typename T, size_t WIDTH0, size_t HEIGHT0>
template <typename S, size_t WIDTH1, size_t HEIGHT1>
Grid<T, WIDTH0, HEIGHT0>& Grid<T, WIDTH0, HEIGHT0>::operator=(const Grid<S, WIDTH1, HEIGHT1>& rhs)
{
	//No need to check for self-assignment because this version of
	//assignment is never called when T and S are the same
	replicate(rhs);
	return(*this);
}

//Copy object
template <typename T, size_t WIDTH0, size_t HEIGHT0>
template <typename S, size_t WIDTH1, size_t HEIGHT1>
void Grid<T, WIDTH0, HEIGHT0>::replicate(const Grid<S, WIDTH1, HEIGHT1>& src)
{
	for(size_t i=0; i<WIDTH0; i++)
	{
		for(size_t j=0; j<HEIGHT0; j++)
		{
			if(i<WIDTH1 && j<HEIGHT1)
			{
				mCell[i][j] = src.getElemAt(i,j);
			}
			else
			{
				mCell[i][j] = T();
			}
		}
	}
}

/*-----------------------------------------------------------------------
	Non Mutex Lock Implementation
-----------------------------------------------------------------------*/
template <typename T, size_t WIDTH0, size_t HEIGHT0>
void Grid<T, WIDTH0, HEIGHT0>::setElemAt(size_t x, size_t y, const T& inElem)
{
	mCell[x][y] = inElem;
}

template <typename T, size_t WIDTH0, size_t HEIGHT0>
T Grid<T, WIDTH0, HEIGHT0>::getElemAt(size_t x, size_t y)
{
	return (mCell[x][y]);
}

template <typename T, size_t WIDTH0, size_t HEIGHT0>
const T Grid<T, WIDTH0, HEIGHT0>::getElemAt(size_t x, size_t y) const
{
	return (mCell[x][y]);
}

template <typename T, size_t WIDTH0, size_t HEIGHT0>
void Grid<T, WIDTH0, HEIGHT0>::initializeGrid(const T& inElem)
{
	for(int j=0; j< (*this).getHeight(); j++)
	{
		for(int i=0; i< (*this).getWidth(); i++)
		{
			(*this).setElemAt(i,j,inElem);
		}
	}
}

template <typename T, size_t WIDTH0, size_t HEIGHT0>
void Grid<T, WIDTH0, HEIGHT0>::displayGridContent()
{
	for(int j=0; j< (*this).getHeight(); j++)
	{
		//cout << "  ";
		for(int i=0; i< (*this).getWidth(); i++)
		{
			cout << (*this).getElemAt(i,j) << " ";
		}
		cout << endl; //cout << "\n"; cout.flush();
	}
}

template <typename T, size_t WIDTH0, size_t HEIGHT0>
void Grid<T, WIDTH0, HEIGHT0>::logGridContent(stringstream& ss)
{
	for(int j=0; j< (*this).getHeight(); j++)
	{
		//cout << "  ";
		for(int i=0; i< (*this).getWidth(); i++)
		{
			ss << (*this).getElemAt(i,j) << " ";
		}
		ss << "\n";
	}
}

/*-----------------------------------------------------------------------
	Mutex Lock Implementation
-----------------------------------------------------------------------*/
template <typename T, size_t WIDTH0, size_t HEIGHT0>
void Grid<T, WIDTH0, HEIGHT0>::setElemAtLock(size_t x, size_t y, const T& inElem)
{
	unique_lock<std::mutex> lck(mMtx);
	setElemAt(x,y,inElem);
	mCV.notify_all();
}

template <typename T, size_t WIDTH0, size_t HEIGHT0>
T Grid<T, WIDTH0, HEIGHT0>::getElemAtLock(size_t x, size_t y)
{
	T cell;
	{
		unique_lock<std::mutex> lck(mMtx);
		cell = getElemAt(x,y);
		mCV.notify_all();
	}
	return (cell);

}

template <typename T, size_t WIDTH0, size_t HEIGHT0>
void Grid<T, WIDTH0, HEIGHT0>::initializeGridLock(const T& inElem)
{
	unique_lock<std::mutex> lck(mMtx);
	for(int j=0; j< (*this).getHeight(); j++)
	{
		for(int i=0; i< (*this).getWidth(); i++)
		{
			(*this).setElemAt(i,j,inElem);
		}
	}
	mCV.notify_all();
}

template <typename T, size_t WIDTH0, size_t HEIGHT0>
void Grid<T, WIDTH0, HEIGHT0>::displayGridContentLock()
{
	unique_lock<std::mutex> lck(mMtx);
	for(int j=0; j< (*this).getHeight(); j++)
	{
		//cout << "  ";
		for(int i=0; i< (*this).getWidth(); i++)
		{
			cout << (*this).getElemAt(i,j) << " ";
		}
		cout << endl; //cout << "\n"; cout.flush();
	}
	mCV.notify_all();
}

template <typename T, size_t WIDTH0, size_t HEIGHT0>
void Grid<T, WIDTH0, HEIGHT0>::logGridContentLock(stringstream& ss)
{
	unique_lock<std::mutex> lck(mMtx);
	for(int j=0; j< (*this).getHeight(); j++)
	{
		//cout << "  ";
		for(int i=0; i< (*this).getWidth(); i++)
		{
			ss << (*this).getElemAt(i,j) << " ";
		}
		ss << "\n";
	}
	mCV.notify_all();
}

