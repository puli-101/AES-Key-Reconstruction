import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import sys

def get_len(OGdf, df, divisions, value):
    min_ = df['decay_OG'].min()
    max_ = df['decay_OG'].max()
    div_len = (max_ - min_)/divisions
    curr_division = int((value - min_) / div_len)
    if (value == max_):
        curr_division -= 1
    low_bound = min_ + curr_division * div_len
    up_bound = min_ + (curr_division+1) * div_len
    assert(curr_division < divisions)
    tests_in_division = len(OGdf[(OGdf['decay_OG'] > low_bound) & (OGdf['decay_OG'] < up_bound)])
    failed_tests =  len(df[(df['decay_OG'] > low_bound) & (df['decay_OG'] < up_bound)])
    assert(failed_tests/tests_in_division < 1)
    return 100/tests_in_division


def graph(path, options, divisions):
    if not(True in options.values()):
        print("Nothing to be graphed")
        return 

    df = pd.read_csv(path, header=0, sep=";", engine="pyarrow")

    if options["failures"]:
        rslt_df = df[df['match'] == False]
        rslt_df['decay_OG'].hist(bins=divisions)
        plt.suptitle('Number of failures as a function of decay rate')
        plt.xlabel('Decay rate (errors/key size)')
        plt.ylabel('Number of failures (#)')
    if options["relative"]:
        rslt_df = df[df['match'] == False]

        cweights = np.ones_like(rslt_df["decay_OG"])
        for i in range(len(cweights)):
            cweights[i] = get_len(df, rslt_df, divisions, rslt_df["decay_OG"].to_numpy()[i])
        plt.hist(rslt_df["decay_OG"], bins=divisions, weights=cweights)
        plt.suptitle('Percentage of failures per decay rate')
        plt.xlabel('Original decay rate (errors/key size)')
        plt.ylabel('% of failures')
    if options["decay"]:
        min_ = df['decay_OG'].min()
        max_ = df['decay_OG'].max()
        t = [(min_ + (max_ - min_)/100 * t) for t in range(101)]
        df.plot(kind="scatter", x="decay_OG", y="decay_ALT")
        plt.plot(t, [2 * y for y in t], color="red")
        plt.suptitle("Decay rate of key schedule after translation")
        plt.xlabel("Decay rate of key in RAM (errors/key size)")
        plt.ylabel("Decay rate after translation (errors/key size)")
    if options["time"]:
        df.plot(kind="scatter", x="decay_OG", y="time")
        plt.suptitle("Execution time of four blocks as a function of decay rate")
        plt.xlabel("Original decay rate (errors/key size)")
        plt.ylabel("Execution time (s)")
    if options["iterations"]:
        for i in range(4):
            df.plot(kind="scatter",x="decay_OG",y="iter_block_"+str(i+1))
            plt.suptitle("Iterations of block number "+str(i+1)+" as a function of decay rate")
            plt.xlabel("Original decay rate (errors/key size)")
            plt.ylabel("Iterations of first block (#)")

    plt.show()


if __name__ == "__main__":
    argc = len(sys.argv)
    paths = ["misc/statistics.csv", "misc/statistics_0625_raw.csv"] #potentiall we can add more path options
    path = paths[0]
    divisions = 10
    options = {'decay': False, 'time': False, 'iterations':False, 'failures':False, 'relative':False}

    if argc > 1:
        for i in range(1,argc):
            option = sys.argv[i].lower()
            if option == "control":
                path = paths[1]
            elif option == "help":
                print("The execution format is as follows\npython3 misc/grapher",end="")
                print(" [all | decay | time | iterations | failures | relative | control | path=PATH | divisions=<n> | help]\n")
                print("Warning, this sourcefile has to be executed from the folder alternative_representations")
                sys.exit()
            elif option.startswith("path="): #custom path
                path = option[5:]
            elif option in options.keys():
                options[option] = True
            elif option == "all":
                for key in options.keys():
                    options[key] = True
            elif option.startswith("divisions="):
                divisions = int(option[10:])
            else:
                print("Unknown option: "+option)
                sys.exit()
    
    graph(path, options, divisions)