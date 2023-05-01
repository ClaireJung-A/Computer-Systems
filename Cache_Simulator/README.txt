Claire Jung

Worked solo.

-------------------------------------------------------------------------------------------------------
Best Configuration:
"256 512 1024 write-allocate write-back lru"

Experiment 1 would be testing the combinations for the booleans with certain set data capacity.
Here, I would be using 256 4 16 as the three data capacity as these were the three given values in the
assignment instructions.
Experiment 2 would be testing for the best numerical data capacity values for the cache.
------
Experiment 1
The three booleans to test are
    - write-allocate / no-write-allocate
    - write-back / write-through
    - fifo / lru.

DATA:
a)
./csim 256 4 16 write-allocate write-back fifo < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 218357
Load misses: 2311
Store hits: 71787
Store misses: 10738
Total cycles: 10300793

b)
./csim 256 4 16 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 219507
Load misses: 1161
Store hits: 71956
Store misses: 10569
Total cycles: 9243593

c)
./csim 256 4 16 write-allocate write-through fifo < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 218357
Load misses: 2311
Store hits: 71787
Store misses: 10738
Total cycles: 13775293

d)
./csim 256 4 16 write-allocate write-through lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 219507
Load misses: 1161
Store hits: 71956
Store misses: 10569
Total cycles: 13247693

e)
./csim 256 4 16 no-write-allocate write-through fifo < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 217968
Load misses: 2700
Store hits: 57994
Store misses: 24531
Total cycles: 9611162

f)
./csim 256 4 16 no-write-allocate write-through lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 218072
Load misses: 2596
Store hits: 58030
Store misses: 24495
Total cycles: 9569598

ANALYSIS:
	        a	        b	    c	        d	        e	    f
Load Hits	218357	    219507	218357	    219507	    217968	218072
Load Miss	2311	    1161	2311	    1161	    2700	2596
Store Hits	71787	    71956	71787	    71956	    57994	58030
Store Miss	10738	    10569	10738	    10569	    24531	24495
Total Cycle	10300793    9243593	13775293	13247693	9611162	956598

Looking at the data, we can first rule out a,c, and d.
Data a is write-allocate write-back fifo
Data c is write-allocate write-through fifo
Data d is write-allocate write-through lru
From this we can conclude that write-allocate write through is not a good configuration for a cache.

No-write allocate/write-through, which is data e and f, had a relatively good total cycle, but these
configuration had the most load miss and store miss. Even considering the fact that
there are no severe penalty misses for (no-write-allocate, write) through compared to (write-allocate, write back),
the cost of 2 cycles of (write allocate, write back) will cost more than a flat 100 cycle in no-write-allocate, write through).

From all this, we can exclude data e and f.
Therefore, the best configuration for a cache which has the lowest total cycles and miss rate is
"write allocate write-back lru"
--------------------
Experiment 2:
The three numerical values to test are
    - number of sets
    - associativity factor
    - block size

I am going to use small and large values for each of the three values and find the best configuration as I did above.

1)
./csim 1 4 16 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 64398
Load misses: 156270
Store hits: 39571
Store misses: 42954
Total cycles: 159669993

2)
./csim 1 256 16 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 215319
Load misses: 5349
Store hits: 71640
Store misses: 10885
Total cycles: 13162393

3)
./csim 1 1 64 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 62584
Load misses: 158084
Store hits: 37316
Store misses: 45209
Total cycles: 650829593


4)
./csim 1 256 64 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 220127
Load misses: 541
Store hits: 79422
Store misses: 3103
Total cycles: 11475993

5)
./csim 256 1 16 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 211533
Load misses: 9135
Store hits: 70212
Store misses: 12313
Total cycles: 17336793


6)
./csim 256 256 16 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 219742
Load misses: 926
Store hits: 72028
Store misses: 10497
Total cycles: 4872393

7)
./csim 256 4 64 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 220217
Load misses: 451
Store hits: 79454
Store misses: 3071
Total cycles: 9858393

8)
./csim 256 16 64 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 220275
Load misses: 393
Store hits: 79465
Store misses: 3060
Total cycles: 5827993

The three numerical values to test are
    - number of sets
    - associativity factor
    - block size

1: small	small	small --- fully associative
2: small	large	small --- fully associative
3: small	small	large --- fully associative, directly mapped
4: small	large	large --- fully associative, directly mapped
5: large	small	small
6: large	large	small
7: large	small	large
8: large	large	large

ANALYSIS:
	        1	        2	        3	        4	        5	    6   	7	    8
Load Hits	64398	    215319	    62584	    220127	    219507	219742	220217	220275
Load Miss	156270	    5349	    158084	    541	        1161	926 	451	    393
Store Hits	39571	    71640	    37316	    79422	    71956	72028	79454	79465
Store Miss	42974	    10885	    45209       3103	    10569	10497	3071	3060
Total Cycle	159669993	13162393	650829593	11475993	9243593	4872393	9858393	5827993

The first pattern I saw in the dataset is the associativity has high correlation with the total cycle.
When there is low associativity factor, the total cycle tended to be higher, and vice versa.
Also, next thing I realized is that dataset 4 and 8 has extremely low miss rates and while having the
similar hit rates with the other datasets.
Dataset 4 is having high associativity factor and block size.
The next pattern I realized is the frequent data set in hit and miss rates in both load and store in the last
four dataset. Whereas the first half of the dataset (data 1 to 4) has fluctuating hit and miss rates showing
instability, the latter half of the dataset is more frequent.

From these former experiment, we can get a ballpark of what dataset we use to further investigate.
It seems like the best configuration for a cache has large number of sets, associativity factor and block size.
Just to make sure, I decided to compare with 16 sets, 32 blocks, 16 bytes and double the size to see if there is an
exact pattern as I predicted.

./csim 16 32 16 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 218898
Load misses: 1770
Store hits: 71862
Store misses: 10663
Total cycles: 10007993

./csim 32 64 32 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 220014
Load misses: 654
Store hits: 76708
Store misses: 5817
Total cycles: 8911193


./csim 64 128 64 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 220275
Load misses: 393
Store hits: 79465
Store misses: 3060
Total cycles: 5827993

./csim 128 256 128 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 220396
Load misses: 272
Store hits: 80971
Store misses: 1554
Total cycles: 6146393

./csim 256 512 512 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 220523
Load misses: 145
Store hits: 82108
Store misses: 417
Total cycles: 7496793

./csim 256 512 1024 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 220560
Load misses: 108
Store hits: 82299
Store misses: 226
Total cycles: 8853593


As I predicted, there is a pattern shown as I increase the sizes. The hit tends to increase, miss rate decreases and
even the total cycle tend to decrease.

We can conclude that the best configuration for a cache which has the lowest total cycles and miss rate is
"256 512 1024 write-allocate write-back lru"

