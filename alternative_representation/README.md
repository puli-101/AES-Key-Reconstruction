# AES Key Reconstruction - Alternative Representation

## Functionalities

### 1. Generators

keygen.c generates a key schedule as specified in pages 7 and 8 of bibliography. Graphically it corresponds to the following image:

<p align="center">
  <img alt="See page 10 of bibliography" src="./img/graphic.png?raw=true" />
  <br>
  One round of the AES-128 key schedule (Leurent & Pernot, 2021)
</p>

$k_i$ corresponds to the $i$-th byte of the orignial key. On the other hand, $k'_i$ corresponds to the $i$-th byte after the transformations shown in page 7 of the bibliography.

### 2. Error correcting algorithms

#### 2.1 bruteforce_bsc.c

Bruteforces all possible values of the initial row of an alternative key schedule.

In particular all possible values of [ $s_i$ , $s_{i+1}$ , $s_{i+2}$ , $s_{i+3}$ ] for { $ i \in {0,1,2,3} $} are enumerated and each candidate is ranked according to its Z-score

#### 2.2 heuristic_bsc.c 

Ennumerates key schedules by least divergence to extracted key schedule relative to the decay probability using the Z-score. Outputs key schedule that has Z-score of less than 3/2 standard deviations

## Compilation

It suffices to execute 'make'. The binary files will be located at ./bin

## Execution

### 1. Key Schedule Generator
To generate an alternative AES key schedule, execute 

    ./bin/keygen [KEY] [OPTIONS]

Where KEY represents an AES-128 key in hexadecimal and OPTIONS include (for now) -v=false to disable verbose

If no input is given, then a random AES-128 key schedule is generated.

### 2. Noisy Channels
To modify a schedule, execute

    ./bin/noise <file> <probability> <channel_t> [options]

Where 

- 'file' contains an AES key schedule as formatted as ./bin/keygen 's output (see './samples/aes-128-schedule.txt' for an example)
- 'probability' indicates how likely each bit is to flip/be erased
- 'channel_t' indicates the type of noisy channel the key schedule goes through. Accepted types include 'bin-sym' for binary symmetric, 'bin-erasure' for binary erasure, and 'z-channel'
- options include -v=false to disable verbose

### 3. Error Correction

#### 3.1 Bruteforce Approach

To correct a key schedule that lost bits through the binary symmetric channel, execute

    ./bin/bruteforce <file> <probability> [options]

You can expect execution times of up to 8 hours.

#### 3.2 Heuristic Solution

To correct heuristically a key schedule that lost bits through the binary symmetric channel, execute

    ./bin/heuristic <file> <probability> [options]

You can expect execution times of up to 30 minutes.

## Bibliography

- Leurent, G., & Pernot, C. (2021, June). New representations of the AES key schedule. In Annual International Conference on the Theory and Applications of Cryptographic Techniques (pp. 54-84). Cham: Springer International Publishing. https://eprint.iacr.org/2020/1253.pdf