# Project: Serial Dynamic Memory Allocator

#### Submitted by : ac2255@g.rit.edu
#### Time Spent On Project: 2 Weeks

### Description:
 The requirement was to implement a serial dynamic memory allocator in C. The implementation is graded using a performance index which is a function of the meory utilization as well as the throughput.
<br>
An additional requirement was to write a heap validator which throws an error based on when certian defined properties are violated by the heap.
<br>

### Boilerplate Walkthrough

* The memlib.c file simulates our heap environment by providing functions that initialize the heap using the sbrk() system call. There are additional functions to grow the heap as well as get its low and high addresses.
* mdriver.c calls the main() function and is responsible for recording the memory usage info.
* We are required to implement our logic in allocator.c. 
* The traces/ and additional_traces dir 

### Provided Implementation

* The code provided to us had a version of my_malloc() that used a system call every time that it was called. 
* For freeing the object no implementation was provided.
* The realloc was implemented using a memcpy() call. It has been kept unchanged.
* Since the malloc used system calls each time it was invoked, and since freeing did not actually recycle the memory, the overall memory usage (as a percentage of the heap size) was quite low. Below are the statistics across the 9 test cases 

```bash
ac2255@sulfur:~/phpc/MIT6_172F18-project3/mymalloc$ ./mdriver -v -t traces/
Using tracefiles in traces/
Measuring performance with gettimeofday().

Results for libc malloc:
trace                   filename     valid   checked  util     ops      secs Kops/sec
 0                   trace_c0_v0       yes        no    0%    5694  0.000553    10293
 1                   trace_c8_v0       yes        no    0%    8402  0.000641    13110
 2                   trace_c4_v0       yes        no    0%   18604  0.000619    30055
 3                   trace_c3_v0       yes        no    0%    5768  0.000231    24981
 4                   trace_c9_v0       yes        no    0%   14401  0.000438    32856
 5                   trace_c5_v0       yes        no    0%   30610  0.000522    58651
 6                   trace_c6_v0       yes        no    0%    7996  0.000269    29681
 7                   trace_c1_v0       yes        no    0%   25958  0.000647    40133
 8                   trace_c2_v0       yes        no    0%    4800  0.000758     6332
 9                   trace_c7_v0       yes        no    0%    3840  0.000158    24288
Geometric Mean                                          0%  126073  0.004837    22664
ERROR: mem_sbrk failed. Ran out of memory... (52372800)
ERROR [trace 4, line 2704]: impl realloc failed.

Results for mm malloc:
trace                   filename     valid   checked  util     ops      secs Kops/sec
 0                   trace_c0_v0       yes        no   23%    5694  0.000050   114108
 1                   trace_c8_v0       yes        no   25%    8402  0.000406    20710
 2                   trace_c4_v0       yes        no   31%   18604  0.000103   179749
 3                   trace_c3_v0       yes        no   27%    5768  0.000054   106225
 4                   trace_c9_v0        no        no     -       -         -       -
 5                   trace_c5_v0       yes        no    1%   30610  0.000296   103552
 6                   trace_c6_v0       yes        no   26%    7996  0.000151    52954
 7                   trace_c1_v0       yes        no    1%   25958  0.000260    99915
 8                   trace_c2_v0       yes        no   39%    4800  0.000042   114286
 9                   trace_c7_v0       yes        no    1%    3840  0.000022   172197
Geometric Mean                                           -       -         -       -

(throughput)          filename    libc    base      my        (util)
                   trace_c0_v0   10293   11322  114108   100%    23%
                   trace_c8_v0   13110   14421   20710   100%    25%
                   trace_c4_v0   30055   33060  179749   100%    31%
                   trace_c3_v0   24981   27479  106225   100%    27%
                   trace_c5_v0   58651   64000  103552   100%     1%
                   trace_c6_v0   29681   32649   52954   100%    26%
                   trace_c1_v0   40133   44146   99915   100%     1%
                   trace_c2_v0    6332    6965  114286   100%    39%
                   trace_c7_v0   24288   26717  172197   100%     1%
# GeometricMean(12.660564 (util),  100.000000 (tput))  =  35.581686
Terminated with 1 errors
ac2255@sulfur:~/phpc/MIT6_172F18-project3/mymalloc$ 
```

* In the project's version of the heap, the heap can only be grown and not shrunk. Therefore, to reiterate, since the heap is grown on each malloc and free does nothing, the performance is low.

### Approach #1 : Single Free List with First Fit

* The first approach that was implemented was a circular doubly linked list of free nodes. 
* my_malloc: When the program requests dynamic memory, the list of free nodes is searched for the block of appropriate size. Since our approach is first fit , the program selects the first block from the starting reference node that meets the size requirements (greater than or equal.)
* Our my_free() implementation took the pointer to be freed and added it to the "start" of our circular linked list, ie, next to the starting reference node. 
* We do not sort the blocks, in order to keep the free() in constant time.
* The stats are reported below:

```bash
ac2255@sulfur:~/phpc/project_3/mymalloc$ ./mdriver -v -t traces/
Using tracefiles in traces/
Measuring performance with gettimeofday().

Results for libc malloc:
trace                   filename     valid   checked  util     ops      secs Kops/sec
 0                   trace_c0_v0       yes        no    0%    5694  0.000545    10442
 1                   trace_c8_v0       yes        no    0%    8402  0.000643    13075
 2                   trace_c4_v0       yes        no    0%   18604  0.000630    29549
 3                   trace_c3_v0       yes        no    0%    5768  0.000231    24981
 4                   trace_c9_v0       yes        no    0%   14401  0.000442    32581
 5                   trace_c5_v0       yes        no    0%   30610  0.000524    58405
 6                   trace_c6_v0       yes        no    0%    7996  0.000269    29681
 7                   trace_c1_v0       yes        no    0%   25958  0.000655    39631
 8                   trace_c2_v0       yes        no    0%    4800  0.000762     6298
 9                   trace_c7_v0       yes        no    0%    3840  0.000158    24365
Geometric Mean                                          0%  126073  0.004859    22591
ERROR: mem_sbrk failed. Ran out of memory... (52402312)
ERROR [trace 4, line 2707]: impl realloc failed.

Results for mm malloc:
trace                   filename     valid   checked  util     ops      secs Kops/sec
 0                   trace_c0_v0       yes        no   71%    5694  0.000058    98512
 1                   trace_c8_v0       yes        no   37%    8402  0.000425    19755
 2                   trace_c4_v0       yes        no   47%   18604  0.019375      960
 3                   trace_c3_v0       yes        no   35%    5768  0.004445     1298
 4                   trace_c9_v0        no        no     -       -         -       -
 5                   trace_c5_v0       yes        no   80%   30610  0.000211   145209
 6                   trace_c6_v0       yes        no   38%    7996  0.000998     8012
 7                   trace_c1_v0       yes        no   71%   25958  0.000233   111456
 8                   trace_c2_v0       yes        no   76%    4800  0.000105    45889
 9                   trace_c7_v0       yes        no   33%    3840  0.000035   109091
Geometric Mean                                           -       -         -       -

(throughput)          filename    libc    base      my        (util)
                   trace_c0_v0   10442   11486   98512   100%    71%
                   trace_c8_v0   13075   14383   19755   100%    37%
                   trace_c4_v0   29549   32504     960     3%    47%
                   trace_c3_v0   24981   27479    1298     5%    35%
                   trace_c5_v0   58405   64000  145209   100%    80%
                   trace_c6_v0   29681   32649    8012    25%    38%
                   trace_c1_v0   39631   43594  111456   100%    71%
                   trace_c2_v0    6298    6928   45889   100%    76%
                   trace_c7_v0   24365   26802  109091   100%    33%
# GeometricMean(54.515153 (util),  45.023947 (tput))  =  49.542783
Terminated with 1 errors
``` 
* As you can see the performance goes up to 49, up from 35. This is due to increased memory recycling. For trace 0 the usage is 98%. This due to fixed pattern alloc and dealloc.  
* In this implementation we only call mem_sbrk() when an apropriate size block is not found.
* We now discuss a next approach to further improve performance.

### Approach #2 : Segregated Free List with First Fit

* In this approach we maintain a fixed size array of our free_node headers.
* Each index in the array is to define a size class.
* Each index has a size cap of 2 ^^ index.
* Each index has has a circular linked list which holds blocks of sizes [2^^ index-1 , 2 ^^ index].
* The allocation and deallocation mechanisms find the right index by size and search through the appropriate linked list. The algorithms for malloc and free are same as above.
* The stats are reported below:

```bash
ac2255@sulfur:~/phpc/project_3/mymalloc$ ./mdriver -v -t traces/
Using tracefiles in traces/
Measuring performance with gettimeofday().

Results for libc malloc:
trace                   filename     valid   checked  util     ops      secs Kops/sec
 0                   trace_c0_v0       yes        no    0%    5694  0.000458    12440
 1                   trace_c8_v0       yes        no    0%    8402  0.000540    15568
 2                   trace_c4_v0       yes        no    0%   18604  0.000556    33485
 3                   trace_c3_v0       yes        no    0%    5768  0.000222    25982
 4                   trace_c9_v0       yes        no    0%   14401  0.000418    34460
 5                   trace_c5_v0       yes        no    0%   30610  0.000514    59553
 6                   trace_c6_v0       yes        no    0%    7996  0.000271    29560
 7                   trace_c1_v0       yes        no    0%   25958  0.000646    40176
 8                   trace_c2_v0       yes        no    0%    4800  0.000751     6395
 9                   trace_c7_v0       yes        no    0%    3840  0.000157    24505
Geometric Mean                                          0%  126073  0.004531    24036
ERROR: mem_sbrk failed. Ran out of memory... (52403040)
ERROR [trace 4, line 2707]: impl realloc failed.

Results for mm malloc:
trace                   filename     valid   checked  util     ops      secs Kops/sec
 0                   trace_c0_v0       yes        no   98%    5694  0.000085    67067
 1                   trace_c8_v0       yes        no   55%    8402  0.000435    19302
 2                   trace_c4_v0       yes        no   71%   18604  0.000204    90973
 3                   trace_c3_v0       yes        no   43%    5768  0.000072    80446
 4                   trace_c9_v0        no        no     -       -         -       -
 5                   trace_c5_v0       yes        no   81%   30610  0.000342    89503
 6                   trace_c6_v0       yes        no   56%    7996  0.000998     8012
 7                   trace_c1_v0       yes        no   75%   25958  0.000292    88867
 8                   trace_c2_v0       yes        no   84%    4800  0.000133    36172
 9                   trace_c7_v0       yes        no   22%    3840  0.000074    51682
Geometric Mean                                           -       -         -       -

(throughput)          filename    libc    base      my        (util)
                   trace_c0_v0   12440   13685   67067   100%    98%
                   trace_c8_v0   15568   17125   19302   100%    55%
                   trace_c4_v0   33485   36833   90973   100%    71%
                   trace_c3_v0   25982   28580   80446   100%    43%
                   trace_c5_v0   59553   64000   89503   100%    81%
                   trace_c6_v0   29560   32516    8012    25%    56%
                   trace_c1_v0   40176   44194   88867   100%    75%
                   trace_c2_v0    6395    7034   36172   100%    84%
                   trace_c7_v0   24505   26956   51682   100%    22%
# GeometricMean(63.330675 (util),  86.928944 (tput))  =  74.197498
Terminated with 1 errors
```

* In both our implementations the last trace case fails due to running out of memory. Our allocator has a max heap size of 50 MB.
* But we can observe that the performance now goes up to 74 across all the cases. 
* This can be explained due to better performance of first fits because of binning the malloc requests by size, so that leads to less internal fragmentation.

### Validator

* We also write a heap validator in validator.h that checks the following properties:
  - Allocated ranges returned by the allocator must be aligned to 8 bytes.
  - Allocated ranges returned by the allocator must be within the heap.
  - Allocated ranges returned by the allocator must not overlap.
  - When calling my_realloc() on an existing allocation, the original data must be intact (up to the reallocated size).

* Our validator does not throw errors when ./mdriver is run, meaning that the heap is consistent as per OUR implementation of the validator.
