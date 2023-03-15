import sqlite3
from matplotlib import pyplot as plt
import numpy as np
import math
import statistics


def getMeanAndStdevOld(path):

    con = sqlite3.connect(path)
    cursor = con.cursor()

    bin_data = cursor.execute("select * from data;").fetchall()

    pos_bins = dict()
    neg_bins = dict()

    ## bin_data has all data for the histogram, need to get mean and standard deviation for each bin
    ## each row of the query is a tuple of 5 elements [histo name, weight id, bin #, positive value, negative value]
    ## sort them into +bin/-bin[name] -> bin # -> [mean, standard deviation]

    for row in bin_data:
        ## if the histo name is not inside the bin dictionaries, create a new dictionary for each of +/- bin dictionary
        ## append values to +/-bin[name][bin#]
       
        if row[0] not in pos_bins or row[0] not in neg_bins:
            pos_bins[row[0]] = dict()
            neg_bins[row[0]] = dict()
            pos_bins[row[0]][row[2]] = [float(row[3])]
            neg_bins[row[0]][row[2]] = [float(row[4])]
     
        else:
            if row[2] in pos_bins[row[0]] or row[2] in neg_bins[row[0]]:
                pos_bins[row[0]][row[2]].append(float(row[3]))
                neg_bins[row[0]][row[2]].append(float(row[4]))
            else :
                pos_bins[row[0]][row[2]] = [float(row[3])]
                neg_bins[row[0]][row[2]] = [float(row[4])]

    output = dict()

    for histo_name in pos_bins: 
        output[histo_name] = dict()
        for bin_i in pos_bins[histo_name]: 
            output[histo_name][bin_i] = [statistics.mean(pos_bins[histo_name][bin_i]), statistics.stdev(pos_bins[histo_name][bin_i])]

    for histo_name in neg_bins:
        for bin_i in neg_bins[histo_name]: 
            output[histo_name][bin_i].extend([statistics.mean(neg_bins[histo_name][bin_i]), statistics.stdev(neg_bins[histo_name][bin_i])])

    return output



def getMeanAndStdev(path):

    con = sqlite3.connect(path)
    cursor = con.cursor()
    bin_data = cursor.execute("select * from data;").fetchall()

    ## parse data in the form of parsed_data[histo_name][bin #][{positive value, negative value}]
    parsed_data = dict()
    for row in bin_data:

        histo_name = row[0]
        weight_id = row[1]
        bin_number = row[2]
        value = float(row[3]) - abs(float(row[4]))
      
        if histo_name not in parsed_data:
            ## if histo name is not in the parsed_data dictionary, then create a new bin dictionary for that histo, then for the bin, create a weigh id dictionary
            parsed_data[histo_name] = dict()
            parsed_data[histo_name][bin_number] = []
            
        else:
            ## since histo name is in the parsed_data dictionary, we need to check if the bin in the dictioary, if not then create a weight id dictionary for that bin
            if bin_number not in parsed_data[histo_name]:
                parsed_data[histo_name][bin_number] = []

        parsed_data[histo_name][bin_number].append(value)
   
    output = dict()
    for histo_name in parsed_data:
        output[histo_name] = dict()
        for bin_number in parsed_data[histo_name]:
            output[histo_name][bin_number] = [statistics.mean(parsed_data[histo_name][bin_number]), statistics.stdev(parsed_data[histo_name][bin_number])]

    return output

def getHistoStatisticsAvg(path):

    con = sqlite3.connect(path)
    cursor = con.cursor()
   
    
    statistics = cursor.execute("select name, avg(pos_num_events), avg(neg_num_events), avg(pos_sum_event_weights_over_events), avg(neg_sum_event_weights_over_events), avg(pos_entries), avg(neg_entries), avg(pos_sum_event_weights_over_entries), avg(neg_sum_event_weights_over_entries), avg(pos_sum_squared_weights), avg(neg_sum_squared_weights), avg(pos_value_times_weight), avg(neg_value_times_weight), avg(pos_value_squared_times_weight), avg(neg_value_squared_times_weight) from Statistics group by name;").fetchall()
       
    statdict = dict()
    for i in range(len(statistics)):
        statdict[statistics[i][0]] = statistics[i][1:]
        
    return statdict;






## debug for printing out output dictionary
## structure is as follows:
## output[histogram_name][bin #] = [positive mean, positive stdev, negative mean, negative stddev]


def DBreader_debug(output):

    for name in output:
        print(name)
        for eachbin in output[name]:
            print(eachbin)
            for val in output[name][eachbin]:
                print(val)


    for histo in output:
        num_of_keys = len(output[histo].keys())
        labels = [None] * num_of_keys
        for i in range(1,num_of_keys):
            labels[i] = i
        labels[0] = 'underflow'
        labels[num_of_keys-1] = 'overflow'
        positives = [None] * num_of_keys
        negatives = [None] * num_of_keys
        for row in output[histo]:
            if(row == 'underflow'):
                positives[0] = output[histo][row][0]
                negatives[0] = output[histo][row][2]
            elif(row == 'overflow'):
                positives[num_of_keys-1] = output[histo][row][0]
                negatives[num_of_keys-1] = output[histo][row][2]
            else: 
                positives[int(row)] = output[histo][row][0]
                negatives[int(row)] = output[histo][row][2]
        #for lable in lables:
         #   print(lable)
        #for val in positives:
         #   print(val)
        #for val in negatives:
         #   print(val)
        x = np.arange(num_of_keys)
        width = 0.5
        fig, ax = plt.subplots()
        rects1 = ax.bar(x - width/3, positives, width, label="positives avg")
        rects2 = ax.bar(x + width/3, negatives, width, label="negatives avg")

        ax.set_ylabel('Events Luminosity = ')
        ax.set_title(histo)
        ax.set_xticks(x, labels, rotation = 65)
        ax.legend() 
        
        #ax.bar_label(rects1, padding=3)
        #ax.bar_label(rects2, padding=3)

        fig.tight_layout()
        plt.show()










    


