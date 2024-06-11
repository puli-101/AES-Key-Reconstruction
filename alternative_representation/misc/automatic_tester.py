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

# Extracts information relative to the heuristic correction
# of a key schedule
def extract_metadata():
    keys = []
    nb_iterations = []
    scores = []

    with open("misc/metadata.tmp", 'r') as content_file:
        lines = content_file.readlines()
        for line in lines:
            keys.append(line.split()[0])
            nb_iterations.append(line.split()[1])
            scores.append(line.split()[2])

    return keys, nb_iterations, scores

# Compares if a corrected key corresponds to
# the original key
def check(key,sched):
    if ("NOTFOUND" in key):
        return False
    
    execute_cmd("./bin/alt_to_classic "+("".join(key))+" -v=false > misc/metadata.tmp")
    transKey = filter("misc/metadata.tmp")

    with open(sched,"r") as f:
        OGkey = f.readline()
    
    OGkey = "".join(OGkey.split())
    return OGkey == transKey
        

if __name__ == "__main__":
        argc = len(sys.argv)
        nb_tests = 10
        offset = 3
        interval = [0.0625, 0.125]
        autoremove = True
        sched_size = 128 * 11
        fixed_probability = 0.0625
        graph = False

        if argc > 1:
            for i in range(1, argc):
                option = sys.argv[i]
                if (option == "-h" or option == "-help" or option == "--help"):
                    print("Usage : "+sys.argv[0]+" [-n=<integer> | -o=<integer> | -p=<float> | -v=<bool> | -r=<bool> | -g=true]")
                    print("Where\t-n indicates the maximum number of tests ("+str(nb_tests)+" by default)")
                    print("\t-o indicates the offset of the tests (e.g. if -o=3 then we start counting from 3 which is the default value)")
                    print("\t-p indicates the maximum error probability considered while generating the tests (default 0.125)")
                    print("\t-v indicates if the partial results/statistics are to be shown (default is "+str(verbose)+")")
                    print("\t-r indicates if test files are to be autoremoved (default is true)")
                    print("\t-g indicates whether a graph will be shown at the end (default : "+str(graph).lower()+")")
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
                elif option.lower() == "-g=true":
                    graph = True
        
        f = open("misc/statistics.csv", "a+")
        if os.stat("misc/statistics.csv").st_size == 0:
            f.write("test_number;hamming_dist_OG;hamming_dist_ALT;decay_OG;decay_ALT;growth;time;iter_block_1;score_1"+
                    ";iter_block_2;score_2;iter_block_3;score_3;iter_block_4;score_4;match\n")
        f.close()

        decay_OG_lst = []
        decay_ALT_lst = []
        iterations_lst = []

        for i in range(nb_tests):
            f = open("misc/statistics.csv", "a+")
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
            execute_cmd("nice -n -20 ./bin/heuristic "+trans_modif+" "+str(dstALT/sched_size)+" -v=false > misc/metadata.tmp")
            exec_time = time.time() - exec_time
            #extract metadata
            keys, iterations, scores = extract_metadata() #read 4 lines of 3 words chacune and make it into a list
            #check if wrong reconstruction
            isValid = check(keys, sched) 

            #writes into csv file
            f.write(str(i+1)+";")
            f.write(str(dstOG)+";")
            f.write(str(dstALT)+";")
            f.write(str(dstOG/sched_size)+";")
            f.write(str(dstALT/sched_size)+";")
            f.write(str(dstALT/dstOG)+";")
            f.write(str(exec_time)+";")
            for j in range(4):
                f.write(iterations[j]+";")
                f.write(scores[j]+";")
            f.write(str(isValid).lower())

            decay_OG_lst.append(dstOG/sched_size * 100)
            decay_ALT_lst.append(dstALT/sched_size * 100)
            iterations_lst.append(sum([int(x) for x in iterations])/4)
            #removes files used
            if autoremove:
                execute_cmd("rm "+files)
            f.write("\n")
            f.close()

        print("Quick rundown")
        print("\tmin\tmax")
        print("Original decay",min(decay_ALT_lst),max(decay_ALT_lst))
        print("Alt decay",min(decay_OG_lst),max(decay_OG_lst))
        print("Avg number of iterations",min(iterations_lst),max(iterations_lst))

        if (graph):
            plt.scatter(decay_OG_lst, decay_ALT_lst)
            plt.title("Propagation de la dégradation des key schedules")
            plt.xlabel("%% dégradation originale")
            plt.ylabel("%% dégradation du kschedule alternatif correspondant")
            plt.show()

            plt.scatter(decay_OG_lst, iterations_lst)
            plt.title("Nombre d'itérations à partir du taux de dégradation")
            plt.xlabel("%% dégradation originale")
            plt.ylabel("# itérations")
            plt.show()

        execute_cmd("rm misc/metadata.tmp")