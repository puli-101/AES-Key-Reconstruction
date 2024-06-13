# AES Key Reconstruction

## Overview

This repository contains generators, modifiers, and reconstructor for two different representations of AES key schedules. 

- The first folder named 'classical_representation' contains the above mention functionalities for standard AES key schedules. In particular, the possibility to expand a 128-bit key or generate a random AES-128 key schedule, simulate the passing of a key schedule through a binary channel, and correct a schedule that went through one of these channels. For further details see 'classic_representation/README.md'

- The second folder named 'alternative_representation' contains the same functionalities presented but for an alternative AES schedule. This new schedule is comprised of four linearly independent blocks of four bytes per round. For further details see 'alternative_representation/README.md' or the article below. The functionalities included in this folder also permit the translation of a standard key schedule to an alternative in order to correct it. 

## Credits

The alternative key schedule was inspired by the following article:

- Leurent, G., & Pernot, C. (2021, June). New representations of the AES key schedule. In Annual International Conference on the Theory and Applications of Cryptographic Techniques (pp. 54-84). Cham: Springer International Publishing. https://eprint.iacr.org/2020/1253.pdf

The original subject of this project was inspired by the key fixing algorithms on cold boot attacks. See:

Halderman, J. A., Schoen, S. D., Heninger, N., Clarkson, W., Paul, W., Calandrino, J. A., ... & Felten, E. W. (2009). Lest we remember: cold-boot attacks on encryption keys. Communications of the ACM, 52(5), 91-98. https://dl.acm.org/doi/pdf/10.1145/1506409.1506429

##

This repository was developed during an internship at the ALMASTY team at Sorbonne Université - LIP6 (Computer Science Laboratory of Paris 6)