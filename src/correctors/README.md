# Error correcting algorithms

## erasures.c

Error correcting algorithm for the binary erasure channel of a classical AES-128 key schedule. Begins by resolving all bits that can be deduced by the initial noisy key and then propagates. If it gets stuck, it begins a recursive exponential ennumeration. 

## alternative_bsc.c

Bruteforces all possible values of the initial row of an alternative key schedule as presented in the following article (see pages 7 and 8)

Leurent, G., & Pernot, C. (2021, June). New representations of the AES key schedule. In Annual International Conference on the Theory and Applications of Cryptographic Techniques (pp. 54-84). Cham: Springer International Publishing. https://eprint.iacr.org/2020/1253.pdf

In particular all possible values of [$s_i$, $s_{i+1}$, $s_{i+2}, $s_{i+3}] for {i \in {0,1,2,3}} are enumerated and each candidate is ranked according to its Z-score

## heuristic_bsc.c 

Ennumerates key schedules using the algorithm presented in the following article using the Z-score as a heuristic of each candidate

Veyrat-Charvillon, N., Gérard, B., Renauld, M., & Standaert, F. X. (2013). An optimal key enumeration algorithm and its application to side-channel attacks. In Selected Areas in Cryptography: 19th International Conference, SAC 2012, Windsor, ON, Canada, August 15-16, 2012, Revised Selected Papers 19 (pp. 390-406). Springer Berlin Heidelberg. https://eprint.iacr.org/2011/610 

