# AES Key Reconstruction

## Overview

This repository contains generators, modifiers, and reconstructor for two different representations of an AES key schedule. 

- The first folder named 'classical_representation' contains the above mention functionalities for a standard AES key schedule. In particular, the possibility to expand a 128-bit key or generate a random AES-128 key schedule, simulate the passing of a key schedule through a binary channel, and correct a schedule that went through one of these channels. For further details see 'classic_representation/README.md'

- The second folder named 'alternative_representation' contains the same functionalities presented but for an alternative AES schedule. This new schedule is comprised of four linearly independent blocks of four bytes per round. For further details see 'alternative_representation/README.md' or the article below. 

## Credits

The alternative key schedule was inspired by the following article:

- Veyrat-Charvillon, N., Gérard, B., Renauld, M., & Standaert, F. X. (2013). An optimal key enumeration algorithm and its application to side-channel attacks. In Selected Areas in Cryptography: 19th International Conference, SAC 2012, Windsor, ON, Canada, August 15-16, 2012, Revised Selected Papers 19 (pp. 390-406). Springer Berlin Heidelberg. https://eprint.iacr.org/2011/610 

##

This repository was developed during an internship at the ALMASTY team at Sorbonne Université - LIP6 (Computer Science Laboratory of Paris 6)