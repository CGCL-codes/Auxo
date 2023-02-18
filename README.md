# Auxo: A Scalable and Efficient Graph Stream Summarization Structure

Auxo is a scalable graph stream summarization structure with high efficiency. Auxo leverages binary logarithmic search and common binary prefixes embedding to provide a space/time efficient scalable tree structure, reducing the item insert/query time from $O(N)$ to $O(logN)$ as well as reducing the total storage cost in a $logN$ scale.

## Introduction

A graph stream refers to a time sequentially updating stream of edges, forming a huge and fast-evolving graph. The vast volume and high update speed of a graph stream bring stringent requirements for the data management structure, including sublinear space cost, constant time cost for updating, and scalability of the structure. Existing designs summarize a graph stream by leveraging a hash-based compressed matrix and representing an edge using its fingerprint to achieve practical storage for a graph stream with a known upper bound of data volume. However, they fail to support the dynamically extending of a graph stream.  

Existing graph stream summarizations represent the original graph stream with a hash-based compressed matrix and denote the items using their Boolean labels, achieving an approximate and practical storage scheme with sublinear memory cost. Howeer, their size-predefined compressed structures do not scale facing real-world graph streams where edges arrive continuously with unknown bounds.

To address this issue, we propose Auxo whihc is based on a novel prefix embedded tree (PET), leveraging binary logarithmic search and common binary prefixes embedding to provide a space/time efficient scalable tree structure. PET reduces the item insert/query time from $O(|E|)$ to $O(log|E|)$ as well as reducing the total storage cost in a $log|E|$ scale, where $|E|$ is the scale of the dataset.

Below shows our inisght of PET structure. PET extends new building blocks in a tree-style to achieve logarithmic computation cost for insert/query processing. Moreover, PET embeds the prefix information inside the tree. Thus, in all the extended building blocks on the 𝑖th level of the tree, Auxo can omit an 𝑖-bit prefix for every inserted fingerprint without sacrificing query accuracy.

## Insight of PET
### ![Insight of PET](https://github.com/CGCL-codes/Auxo/blob/main/imgs/insight.png)

To further improve the memory utilization of PET, we propose a proportionally incremental strategy to expand PET by exploiting the principle of proportional sequence. Our proposed incremental expanding scheme decomposes the exponentially expanding of a new level into a proportionally incremental sequence, avoiding the under-utilization of memory. Below is an example of our proportional incremental strategy. Every time when the Main tree(a PET) extends to the next level, it constructs a Deputy tree to do this in a level-by-level way. Whenever Deputy tree extends, it moves the edge stored in the current level to the newest level and free the memory of the old level.
## Proportional incremental strategy
<img src="[https://github.com/仓库名/项目名/blob/master/m/20200424230936851.png](https://github.com/CGCL-codes/Auxo/blob/main/imgs/Auxo_pro.png)" width="200" height="200" alt="Proportional incremental strategy"/><br/>

## About the source code and data sets

We have implemented the basic idea GSS_Chain (GSS_Chain.cpp), Auxo (Auxo.cpp), and proportional Auxo (AuxoPro.cpp) in C++. The source code is in the "Auxo" directory. We complete the code on Linux 5.4.0-99-generic and compile successfully using gcc 11.3.0.

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
    $ cd Auxo  
    $ make  
    $ ./main
    
```

## Other related work

We also collect other related work of graph stream summarization strutures to compare and put their code in the "baseline" directory.   

## Other related work

We also collect other related work of graph stream summarization strutures to compare and put their code in the "baseline" directory. 

## Publication

If you want to know more detailed information, please refer to this paper:

Zhiguo Jiang, Hanhua Chen, Hai Jin. Auxo: A Scalable and Efficient Graph Stream Summarization Structure. In Proceedings of the 49th International Conference on Very Large Data Bases (VLDB 2023), Vancouver, Canada, August 28 to September 1, 2023.

## Authors and Copyright

Auxo is developed in National Engineering Research Center for Big Data Technology and System, Cluster and Grid Computing Lab, Services Computing Technology and System Lab, School of Computer Science and Technology, Huazhong University of Science and Technology, Wuhan, China by Zhiguo Jiang (jiangzg@hust.edu.cn), Hanhua Chen (chen@hust.edu.cn), Hai Jin (hjin@hust.edu.cn).

Copyright (C) 2021, [STCS & CGCL](http://grid.hust.edu.cn/) and [Huazhong University of Science and Technology](https://www.hust.edu.cn/).

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

```
  http://www.apache.org/licenses/LICENSE-2.0
```
Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
