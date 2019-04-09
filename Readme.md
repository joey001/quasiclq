# Quasiclq
Quasiclq is an implement of a quasi-clique problem algorithm.

## Running Command
```
./bin/quasiclq {OPTIONS}
```

    FLSQ: A Fast Local Search Algorithm for finding maximum gamma-quasi-cliques

  OPTIONS:

      -h, --help                        Display this help menu
      -f[benchmark], --file=[benchmark] Path to benchmark, DIMACS and
                                        SNAPformats are supported.
      -k[gamma], --gamma=[gamma]        The parameter gamma, which should be in
                                        (0, 1)
      -o[best], --best=[best]           The best objective value as you known,
                                        which is used to accelerate the
                                        algorithm.
      -c[seed], --seed=[seed]           Seed due to the randomness
      -t[time], --time=[time]           The cutdown time in second.
      -a[alpha], --alpha=[alpha]        The parameter alpha
      -b[beta], --beta=[beta]           The parameter beta

    [Zhou et al., 2019] Yi Zhou, Zhenyu Guo, Jin-Kao Hao

## Scripts
python3 must be supported
```
python Scripts/cmdgen.py [benchmark director path] [command list text path]
python Scripts/zipOutput.py
```