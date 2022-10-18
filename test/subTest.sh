#!/bin/bash
# specify number of HBs
export NO_OF_HBEATS=1

#export LDMS_SAMPLING_INTERVAL=1 # recommended 1 sec
export LDMS_SAMPLING_INTERVAL=0.1


#Run the program
./testHB

