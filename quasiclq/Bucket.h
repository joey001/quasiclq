#pragma once
#include<climits>
struct ST_Cell {
	ST_Cell* next;
	ST_Cell* prev;
	int val;
	int item;	
	int isInSlot;
public:
	ST_Cell() {}
};

typedef ST_Cell Cell;

class Bucket
{
private:
	//struct Cell;
	int numItems; // Number of items
	//int* itemVal; // the value of items

	int maxSlotRange; // the maximum slots
	//int* szSlots;	
	Cell** slots; //The slots of cells
	Cell** itemPos;
	int minNonEmptySlot;
	//Cell* cells;
public:
	const int InvalidValue = INT_MAX;

	Bucket(int numItem, int maxVal);
	Cell* getFirstItem(int val);
	Cell* getMinFirstItem();
	int minNonemptyVal();	//get the maximum value
	
	int addItem(int it);
	int removeItem(int it);
	int resetItemValue(int item, int val);
	int getItemValue(int item);
	//int changeItemVal(int it, int delta);
	void db_ShowBucket();
	~Bucket();
};

void test1();


