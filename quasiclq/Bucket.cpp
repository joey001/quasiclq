#include "Bucket.h"
#include <cassert>
#include<cstdlib>
#include <cstring>
#include <cstdio>


Bucket::Bucket(int numItem, int maxVal)
{
	assert(numItem >= 0 && maxVal >= 0);
	this->numItems = numItem;
	this->maxSlotRange = maxVal + 1;

	itemPos = new Cell*[numItem];
	for (int i = 0; i < numItem; i++) {
		itemPos[i] = new Cell;
		itemPos[i]->prev = nullptr;
		itemPos[i]->next = nullptr;
		itemPos[i]->item = i;
		itemPos[i]->val = InvalidValue;
		itemPos[i]->isInSlot = 0;
	}
	slots = new Cell*[maxSlotRange];
	for (int i = 0; i < maxSlotRange; i++) {
		slots[i] = nullptr;
	}
	minNonEmptySlot = maxSlotRange; //invalid position, indicating empty bicket
}

Cell * Bucket::getFirstItem(int val)
{
	if (val < 0 || val >= maxSlotRange) {
		fprintf(stderr, "Value %d out of range\n", val);
		return nullptr;
	}
	return slots[val]; // return the next rather than the current
}

Cell * Bucket::getMinFirstItem()
{
	return slots[minNonEmptySlot];
}

int Bucket::minNonemptyVal()
{
	return minNonEmptySlot;
}

int Bucket::addItem(int it)
{
	assert(it >= 0 && it < numItems);
	assert(itemPos[it]->val != InvalidValue);
	/*if (itemPos[it]->val == InvalidValue) {
		fprintf(stderr, "Item %d is in slot\n", it);
		return -1;
	}*/
	if (itemPos[it]->isInSlot == 1) {
		fprintf(stderr, "Item %d is in slot\n", it);
		return it;
	}
	else {
		int val = itemPos[it]->val;
		Cell* snext = slots[val];
		slots[val] = itemPos[it];
		itemPos[it]->prev = nullptr;
		itemPos[it]->next = snext;
		if (snext != nullptr) {
			snext->prev = itemPos[it];
		}
		itemPos[it]->isInSlot = 1;
		if (val < minNonEmptySlot) {
			minNonEmptySlot = val;
		}
		return it;
	}
}

int Bucket::removeItem(int it)
{
	assert(it >= 0 && it < numItems);
	assert(itemPos[it]->val != InvalidValue);
	if (itemPos[it]->isInSlot == 0) {
		fprintf(stderr, "Item %d is not in slot\n", it);
		return -1;
	}
	else {
		int val = itemPos[it]->val;
		Cell* inext = itemPos[it]->next;
		Cell* iprev = itemPos[it]->prev;
		itemPos[it]->next = nullptr;
		itemPos[it]->prev = nullptr;
		if (inext != nullptr) {
			inext->prev = iprev;
		}		
		if (iprev != nullptr) {
			iprev->next = inext;
		}
		else {
			slots[val] = inext;
			if (inext  == nullptr && minNonEmptySlot == val) { //&& iprev == nullptr
				while (slots[minNonEmptySlot] == NULL) {
					minNonEmptySlot++;
					if (minNonEmptySlot == maxSlotRange) break;
				}
			}
		}
		itemPos[it]->isInSlot = 0;
		return val;
	}
}


int Bucket::resetItemValue(int it, int val)
{
	assert(it >= 0 && it < numItems && val >= 0 && val < maxSlotRange);
	if (itemPos[it]->val == val) {
		fprintf(stderr, "Item %d has the same value %d\n", it, val);
		return 0;
	}
	//Optimize
	if (itemPos[it]->isInSlot) {		
		removeItem(it);
		itemPos[it]->val = val;
		addItem(it);
	}else{
		itemPos[it]->val = val;
	}
	return 1;
}

int Bucket::increaseUnitItemValue(int it)
{
	assert(it >= 0 && it < numItems);
	int val = getItemValue(it);
	assert(val <= maxSlotRange);
	if (itemPos[it]->isInSlot) {
		removeItem(it);
		itemPos[it]->val = val + 1;
		addItem(it);
	}
	else {
		itemPos[it]->val++;
	}
	return 1;
}

int Bucket::decreaseUnitItemValue(int it)
{
	assert(it >= 0 && it < numItems);
	int val = getItemValue(it);
	assert(val > 0);
	if (itemPos[it]->isInSlot) {
		removeItem(it);
		itemPos[it]->val = val - 1;
		addItem(it);
	}
	else {
		itemPos[it]->val--;
	}
	return 1;
}

int Bucket::getItemValue(int item)
{
	assert(item >= 0 && item < numItems);
	return itemPos[item]->val;
}



void Bucket::db_ShowBucket()
{
	for (int i = 0; i < maxSlotRange; i++) {
		Cell* pcell = getFirstItem(i);
		if (pcell != nullptr) {
			printf("Slot[%d]:", i);
			while (pcell != nullptr) {
				printf("%d ", pcell->item);
				pcell = pcell->next;
			}
			printf("\n");
		}
	}
	printf("Min val: %d\n", minNonEmptySlot);
}

int Bucket::isEmpty()
{
	return minNonEmptySlot == maxSlotRange;
}

Bucket::~Bucket()
{
	//TODO
}

void test1()
{
	//10 items maxmium vlaue 20
	Bucket bkt(10, 20);

	bkt.resetItemValue(1, 20);
	bkt.resetItemValue(2, 10);
	bkt.resetItemValue(3, 10);
	
	bkt.addItem(1);
	bkt.addItem(2);
	bkt.addItem(3);
	int minval = bkt.minNonemptyVal();
	assert(minval== 10);
	
	Cell* pcell = bkt.getFirstItem(minval);
	while (pcell != nullptr) {
		printf("%d\n", pcell->item);
		pcell = pcell->next;
	}

	bkt.resetItemValue(1, 9);
	pcell = bkt.getMinFirstItem();
	while (pcell != nullptr) {
		printf("%d\n", pcell->item);
		pcell = pcell->next;
	}

	bkt.removeItem(2);

	bkt.addItem(2);
	bkt.resetItemValue(4, 9);
	bkt.addItem(4);
	bkt.resetItemValue(1, 10);
	bkt.db_ShowBucket();
	
	printf("Remove 4\n");
	bkt.removeItem(4);
	bkt.db_ShowBucket();
}

int mainXXXX() {
	test1();
	return 0;
}