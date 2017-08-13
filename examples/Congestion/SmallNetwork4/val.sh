#!/bin/bash

valgrind --leak-check=full opp_run -r 0 -n ../../.. -l ../../../policies/rinasim -c _19-Cubic-1Gbps-10ms-536B --debug-on-errors=false -u Cmdenv omnetpp.ini
