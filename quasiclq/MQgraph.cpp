#include "utils.hpp"


/*original graph, the structure is kept the same,
* but the id of vertices are renumbered, the original
* id can be retrievaled by org_vid*/
int GVNum;	//The number of vertices in the input graph
int GENum;	//The number of edges in the input graph
int GFmt;	// The format, currently unimportant
int* GNumNbrs;
int** GNbrLst;

const char* file_suffix(char* filename) {
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) return "";
	return dot + 1;
}
const char* file_basename(char* filename) {
	const char *slash = strrchr(filename, '\\');
	if (!slash || slash == filename) return "";
	return slash + 1;

}
/**
* load instances from  2nd Dimacs competetion
*
*/
int load_clq_instance(char* filename) {
	ifstream infile(filename);
	char line[1024];
	char tmps1[1024];
	char tmps2[1024];
	/*graph adjacent matrix*/
	int **gmat;
	if (!infile) {
		fprintf(stderr, "Can not find file %s\n", filename);
		return 0;
	}

	infile.getline(line, 1024);
	while (line[0] != 'p')	infile.getline(line, 1024);
	sscanf(line, "%s %s %d %d", tmps1, tmps2, &GVNum, &GENum);

	gmat = new int*[GVNum];
	for (int i = 0; i < GVNum; i++) {
		gmat[i] = new int[GVNum];
		memset(gmat[i], 0, sizeof(int) * GVNum);
	}

	int ecnt = 0;
	GNumNbrs = new int[GVNum];
	memset(GNumNbrs, 0, sizeof(int) * GVNum);
	while (infile.getline(line, 1024)) {
		int v1, v2;
		if (strlen(line) == 0)
			continue;
		if (line[0] != 'e')
			fprintf(stderr, "ERROR in line %d\n", ecnt + 1);
		sscanf(line, "%s %d %d", tmps1, &v1, &v2);
		v1--, v2--;
		gmat[v1][v2] = 1;
		gmat[v2][v1] = 1;
		GNumNbrs[v1]++;
		GNumNbrs[v2]++;
		ecnt++;
	}
	assert(GENum == ecnt);
	GFmt = 0;
	GNbrLst = new int*[GVNum];
	for (int i = 0; i < GVNum; i++) {
		int adj_cnt = 0;
		GNbrLst[i] = new int[GNumNbrs[i]];
		for (int j = 0; j < GVNum; j++) {
			if (gmat[i][j] == 1)
				GNbrLst[i][adj_cnt++] = j;
		}
		assert(GNumNbrs[i] == adj_cnt);
		delete[] gmat[i];
	}
	printf("load 2nd dimacs graph %s with vertices %d, edges %d \n",
		file_basename(filename), GVNum, GENum);
	delete[] gmat;
	return 1;
}

/*load instances from  Stanford Large Network Dataset Collection
* URL: http://snap.stanford.edu/data/*/
int load_snap_instance(char *filename) {
	ifstream infile(filename);
	char line[1024];
	vector<pair<int, int> > *pvec_edges = new vector<pair<int, int> >();
	const int CONST_MAX_VE_NUM = 9999999;
	if (!infile) {
		fprintf(stderr, "Can not find file %s\n", filename);
		return 0;
	}
	int max_id = 0;
	int from, to;
	//int max_id = 0;
	while (infile.getline(line, 1024)) {
		char *p = line;
		while (*p == ' ' && *p != '\0') p++;
		if (*p != '#') {
			stringstream ss(line);
			ss >> from >> to;
			//printf("%d %d \n", from, to);
			pvec_edges->push_back(make_pair(from, to));
			if (from > max_id)
				max_id = from;
			else if (to > max_id)
				max_id = to;
		}
	}
	int *newid = new int[max_id + 1];
	//init the newid map
	for (int i = 0; i < max_id + 1; i++) {
		newid[i] = -1;
	}

	int v_num = 0;
	//count edges,resign ids from 1...v_num
	for (int i = 0; i < (int)pvec_edges->size(); i++) {
		from = (pvec_edges->at(i)).first;
		if (newid[from] == -1) {
			newid[from] = v_num;
			v_num++;
		}
		(pvec_edges->at(i)).first = newid[from];
		to = (pvec_edges->at(i)).second;
		if (newid[to] == -1) {
			newid[to] = v_num;
			v_num++;
		}
		(pvec_edges->at(i)).second = newid[to];
	}
	//	sort(pvec_edges->begin(), pvec_edges->end());
	//	for (int i = 0; i < pvec_edges->size(); i++){
	//		printf("%d %d\n", pvec_edges->at(i).first, pvec_edges->at(i).second);
	//	}
	GVNum = v_num;
	int *estimate_edge_cnt = new int[GVNum];
	memset(estimate_edge_cnt, 0, sizeof(int) * GVNum);
	for (int i = 0; i < (int)pvec_edges->size(); i++) {
		from = (pvec_edges->at(i)).first;
		to = (pvec_edges->at(i)).second;
		estimate_edge_cnt[from]++;
		estimate_edge_cnt[to]++;
	}
	GENum = 0;
	GNumNbrs = new int[GVNum];
	memset(GNumNbrs, 0, sizeof(int) * GVNum);
	GNbrLst = new int*[GVNum];
	for (int i = 0; i < (int)pvec_edges->size(); i++) {
		from = (pvec_edges->at(i)).first;
		to = (pvec_edges->at(i)).second;
		if (from == to) continue; //self-edges are abandoned
		if (GNumNbrs[from] == 0) {
			GNbrLst[from] = new int[estimate_edge_cnt[from]];
		}
		int exist1 = 0;
		for (int j = 0; j < GNumNbrs[from]; j++) {
			if (GNbrLst[from][j] == to) {
				exist1 = 1;
				break;
			}
		}
		if (!exist1) {
			GNbrLst[from][GNumNbrs[from]] = to;
			GNumNbrs[from]++;
			GENum++;
		}

		if (GNumNbrs[to] == 0) {
			GNbrLst[to] = new int[estimate_edge_cnt[to]];
		}
		int exist2 = 0;
		for (int j = 0; j < GNumNbrs[to]; j++) {
			if (GNbrLst[to][j] == from) {
				exist2 = 1;
				break;
			}
		}
		if (!exist2) {
			GNbrLst[to][GNumNbrs[to]] = from;
			GNumNbrs[to]++;
			GENum++;
		}
	}
	assert(GENum % 2 == 0);
	GENum = GENum / 2;
	printf("load SNAP graph %s with vertices %d, edges %d (directed edges %d)\n",
		file_basename(filename), GVNum, GENum, (int)pvec_edges->size());
	//print_org_graph();
	//build v_
	delete[] newid;
	delete[] estimate_edge_cnt;
	delete pvec_edges;
	return 1;
}

/*load instances of metis format from
* instances are download from
* http://www.cc.gatech.edu/dimacs10/downloads.shtml
*/
int load_metis_instance(char* filename) {
	ifstream infile(filename);
	string line;

	if (!infile) {
		fprintf(stderr, "Can not find file %s\n", filename);
		exit(0);
	}
	GFmt = 0;
	//ignore comment
	getline(infile, line);
	while (line.length() == 0 || line[0] == '%')
		getline(infile, line);
	stringstream l1_ss(line);
	l1_ss >> GVNum >> GENum >> GFmt;
	cout << line << endl;
	if (GFmt == 100) {
		cerr << "self-loops and/or multiple edges need to be considered";
		return 0;
	}
	/*allocate graph memory*/
	GNumNbrs = new int[GVNum];
	GNbrLst = new int*[GVNum];

	int v_no = 0;
	int *vlst = new int[GVNum];
	int n_adj = 0;
	//ignore the char
	//read the end of first line
	//infile.getline(line, LINE_LEN);
	while (getline(infile, line) && v_no < GVNum) {
		if (infile.fail()) {
			fprintf(stderr, "Error in read file, vno %d\n", v_no);
			exit(-1);
		}
		stringstream ss(line);
		int ve_adj;

		n_adj = 0;
		while (ss >> ve_adj) {
			vlst[n_adj++] = ve_adj - 1;
		}
		GNumNbrs[v_no] = n_adj;
		if (n_adj == 0)
			GNbrLst[v_no] = NULL;
		else {
			GNbrLst[v_no] = new int[n_adj];
			memcpy(GNbrLst[v_no], vlst, sizeof(int) * n_adj);
		}
		v_no++;
	}
	assert(v_no == GVNum);
	printf("load metis graph %s with vertices %d, edges %d \n",
		file_basename(filename), GVNum, GENum);
	//debug
	delete[] vlst;
	infile.close();
	return 1;
}

double computeDensity(set<int> sol) {
	int nedge = 0;
	for (set<int>::iterator itr = sol.begin();
		itr != sol.end();
		itr++) {
		int v = *itr;
		for (int i = 0; i < GNumNbrs[v]; i++) {
			int vnbr = GNbrLst[v][i];
			set<int>::iterator rt = sol.find(vnbr);
			if (rt != sol.end()) {
				nedge++;
			}
		}

	}
	return (double)nedge / (sol.size()*(sol.size() - 1));
}