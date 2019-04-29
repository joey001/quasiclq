# Quasiclq

Quasiclq is an implement of a quasi-clique problem algorithm.

## Running Command

### FLSQ

```bash
./quasiclq/bin/quasiclq {OPTIONS}
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

### HC3

```bash
./hc3/bin/hc3 {OPTIONS}
```

    HC3: An implementation of HC3 algorithm

  OPTIONS:

      -h, --help                        Display this help menu
      -f[benchmark], --file=[benchmark] Path to benchmark, DIMACS and
                                        SNAPformats are supported.
      -k[gamma], --gamma=[gamma]        The parameter gamma, which should be in
                                        (0, 1)
      -c[seed], --seed=[seed]           Seed due to the randomness
      -t[time], --time=[time]           The cutdown time in second.
      -a[alpha], --alpha=[alpha]        The parameter alpha

    [Oliveira et al., 2013] Oliveira, A. B. , Plastino, A. , & Ribeiro, C. C.
    This implementation is made by Zhenyu Guo.

## Scripts

python3 must be supported

```bash
python script/cmdgen.py [benchmark director path] [command list text path]
python script/zipOutput.py
```