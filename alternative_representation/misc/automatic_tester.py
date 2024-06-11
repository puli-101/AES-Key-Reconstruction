import sys
import os
import random

if __name__ == "__main__":
        argc = len(sys.argv)
        nb_tests = 100
        offset = 2
        interval = [0.0625, 0.125]

        if argc > 1:
            for i in range(1, argc):
                option = sys.argv[i]
                if (option == "-h" or option == "-help" or option == "--help"):
                    print("Usage : "+sys.argv[0]+" [-n=<integer> | -o=<integer> | -p=<float>]")
                    print("Where\t-n indicates the maximum number of tests (100 by default)")
                    print("\t-o indicates the offset of the tests (e.g. if -o=2 then we start counting from 2 which is the default value)")
                    print("\t-p indicates the maximum error probability considered while generating the tests\n")
                    print("Warning : this code has to be run from the folder alternative_representation")
                    sys.exit()
                elif option.startswith("-n"):
                    nb_tests = int(option[3:])
                elif option.startswith("-o"):
                    offset = int(option[3:])
                elif option.startswith("-p"):
                    interval[1] = float(option[3:])
                
        print(nb_tests)
        print(offset)
        print(interval[1])
        #for i in range(nb_tests):
        #    probability = random.uniform(interval[0], interval[1])
        #    os.system("../classic_representation/bin/keygen -v=false > samples/sched"+str(i+offset)+".txt")
        #    os.system("../classic_representation/bin/noise samples/sched"+str(i+offset)+".txt -v=false > samples/sched"+str(i+2)+".txt")

