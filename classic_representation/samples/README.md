# Samples

The following files contain sample outputs of the main sourcefiles, which also serve as inputs to each other. For instance 'sched1' which is the output of 'keygen' can be utilized as an input for 'keymod', and 'sched1_bec' which is the output of 'keymod' can be used as input for 'correct_bec'. In detail: 

- sched1.txt : contains an AES-128 key schedule with master key '64cff2be 1e72ced5 4423bf89 0d1d88c3'
- sched1_bec.txt : contains an AES-128 key schedule that went through the binary erasure channel. The schedule is represented as a set binary strings with the missing bits marked as 'X'
- sched1_bsc.txt : contains a key schedule that went through the binary symmetric channel
- sched1_z.txt : exactly the same for the Z-channel 

To properly run the binaries one must follow exactly the formats of these sample files