
#include "utils.hpp"


using namespace std;


//------------------------Algorithm- MaQuasiClq--------------------------------
namespace SimpleLS {
	RandAccessList* solset; // the solution set
	int solNe; // number of edges in the solution
	int *lbNumEdge;
	int *nbrInSol; //number of adjacent edges in the solution
	RandAccessList* cands; // a list of vertices who is adjacent to all vertices in solution
	set<int> bestSol;
	clock_t clkStart;
	int steps;

	void init() {
		solset = ral_init(GVNum);
		cands = ral_init(GVNum);
		nbrInSol = new int[GVNum];
		memset(nbrInSol, 0, sizeof(int)* GVNum);
		solNe = 0;

		lbNumEdge = new int[GVNum];
		lbNumEdge[0] = 0;
		for (int i = 0; i < GVNum; i++) {
			//ral_add(cands, i);
			if (i > 0)
				lbNumEdge[i] = (int)ceil(param_gamma * (i *(i - 1) / 2));

		}

	}

	void addVtx(int vtx) {
		assert(!ral_contains(solset, vtx));

		ral_add(solset, vtx);
		if (ral_contains(cands, vtx)) {
			ral_delete(cands, vtx);
		}
		solNe += nbrInSol[vtx];

		for (int i = 0; i < GNumNbrs[vtx]; i++) {
			int vnbr = GNbrLst[vtx][i];
			nbrInSol[vnbr]++;
			if (!ral_contains(solset, vnbr) && nbrInSol[vnbr] == 1) {
				ral_add(cands, vnbr);
			}
		}
	}
	void removeVtx(int vtx) {
		assert(ral_contains(solset, vtx));

		ral_delete(solset, vtx);
		if (!ral_contains(cands, vtx) && nbrInSol[vtx] > 0) {
			ral_add(cands, vtx);
		}
		solNe -= nbrInSol[vtx];

		for (int i = 0; i < GNumNbrs[vtx]; i++) {
			int vnbr = GNbrLst[vtx][i];
			nbrInSol[vnbr]--;
			if (!ral_contains(solset, vnbr) && nbrInSol[vnbr] == 0) {
				ral_delete(cands, vnbr);
			}
		}
	}



	//find an initial solution 
	int randLocalSearch() {
		steps = 0;
		clkStart = clock();
		while ((double)(clock() - clkStart) / CLOCKS_PER_SEC < param_max_secs) {
			//find the expanable vertices
			//printf("------Step %d--------\n", step);
			vector<int> vecAdd;
			//vector<int> vecSwap;
			if (cands->vnum == 0) {
				int seedvtx = rand() % GVNum;
				addVtx(seedvtx);
			}
			for (int i = 0; i < cands->vnum; i++) {
				int vi = cands->vlist[i];
				if (nbrInSol[vi] >= lbNumEdge[solset->vnum + 1] - solNe) {
					vecAdd.push_back(vi);
				}
				//else if (nbrInSol[vi] >= lbNumEdge[solset->vnum] / (solset->vnum) - 1) { // average degree
				//	vecSwap.push_back(vi);
				//}
			}
			if (vecAdd.size() > 0) {
				int v = vecAdd[rand() % vecAdd.size()];
				addVtx(v);
				//printf("Add %d \n", v);
			}
			else {
				int randv = solset->vlist[rand() % solset->vnum];
				removeVtx(randv);
				//printf("Remove %d\n", randv);
			}

			//printf("sol set :\n");
			//ral_showList(solset, stdout);
			//printf("cand set:\n");
			//ral_showList(cands,stdout);
			if (solset->vnum > (int)bestSol.size()) {
				bestSol.clear();
				for (int i = 0; i < solset->vnum; i++) {
					bestSol.insert(solset->vlist[i]);
				}
				bestSol.insert(solset->vlist, solset->vlist + solset->vnum);
				printf("Step %d: best size %d \n", steps, bestSol.size());
			}

			steps++;
		}
		return 0;
	}

	int main() {
		const char* fileext = file_suffix(param_graph_file_name);
		int load = 0;
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
		/*for (int v = 0; v < GVNum; v++) {
			printf("%d [%d]: ", v, GNumNbrs[v]);
			for (int i = 0; i < GNumNbrs[v]; i++) {
				printf("%d ", GNbrLst[v][i]);
			}
			printf("\n");
		}*/
		SimpleLS::init();
		randLocalSearch();

		double bestDens = computeDensity(bestSol);
		double seconds = (double)(clock() - clkStart) / CLOCKS_PER_SEC;
		printf("Best density %.3lf\n", bestDens);
		if (bestDens < param_gamma) {
			printf("Wrong solutoion\n");
			return 1;
		}
		else {
			printf("Graph %s: best sol %d, after %d steps, %.2lf seconds\n",
				file_basename(param_graph_file_name),
				bestSol.size(), steps, seconds);

		}
		system("pause");
	}
}

int mainX() {
	SimpleLS::main();
	return 0;
}