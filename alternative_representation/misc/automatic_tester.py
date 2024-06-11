import sys
import os
import random
import matplotlib.pyplot as plt
import time

verbose = True

## see TODO

def filter(filename):
    with open(filename, 'r') as content_file:
        content = content_file.read()
    return "".join(content.split())

def calcHamming(file1, file2):
    txt1 = filter(file1)
    txt2 = filter(file2)
    assert(len(txt1) == len(txt2))
    
    m1 = int(txt1,16)
    m2 = int(txt2,16)
    m = m1 ^ m2

    return m.bit_count()
    
def execute_cmd(cmd):
    if verbose:
        print(cmd)
    os.system(cmd)

if __name__ == "__main__":
        argc = len(sys.argv)
        nb_tests = 10
        offset = 3
        interval = [0.0625, 0.125]
        autoremove = True
        sched_size = 128 * 11
        fixed_probability = 0.0625

        if argc > 1:
            for i in range(1, argc):
                option = sys.argv[i]
                if (option == "-h" or option == "-help" or option == "--help"):
                    print("Usage : "+sys.argv[0]+" [-n=<integer> | -o=<integer> | -p=<float> | -v=<bool> | -r=<bool>]")
                    print("Where\t-n indicates the maximum number of tests ("+str(nb_tests)+" by default)")
                    print("\t-o indicates the offset of the tests (e.g. if -o=3 then we start counting from 3 which is the default value)")
                    print("\t-p indicates the maximum error probability considered while generating the tests (default 0.125)")
                    print("\t-v indicates if the partial results/statistics are to be shown (default is "+str(verbose)+")")
                    print("\t-r indicates if test files are to be autoremoved (default is true) \n")
                    print("Warning : this code has to be run from the folder alternative_representation")
                    sys.exit()
                elif option.startswith("-n"):
                    nb_tests = int(option[3:])
                elif option.startswith("-o"):
                    offset = int(option[3:])
                elif option.startswith("-p"):
                    interval[1] = float(option[3:])
                elif option.lower() == "-v=false":
                    verbose = False
                elif option.lower() == "-r=false":
                    autoremove = False
        
        f = open("misc/statistics.csv", "a+")
        if os.stat("misc/statistics.csv").st_size == 0:
            f.write("test_number;hamming_dist_OG;hamming_dist_ALT;decay_OG;decay_ALT;growth;time\n")

        decay_OG_lst = []
        decay_ALT_lst = []
        for i in range(nb_tests):
            probability = fixed_probability #random.uniform(interval[0], interval[1])

            sched = "samples/sched"+str(i+offset)+".txt"
            modif = "samples/sched"+str(i+offset)+"_"+(str(probability)[2:6])+".txt"
            trans = "samples/translation"+str(i+offset)+".txt"
            trans_modif = "samples/translation"+str(i+offset)+"_"+(str(probability)[2:6])+".txt"
            files = sched+" "+modif+" "+trans+" "+trans_modif

            #generates schedule
            execute_cmd("../classic_representation/bin/keygen -v=false > "+sched)
            #adds noise
            execute_cmd("../classic_representation/bin/noise " + sched \
                       +" "+str(probability)+" bin-symm -v=false > "+modif)
            #determines hamming distance of the noisy schedule and the original
            dstOG = calcHamming(sched, modif)
            #translates original to alternative
            execute_cmd("./bin/classic_to_alt "+sched+" -v=false > "+trans)
            #translates original noisy to alternative noise
            execute_cmd("./bin/classic_to_alt "+modif+" -v=false > "+trans_modif)
            #determines hamming distance of noisy alternative and original alternative
            dstALT = calcHamming(trans, trans_modif)
            #calcs execution time
            exec_time = time.time()
            execute_cmd("./bin/heuristic "+trans_modif+" "+str(dstALT/sched_size)+" -v=false >> misc/metadata.txt")
            exec_time = time.time() - exec_time
            #extract metadata
            # TODO (i.e. extract number of iterations per key)
            #check if wrong reconstruction
            # TODO (suffices to execute one of the translation binaries)

            f.write(str(i+1)+";")
            f.write(str(dstOG)+";")
            f.write(str(dstALT)+";")
            f.write(str(dstOG/sched_size)+";")
            f.write(str(dstALT/sched_size)+";")
            f.write(str(dstALT/dstOG)+";")
            f.write(str(exec_time))

            decay_OG_lst.append(dstOG/sched_size * 100)
            decay_ALT_lst.append(dstALT/sched_size * 100)

            if autoremove:
                execute_cmd("rm "+files)
            f.write("\n")
        f.close()

        print(decay_OG_lst)
        print(decay_ALT_lst)
        print(min(decay_ALT_lst),max(decay_ALT_lst))
        print(min(decay_OG_lst),max(decay_OG_lst))
        plt.scatter(decay_OG_lst, decay_ALT_lst)
        plt.title("Degradation des key schedules")
        plt.xlabel("%% degradation originel")
        plt.ylabel("%% degradation alternatif")
        plt.show()
