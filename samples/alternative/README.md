# Alternative Key Schedule Representations

This folder contains alternative representations of AES-128 key schedules as stated by Leurent and Pernot. These key schedules untangle the dependencies of each bit into 4 independent blocks of 4-byte words which facilitates the error correction. For further information see 

Leurent, G., & Pernot, C. (2021, June). New representations of the AES key schedule. In Annual International Conference on the Theory and Applications of Cryptographic Techniques (pp. 54-84). Cham: Springer International Publishing. https://eprint.iacr.org/2020/1253.pdf

## Files

- sched1.txt contains a key schedule corresponding to the master key 'abcdef0123456789abcdef0123456789'
- sched1_bsc_0625.txt contains the corresponding key schedule that went through the binary symmetric channel with p = 0.0625