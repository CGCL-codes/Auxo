# Auxo: A Time and Memory Efficient Structure for Scalable Graph Stream Summarization
## Introduction

A graph stream refers to a time sequentially updating stream of edges, forming a huge and fast-evolving graph. The vast volume and high update speed of a graph stream bring stringent requirements for the data management structure, including sublinear space cost, constant time cost for updating, and scalability of the structure. Existing designs summarize a graph stream by leveraging a hash-based compressed matrix and representing an edge using its fingerprint to achieve practical storage for a graph stream with a known upper bound of data volume. However, they fail to support the dynamically extending of a graph stream.

To address this issue, we propose a novel prefix embedded tree (PET) which leverages binary logarithmic search and common binary prefixes embedding to provide a space/time efficient scalable tree structure. PET reduces the item insert/query time from $O(N)$ to $O(logN)$ as well as reducing the total storage cost in a $logN$ scale, where $N$ is the scale of the dataset. To further improve the memory utilization of PET during scaling, we further propose a proportionally incremental strategy. Based on the PET and proportionally incremental strategy, we propose Auxo and proportional Auxo. We conduct comprehensive experiments on large-scale real-world datasets to evaluate the performance of this design. Results show that Auxo significantly reduces the insert and query time by one to two orders of magnitude compared to existing designs. Meanwhile, Auxo achieves efficiently and economically structure-scaling.  


## About the source code and data sets

We have implemented the basic idea GSS_Chain (GSS_Chain.cpp), Auxo (Auxo.cpp), and proportional Auxo (AuxoPro.cpp) in C++. We complete the code on Linux 5.4.0-99-generic and compile successfully using gcc 7.5.0. The hash function we use is in the file querysupportstruct.h.

All the data sets we use can be downloaded from the below website (In the "data" directory, we also upload a small test dataset sample "lkml" due to the size limitation of the project uploaded):  
```
    https://catalog.caida.org/details/dataset/passive_2015_pcap  
    https://networkrepository.com  
    http://dl.acm.org/citation.cfm?id=2488173
```

The data sets should be preprocessed to the format as below:  
* rawdata.txt -- The adjacency matrix of the network in whitespace-separated values format, with one edge per line  
    The meaning of the columns are:  
    ```
        First column: ID of source node  
        Second column: ID of destination node  
        Third column: edge weight  
        Fourth column: timestamp of the edge  
     ```

* edgeQueryTest.txt -- Test data for edge query
    The meaning of the columns are:
    ```
        First column: ID of source node  
        First column: ID of destination node  
        Third column: Accumulated weight of the edge  
    ```


* nodeOutQueryTest.txt & nodeInQueryTest.txt -- Test data for node out (in)-flow query
    The meaning of the columns are:  
    ```
        First column: ID of source (destination) node  
        Second column: Accumulated weight of the node  
    ```

## How to run

Suppose you've already cloned the repository.  
You just need:
```
    $ cd code  
    $ make  
    $ ./main

## Other related work

We also collect other related work of graph stream summarization strutures like Horae, Scube, MoSSo, SGS, GS4 and their codes are in the "baseline" directory.   
