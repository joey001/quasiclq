/*
 * utils.hpp
 *
 *  Created on: Mar 7, 2016
 *      Author: zhou
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_
#pragma warning(disable : 4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <sstream>
#include <list>
#include <utility>
#include <set>
using namespace std;
/*Define the structure*/
typedef struct ST_RandAccessList{
	int *vlist;
	int *vpos;
	int vnum;
	int capacity;
}RandAccessList;
#define LARGE_INT 99999999
/*Parameters*/
//char param_graph_file_name[1024] = "instances\\brock200_2.clq";
extern char param_graph_file_name[1024];
extern double param_gamma;
extern int param_best;
extern int param_max_secs;
extern unsigned int param_seed;


/***************utils*********************/
RandAccessList* ral_init(int capacity);
void ral_add(RandAccessList *ral, int vid);
void ral_delete(RandAccessList *ral, int vid);
void ral_clear(RandAccessList *ral);
void ral_release(RandAccessList *ral);
int ral_contains(RandAccessList *ral, int vid);
int cmpfunc (const void * a, const void * b);
void ral_showList(RandAccessList *ral, FILE *f);


extern int GVNum;	//The number of vertices in the input graph
extern int GENum;	//The number of edges in the input graph
extern int GFmt;	// The format, currently unimportant
extern int* GNumNbrs;
extern int** GNbrLst;

const char* file_suffix(char* filename);
const char* file_basename(char* filename);

int load_clq_instance(char* filename);
int load_snap_instance(char *filename);
int load_metis_instance(char* filename);
//void shuffle(int *randlist, int len, int randlen);
double computeDensity(set<int> sol);


#endif /* UTILS_HPP_ */
