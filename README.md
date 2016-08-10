# lis_stream
We provide an implementation of a LIS computation algorithms for LIS enumeration and constrained LIS enumeration in the data stream model.

# Use
### Install
ALL codes are implemented only in standard C++. 
You can just import this project into Eclipse for C/C++ with the Git plugin.

### API
For each method in {quadruple, canonical, lisw, range, slope, variant, dynprogram}, 
we provide a run_microsoft(win_size) function where explicitly present the corresponding 
running examples. 

Following are the key components of the project.

1.  datastream: 
	datastream/datastream.h, datastream/datastream.cpp.
output item one by one from a given file.

2.  Key components in each method:
2.1 Update(newItem)
Containing two main operations: insertion and deletion

2.2 Construct(Sequence)
Constructing the corresponding data structure over the inputted sequence.

2.3 Compute_str()
Return the target LIS(s).

2.4 Initialization: constructor of each method
The key parameters: win_size and data_set.

2.5 Run(dataset)
Run the algorithm over given dataset.

3.  util:
	util/util.h, util/util.cpp
Provide global control(functions and variables) over projects

4.  runtime:
	util/runtime.h, util/runtime.cpp
Collect the running time of each method.
 
### See the following paper for details on our algorithms.
-- quadruple from
1. Y. Li, L. Zou, H. Zhang, and D. Zhao. Computing longest increasing
subsequence over sequential data streams. arXiv preprint
arXiv:1604.02552, 2016.
http://arxiv.org/abs/1604.02552.

Besides, we also implemnet some comparative algorithms as follows:
--variant from
2. S. Deorowicz. On Some Variants of the Longest Increasing
Subsequence Problem. Theoretical and Applied Informatics,
21(3):135每148, 2009.

--minheight from
3. C.-t. Tseng, C.-b. Yang, and H.-y. Ann. Minimum Height and
Sequence Constrained Longest Increasing Subsequence. Journal of
internet Technology, 10:173每178, 2009.

-- range & slope from
4. I.-H. Yang and Y.-C. Chen. Fast algorithms for the constrained
longest increasing subsequence problems. In Proceedings of the 25th
Workshop on Combinatorial Mathematics and Computing Theory,
pages 226每231, 2008.

-- canonical from
5. E. Chen, L. Yang, and H. Yuan. Longest increasing subsequences in
windows based on canonical antichain partition. Theoretical
Computer Science, 378(3):223每236, June 2007.

-- lisw from
6. M. H. Albert, A. Golynski, A. M. Hamel, A. L車pez-Ortiz, S. Rao,
and M. A. Safari. Longest increasing subsequences in sliding windows. 
Theoretical Computer Science, 321(2-3):405每414, Aug. 2004.
