#include <bitset>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <vector>
#include "args.hxx"

#define BITSIZE 4010

struct Parameters {
    double gamma;
    int seed;
    int timeLim;
    double alpha;
};

int n, m;

Parameters Param;

std::bitset<BITSIZE> *Nei;

class Set {
   public:
    std::bitset<BITSIZE> B;
    std::vector<int> V;

    void clear() {
        for (auto v : V) B.reset(v);
        V.clear();
    }

    int deg(int &v) {
        static std::bitset<BITSIZE> tmp;
        tmp = B & Nei[v];

        return tmp.count();
    }

    void insert(int &v) {
        V.push_back(v);
        B.set(v);
    }
};

class SetE : public Set {
   public:
    int E;

    void clear() {
        for (auto v : V) B.reset(v);
        V.clear();
        E = 0;
    }

    void insert(int &v) {
        E += deg(v);
        V.push_back(v);
        B.set(v);
    }
};

Set V, CL;
SetE S, Stemp;

std::vector<std::pair<int, int> > RCL;

double ktemp;

bool input(int argc, char *argv[]) {
    args::ArgumentParser parser("HC3: An implementation of HC3 algorithm\n",
                                "[Oliveira et al., 2013] Oliveira, A. B. , "
                                "Plastino, A. , & Ribeiro, C. C.\n"
                                "This implementation is made by Zhenyu Guo.\n");

    args::HelpFlag help(parser, "help", "Display this help menu",
                        {'h', "help"});
    args::Group required(parser, "", args::Group::Validators::All);

    args::ValueFlag<std::string> benchmark_file(
        parser, "benchmark",
        "Path to benchmark, DIMACS and SNAP"
        "formats are supported.",
        {'f', "file"}, "benchmark/Even_3000.clq");

    args::ValueFlag<double> gamma(
        parser, "gamma", "The parameter gamma, which should be in (0, 1)",
        {'k', "gamma"}, 0.95);

    // args::ValueFlag<int> best(parser, "best", "The best objective value as
    // you known, which is used to accelerate the algorithm.", {'o', "best"},
    // 99999);

    args::ValueFlag<int> seed(parser, "seed", "Seed due to the randomness",
                              {'c', "seed"}, 12345);

    args::ValueFlag<int> timeLim(parser, "time", "The cutdown time in second.",
                                 {'t', "time"}, 30);

    args::ValueFlag<double> alpha(parser, "alpha", "The parameter alpha",
                                  {'a', "alpha"}, 0.1);

    // args::ValueFlag<double> beta(parser, "beta", "The parameter beta", {'b',
    // "beta"}, 0.25);

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << parser;
        return 0;
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 0;
    } catch (args::ValidationError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 0;
    }

    std::string filePath = args::get(benchmark_file);

    Param.gamma = args::get(gamma);
    Param.alpha = args::get(alpha);
    Param.seed = args::get(seed);
    Param.timeLim = args::get(timeLim);

    std::ifstream ifs(filePath.c_str());
    std::stringstream ss;
    std::string buff;

    while (std::getline(ifs, buff)) {
        std::string tmp;
        ss.clear();
        ss << buff;
        ss >> tmp;
        if (tmp == "c") continue;
        if (tmp == "p") {
            ss >> tmp >> n >> m;
            Nei = new std::bitset<BITSIZE>[n + 10];
        } else {
            int u, v;
            ss >> u >> v;
            Nei[u].set(v);
            Nei[v].set(u);
        }
    }
    return 1;
}

int pickFromRCL() {
    int p = std::max(1, (int)(Param.alpha * RCL.size()));
    return RCL[rand() % p].second;
}

void init() {
    RCL.clear();
    for (int v = 1; v <= n; ++v) RCL.push_back({V.deg(v), v});
    std::sort(RCL.begin(), RCL.end(), std::greater<std::pair<int, int> >());
    int x = pickFromRCL();

    Stemp.insert(x);

    ktemp = 1;

    for (int v = 1; v <= n; ++v) V.insert(v);
}

int x2(int x) { return x * (x - 1) / 2; }

void work() {
    S.clear();
    for (auto v : Stemp.V) S.insert(v);
    CL.clear();

    static std::bitset<BITSIZE> VmSB;
    VmSB = V.B ^ S.B;

    for (int v = VmSB._Find_first(); v < VmSB.size(); v = VmSB._Find_next(v)) {
        if (((double)S.E + S.deg(v)) / x2(S.V.size() + 1) >= ktemp)
            CL.insert(v);
    }

    RCL.clear();

    if ((int)CL.V.size()) {
        for (auto v : CL.V) {
            int difv =
                CL.deg(v) + CL.V.size() * (S.deg(v) - ktemp * (S.V.size() + 1));
            RCL.push_back({difv, v});
        }
    } else {
        for (int v = VmSB._Find_first(); v < VmSB.size();
             v = VmSB._Find_next(v)) {
            if (S.deg(v) > 0) CL.insert(v);
        }
        if (CL.V.size() == 0)
            return;
        else {
            for (auto v : CL.V) RCL.push_back({S.deg(v), v});
        }
    }

    std::sort(RCL.begin(), RCL.end(), std::greater<std::pair<int, int> >());
    int x = pickFromRCL();

    Stemp.insert(x);
    ktemp = (double)Stemp.E / x2(Stemp.V.size());
}

int main(int argc, char *argv[]) {
    input(argc, argv);
	srand(Param.seed);
    init();

    while (ktemp >= Param.gamma) {
        work();
    }
    return 0;
}