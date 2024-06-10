# AES Key Reconstruction - Alternative Representation

## Generators

keygen.c generates a key schedule as specified in pages 7 and 8 of bibliography. Graphically it corresponds to the following image:

![See page 10 of bibliography](./img/graphic.png?raw=true "One round of the AES-128 key schedule (Leurent & Pernot, 2021)")

## Error correcting algorithms

### bruteforce_bsc.c

Bruteforces all possible values of the initial row of an alternative key schedule.

In particular all possible values of [ $s_i$ , $s_{i+1}$ , $s_{i+2}$ , $s_{i+3}$ ] for { $ i \in {0,1,2,3} $} are enumerated and each candidate is ranked according to its Z-score

### heuristic_bsc.c 

Ennumerates key schedules by least divergence to extracted key schedule relative to the decay probability using the Z-score. Outputs key schedule that has Z-score of less than 3/2 standard deviations

## Bibliography

- Leurent, G., & Pernot, C. (2021, June). New representations of the AES key schedule. In Annual International Conference on the Theory and Applications of Cryptographic Techniques (pp. 54-84). Cham: Springer International Publishing. https://eprint.iacr.org/2020/1253.pdf