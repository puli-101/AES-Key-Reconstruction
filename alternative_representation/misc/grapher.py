import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import sys

def graph(path, options):
    if not(True in options.values()):
        print("Nothing to be graphed")
        return 

    df = pd.read_csv(path, header=0, sep=";", engine="pyarrow")

    if options["decay"]:
        min_ = df['decay_OG'].min
        max_ = df['decay_OG'].max
        t = [(min_ + (max_ - min_)/100 * t) for t in range(101)]

        df.plot(kind="scatter", x="decay_OG", y="decay_ALT")
        plt.plot(t, [2 * y for y in t], color="red")
        plt.suptitle("Decay rate of key schedule after translation")
        plt.xlabel("Decay rate of key in RAM (errors/key size)")
        plt.ylabel("Decay rate after translation (errors/key size)")
    if options["time"]:
        df.plot(kind="scatter", x="decay_ALT", y="time")
        plt.suptitle("Execution time of four blocks as a function of decay rate")
        plt.xlabel("Decay rate (errors/key size)")
        plt.ylabel("Execution time (s)")
    if options["iterations"]:
        for i in range(4):
            df.plot(kind="scatter",x="decay_OG",y="iter_block_"+str(i+1))
            plt.suptitle("Iterations of block number",i+1,"as a function of decay rate")
            plt.xlabel("Decay rate (errors/key size)")
            plt.ylabel("Iterations of first block (#)")
    if options["failures"]:
        rslt_df = df[df['match'] == False]
        rslt_df['decay_OG'].hist()
        plt.title('Number of failures as a function of decay rate') 
        plt.xlabel('Decay rate (errors/key size)') 
        plt.ylabel('Number of failures (#)') 
        plt.show() 

    plt.show()


if __name__ == "__main__":
    argc = len(sys.argv)
    paths = ["misc/statistics.csv", "misc/statistics_0625_raw.csv"] #potentiall we can add more path options
    path = paths[0]
    options = {'decay': False, 'time': False, 'iterations':False, 'failures':False, 'divisions': 20}

    if argc > 1:
        for i in range(1,argc):
            option = sys.argv[i].lower()
            if option == "control":
                path = paths[1]
            elif option == "help":
                print(sys.argv[0]+" [decay | time | iterations | failures | control | path=PATH | help]")
                sys.exit()
            elif option.startswith("path="): #custom path
                path = option[5:]
            elif option == "decay" or option == "time" or option == "iterations" or option == "failures":
                options[option] = True
            else:
                print("Unknown option: "+option)
                sys.exit()
    
    graph(path, options)