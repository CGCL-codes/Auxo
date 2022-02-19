# Auxo: A Time and Memory Efficient Structure for Scalable Graph Stream Summarization
## Introduction

A graph stream refers to an unbounded, time involving edge stream, in which, each weighted edge has two endpoints and a timestamp, forming a dynamic graph changing rapidly over time. Graph streams have be widely generated in society network, cyber security, and data center. Due to its huge volume and high update speed, recent research mainly focus on graph stream summarization to achieve linear memory usage and constant time cost per edge insertion with slight accuracy sacrifice. However, when faced with an unbounded graph stream, all the existing work either fail to support a scalable graph stream summarizaiton or are not time and memory efficiently. To address this issue, we proposed Auxo, a time and memory efficient structure for scalable graph stream summarization. Auxo reduces the time cost from O(N) to O(logN) scale and saves the memory by a ratio of (logN-2)/b. Further experiments show that Auxo outperformance the basic idea by one to two orders of magnitude for time cost on insertion and queries and saves 10% memory on average. In addition, we proposes compacted Auxo to increase the memory utilizaiotn. 


## About the source code and date set

We have implemented the basic idea GSS_Chain (GSS_Chain.h), Auxo (Auxo.h) and compacted Auxo (ComAuxo.h) in C++. We complete the code on Linux 5.4.0-99-generic and compile successfully using gcc 7.5.0. The hash function we use is in the file querysupportstruct.h.

All the data sets we use can be download from the below website:
#### https://catalog.caida.org/details/dataset/passive_2015_pcap 
#### https://networkrepository.com 
#### http://dl.acm.org/citation.cfm?id=2488173

The data sets should be preprocessed to the format as below:

rawdata.txt -- The adjacency matrix of the network in whitespace-separated values format, with one edge per line
    The meaning of the columns are: 
        First column: ID of from node 
        Second column: ID of to node
        Third column: edge weight
        Fourth column: timestamp of the edge

edgeQueryTest.txt -- Test data for edge query
    The meaning of the columns are:                     
        First column: ID of from node
        First column: ID of to node
        Third column: Accumulated weight of the edge


nodeOutQueryTest.txt & nodeInQueryTest.txt -- Test data for node out (in)-flow query
    The meaning of the columns are:  
        First column: ID of from (to) node
        Second column: Accumulated weight of the node

##How to run

Suppose you've already cloned the respository and start from the `Auxo` directory and preprocessed the data set in to the format above
You just need:
    $ cd sourceCode
    $ make
    $ ./main
