#include "utils.hpp"
#include <limits.h>
#include <math.h>
#include "Bucket.h"
typedef struct {
	int v1;
	int v2;
}EdgeType;

int RNumEdges; // number of edges in the complement graph
int RNumVtx;
EdgeType* REdges;
int** RIncideEdge;
int** RNeighborVtx;
int* RNumNbrs; // the number of edges for each vertex

int* ubEdgeNum;
//int numSolEdge;
RandAccessList* vioEdges; // violated edges
RandAccessList* solSet; // current solution set
//int* nNbrInSol;
Bucket* pbkt;
long long* tabuIter; // the number of tabu iterations
long long iteration = 0;
int numMinVioEdges;
long long *lastMove;

int* startSolMark; // mark the start solution
int startSim;	//record similarity.


clock_t clkStart;
clock_t clkEnd;
int numBestSolEge;
set<int> bestSol;
int bestIter;
clock_t bestClk;


void init() {
	//Transfer the graph into complement graph
	RNumVtx = GVNum;
	RNumEdges = (GVNum*(GVNum - 1)) / 2 - GENum;
	REdges = new EdgeType[RNumEdges];
	RNumNbrs = new int[RNumVtx];
	RIncideEdge = new int*[RNumVtx];
	RNeighborVtx = new int*[RNumVtx];
	int e = 0;
	for (int v = 0; v < GVNum; v++) {
		RNumNbrs[v] = GVNum - GNumNbrs[v] - 1;
		RIncideEdge[v] = new int[RNumNbrs[v]];
		RNeighborVtx[v] = new int[RNumNbrs[v]];
		int *mark = new int[GVNum];
		memset(mark, 0, sizeof(int) *GVNum);
		for (int i = 0; i < GNumNbrs[v]; i++) {
			mark[GNbrLst[v][i]] = 1;
		}
		//mark[v] = 1; 
		for (int u = v + 1; u < GVNum; u++) {
			if (mark[u] == 0) {
				REdges[e].v1 = v;
				REdges[e].v2 = u;
				e++;
			}
		}
		delete[] mark;
	}
	assert(RNumEdges == e);
	int* tmpEdgeCnt = new int[RNumVtx];
	memset(tmpEdgeCnt, 0, sizeof(int) * RNumVtx);
	for (int e = 0; e < RNumEdges; e++) {
		int v1 = REdges[e].v1;
		int v2 = REdges[e].v2;
		RIncideEdge[v1][tmpEdgeCnt[v1]] = e;
		RNeighborVtx[v1][tmpEdgeCnt[v1]] = v2;
		tmpEdgeCnt[v1]++;

		RIncideEdge[v2][tmpEdgeCnt[v2]] = e;
		RNeighborVtx[v2][tmpEdgeCnt[v2]] = v1;
		tmpEdgeCnt[v2]++;
	}
	/*
	for (int v = 0; v < RNumVtx; v++) {
		printf("%d [%d]: ", v, RNumNbrs[v]);
		for (int i = 0; i < RNumNbrs[v]; i++) {
			printf("%d ", RNeighborVtx[v][i]);
		}
		printf("\n");
	}*/

	//Allocate memory for data
	solSet = ral_init(RNumVtx);
	//numSolEdge = 0;
	vioEdges = ral_init(RNumEdges);
	//nNbrInSol = new int[RNumVtx];
	//memset(nNbrInSol, 0, sizeof(int)* RNumVtx);
	pbkt = new Bucket(RNumVtx, RNumVtx);

	ubEdgeNum = new int[RNumVtx+1];
	for (int i = 0; i < RNumVtx+1; i++) {
		ubEdgeNum[i] = (int)floor((1 - param_gamma) * (i*(i - 1)) / 2);
	}
	tabuIter = new long long[RNumVtx];
	memset(tabuIter, 0, sizeof(long long) * RNumVtx);

	//vetex age
	lastMove = new long long[RNumVtx];
	memset(lastMove, 0, sizeof(long long) *RNumVtx);

	iteration = 1;

	startSim = 0;
	startSolMark = new int[RNumVtx];
	memset(startSolMark, 0, sizeof(int) *RNumVtx);
	srand(param_seed);
}

void addVtx(int v) {
	assert(!ral_contains(solSet, v));
	ral_add(solSet, v);
	//numSolEdge += nNbrInSol[v];
	pbkt->removeItem(v);

	for (int i = 0; i < RNumNbrs[v]; i++) {
		int vnbr = RNeighborVtx[v][i];
		//nNbrInSol[vnbr] += 1;
		pbkt->increaseUnitItemValue(vnbr);
		int eicd = RIncideEdge[v][i];
		if (ral_contains(solSet, vnbr)) {
			ral_add(vioEdges, eicd);
		}		
	}
}

void removeVtx(int v) {
	assert(ral_contains(solSet, v));
	ral_delete(solSet, v);
	//numSolEdge -= nNbrInSol[v];
	pbkt->addItem(v);

	for (int i = 0; i < RNumNbrs[v]; i++) {
		int vnbr = RNeighborVtx[v][i];
		//nNbrInSol[vnbr]--;
		pbkt->decreaseUnitItemValue(vnbr);

		int ecid = RIncideEdge[v][i];
		if (ral_contains(solSet, vnbr)) {
			ral_delete(vioEdges, ecid);
		}
	}
}
/*Find an init solution by greedy*/
int createInitSol() {
	while (1) {
		//the vertex of minimal weight, breaking ties in favor of the minimal degree
		int mindeg = INT_MAX;
		int va = -1;
		Cell* pcell = pbkt->getMinFirstItem();
		while (pcell != nullptr) {
			assert(pcell->isInSlot);
			if (RNumNbrs[pcell->item] < mindeg) {
				va = pcell->item;
				mindeg = RNumNbrs[pcell->item];
			}
			else if (RNumNbrs[pcell->item] < mindeg && rand() %2) {
				va = pcell->item;
			}
			pcell = pcell->next;
		}

		if (va != -1 && vioEdges->vnum + pbkt->getItemValue(va) <= ubEdgeNum[solSet->vnum + 1]) {
			addVtx(va);
			//printf("Add vertex %d\n", va);
		}
		else {
			break;
		}
	}

	return 0;
}

//the vertex of minimal weight, breaking ties in favor of the last moved one
int fastFindMinPosVertex() {
	Cell *p = pbkt->getMinFirstItem();
	return p->item;
}

int fastFindMinDeltaNonTabu() {
	int minval = pbkt->minNonemptyVal();
	int find = 0;
	int va = -1;
	while (!find) {
		//tabu aspiration, return the last moved item, which is locate at the end of the linklist,
		//TODO: imporve the search by make bucket bidirection list.
		if (minval + vioEdges->vnum < numMinVioEdges) {
			Cell *p = pbkt->getFirstItem(minval);
			while (p != nullptr) {
				va = p->item;
				p = p->next;				
			}
			find = 1;
		}
		else {
			Cell *p = pbkt->getFirstItem(minval);
			long long age = LLONG_MAX;
			while (p != nullptr) {
				if (iteration >= tabuIter[p->item] && lastMove[p->item]> age) {
					va = p->item;
					age = lastMove[p->item];
				}
			}
			if (va != -1) find = 1;
		}
		minval++;
	}
	assert(minval != RNumVtx);
	return va;
}
/*
int findMinDeltaNonTabu() {
	int minval = INT_MAX;
	int vout = -1;
	//TODO: optimization
	for (int v = 0; v < RNumVtx; v++) {
		//if (v == 199)
		//	printf("Pause\n");
		if (!ral_contains(solSet, v)) {
			if ((nNbrInSol[v] < minval && iteration >= tabuIter[v]) || nNbrInSol[v] + vioEdges->vnum < numMinVioEdges) {
				vout = v;
				minval = nNbrInSol[v];
			}
			else if (nNbrInSol[v] == minval && iteration >= tabuIter[v]) {
				if (lastMove[v] < lastMove[vout])
					vout = v;
			}
		}
	}
	return vout;
}*/

long long tabuValue(int v) {
	//return iteration + 11 + (int)(rand() % bestSol.size()*0.4);
	return iteration + 1;
}

int findOldestFromSol() {
	long long maxage = LLONG_MAX;
	int va = -1;
	for (int i = 0; i < solSet->vnum; i++) {
		if (lastMove[solSet->vlist[i]] < maxage) {
			maxage = lastMove[solSet->vlist[i]];
			va = solSet->vlist[i];
		}
		else if (lastMove[solSet->vlist[i]] == maxage) {
			va = rand() % 2 ? va : solSet->vlist[i];
		}
	}

	return va;
}
void mildPerturbation() {
	int va = findOldestFromSol();
	assert(va != -1);
	removeVtx(va);
	tabuIter[va] = tabuValue(va);

	int vb = fastFindMinDeltaNonTabu();
	addVtx(vb);
	lastMove[vb] = iteration;
}

int strongPerturbation() {
	int maxstp = solSet->vnum - 1; //remove until the last one
	int step = 0;

	while (step < maxstp) {
		//remove the oldest	vertex
		int va = findOldestFromSol();
		removeVtx(va);
		tabuIter[va] = tabuValue(va);

		//Since the iteration counter does not move,
		//it is not possible to find the vertex which are removed.
		int vb = fastFindMinDeltaNonTabu();
		addVtx(vb);
		lastMove[vb] = iteration;
		//printf("Perturb step %d: rm %d add %d\n", tmp_cnt++, va, vb);
		//The iteration counter is stopped
		step++;
	}
	return step;
}

void incrementLegalSol() {
	//record
	while (vioEdges->vnum <= ubEdgeNum[solSet->vnum]) {
		if (solSet->vnum > (int)bestSol.size()) {
			bestSol.clear();
			bestSol.insert(solSet->vlist, solSet->vlist + solSet->vnum);
			bestIter = iteration;
			bestClk = clock();
			numBestSolEge = vioEdges->vnum;
			printf("Iteration %lld: Legal sol: %d, Best sol: %d\n", iteration, solSet->vnum, (int)bestSol.size());
		}
		//augment a vertex
		int va = fastFindMinPosVertex();
		if (va != -1) {
			addVtx(va);
			lastMove[va] = iteration;
			//printf("Add %d to expand legal\n", va);
			numMinVioEdges = vioEdges->vnum; // update the minimum violated edges
		}
	}
	//records solution
	memset(startSolMark, 0, sizeof(int)* RNumVtx);
	for (int i = 0; i < solSet->vnum; i++) {
		startSolMark[solSet->vlist[i]] = 1;
	}
	startSim = solSet->vnum;

}


void randomTabuSearch() {
	init();
	//stop if the whole graph is a legal quasi-c
	if (RNumEdges <= ubEdgeNum[RNumVtx]) {
		bestSol.clear();
		for (int v = 0; v < RNumVtx; v++)
			bestSol.insert(v);
		bestIter = iteration;
		bestClk = clock();
		numBestSolEge = RNumEdges;		
		return;
	}
	createInitSol();	
	for (int i = 0; i < solSet->vnum; i++) {
		lastMove[solSet->vlist[i]] = iteration;
	}

	clkStart = clock();
	int isEnd = 0;
	int unImporveIt = 0;
	while (!isEnd) {
		//printf("\n---------------Iteration %lld: Best %d--------\n", iteration, bestSol.size());
		//ral_showList(solSet, stdout);
		if (iteration % 1000 == 0) {
			if ((double)(clock() - clkStart) / CLOCKS_PER_SEC > param_max_secs) {
				isEnd = 1;
			}
			//printf("Iteration %lld: sol size %d, violate percent %.2llf\n", iteration, solSet->vnum, (double)vioEdges->vnum/ubEdgeNum[solSet->vnum]);
		}

		//A legal solution is found, expand the solution until it is illegal
		if (vioEdges->vnum <= ubEdgeNum[solSet->vnum]) {
			incrementLegalSol();
		}
		//Rest the known best solution
		if (vioEdges->vnum < numMinVioEdges) {
			numMinVioEdges = vioEdges->vnum;
			unImporveIt = 0;
		}
		else {
			unImporveIt++;
		}

		if (unImporveIt >= solSet->vnum) {
			int maxstp = bestSol.size() * 0.1 + 1; // ensure a minimum pertrub step of 1
			while (maxstp--) {
				mildPerturbation();
				iteration++;
			}			
			unImporveIt = 0;
		}
		else { // local search
		   //In solution, select a random edge, select the old vertex (smaller ageVtx)
			assert(vioEdges->vnum > 0);
			int erand = vioEdges->vlist[rand() % vioEdges->vnum];
			int va = -1;
			if (lastMove[REdges[erand].v1] < lastMove[REdges[erand].v2]) {
				va = REdges[erand].v1;
			}
			else {
				va = REdges[erand].v2;
			}
			removeVtx(va);
			tabuIter[va] = tabuValue(va);
			//printf("Remove %d\n", va);

			//Our solution, find the minium delta
			int vb = fastFindMinDeltaNonTabu();
			if (vb != -1) {
				addVtx(vb);
				lastMove[vb] = iteration;
				//printf("Add %d \n", vb);
			}
			iteration++;
		}

	}
	clkEnd = clock();
}

void showUsage() {
	fprintf(stderr, "mqclq -f <filename> -s <parameter gamma> -t <max seconds>  [-o optimum object] [-c seed]");
}

int read_params(int argc, char **argv) {
	int hasFile = 0;
	int hasTimeLimit = 0;
	int isGammaValid = 0;
	for (int i = 1; i < argc; i += 2) {
		if (argv[i][0] != '-' || argv[i][2] != 0) {
			showUsage();
			exit(0);
		}
		else if (argv[i][1] == 'f') {
			strncpy(param_graph_file_name, argv[i + 1], 1000);
			hasFile = 1;
		}
		else if (argv[i][1] == 'k') {
			param_gamma = atof(argv[i + 1]);
			if (0 <= param_gamma && param_gamma <= 1)
				isGammaValid = 1;
		}
		else if (argv[i][1] == 'o') {
			param_best = atoi(argv[i + 1]);
		}
		else if (argv[i][1] == 'c') {
			param_seed = atoi(argv[i + 1]);
		}
		else if (argv[i][1] == 't') {
			param_max_secs = atoi(argv[i + 1]);
			hasTimeLimit = 1;
		}
	}
	/*check parameters*/
	/*
	if (!hasFile) {
		fprintf(stderr, "No file name\n");
		showUsage();
		return 0;
	}
	if (!hasTimeLimit) {
		fprintf(stderr, "No time limit \n");
		showUsage();
		return 0;
	}
	if (!isGammaValid) {
		fprintf(stderr, "No valid paramete gama\n");
		showUsage();
		return 0;
	}*/
	return 1;
}

void showResult() {
	printf("\n");
	printf("$seed=%d\n", param_seed);
	printf("@solver=splex\n");
	printf("@para_file=%s\n", param_graph_file_name);
	printf("@para_gamma=%.2f\n", param_gamma);
	printf("@para_kbv=%d\n", param_best);
	printf("@para_seconds=%d\n", param_max_secs);
	printf("#vnum=%d\n", GVNum);
	printf("#enum=%d\n", GENum);
	printf("#dens=%.3f\n", ((double)2 * GENum / (GVNum*(GVNum - 1))));
	printf("#objv=%d\n", (int)bestSol.size());
	printf("#bestiter=%d\n", bestIter);
	printf("#besttime=%.3f\n", (double)(bestClk - clkStart) / CLOCKS_PER_SEC);
	printf("#totaliter=%lld\n", iteration);
	printf("#totaltime=%.3f\n", (double)(clkEnd - clkStart) / CLOCKS_PER_SEC);
	printf("Solution:");
	for (set<int>::iterator it = bestSol.begin();
		it != bestSol.end(); it++) {
		printf("%d ", *it);
	}
	printf("\n");
}

int mainXX(int argc, char** argv) {
	int load = 0;

	read_params(argc, argv); //read parameters

	const char* fileext = file_suffix(param_graph_file_name);
	//printf("%s\n",fileext);
	if (0 == strcmp(fileext, "graph")) {
		load = load_metis_instance(param_graph_file_name);
	}
	else if (0 == strcmp(fileext, "txt")) {
		load = load_snap_instance(param_graph_file_name);
	}
	else if (0 == strcmp(fileext, "clq")) {
		load = load_clq_instance(param_graph_file_name);
	}
	if (load != 1) {
		fprintf(stderr, "failed in loading graph %s\n", param_graph_file_name);
		exit(-1);
	}
	randomTabuSearch();

	double bestDens = computeDensity(bestSol);

	printf("Best density %.3lf\n", bestDens);
	if (bestDens < param_gamma) {
		printf("Wrong solutoion\n");
	}
	else {
		//printf("Graph %s: best sol %d, after %d steps, %f seconds\n",
		//	file_basename(param_graph_file_name),
		//	bestSol.size(), iteration, seconds);
		showResult();
	}
	//system("pause");
	return 0;
}
