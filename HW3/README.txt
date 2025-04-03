TODO: names of team members and their contributions to the project

Berndan
-README
-Trouble Shooting
-main.cpp

Cooper 
-cacheSimFuncs
-Trouble Shooting
-main.cpp

TODO (for MS3): best cache report
- For the gcc trace, I ran several tests starting with 256 sets, 4 blocks, of 16 bytes.
I ran a each combination of variables in total and found that write-allocate write-back had the
lowest number of cycles. For this file least recently used showed to have a slight optimization of first in first out.
This makes logical sense to me as least recently used would prioritize likelyhood of being called again
while first in first out just prioritizes when it was put into the cache. The total number of cycles was the only
variable that was different between write through and write back for no write allocate. The total cycles 
went down directly correlating with the lower number of misses. Additionally every single total cycle
ended with the number 83 which I did not have an explanation for. Total Cycles changed for write back compared to write through
in write allocate, but number of misses did not most likely because the writing back when storing has a cost of cycles
to it but does not have a cost of misses so write through and write back have the same number of everything but cycles. 
Write back showed to have the biggest drop of cycles for all variables most likely due to number of cycles having a heavy cost to into
when writing to the real memory. 

Data
./csim 256 4 16 no-write-allocate write-through lru < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 311613
Load misses: 6584
Store hits: 164819
Store misses: 32667
Total cycles: 32697983

./csim 256 4 16 write-allocate write-through lru < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 24394683

./csim 256 4 16 write-allocate write-back lru < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 9185283

Fifo

./csim 256 4 16 no-write-allocate write-through fifo < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 311017
Load misses: 7180
Store hits: 163705
Store misses: 33781
Total cycles: 33270583

./csim 256 4 16 write-allocate write-through fifo < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 314171
Load misses: 4026
Store hits: 188047
Store misses: 9439
Total cycles: 24706383

./csim 256 4 16 write-allocate write-back fifo < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 314171
Load misses: 4026
Store hits: 188047
Store misses: 9439
Total cycles: 9685683

./csim 128 8 8 no-write-allocate write-through lru < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 308828
Load misses: 9369
Store hits: 160789
Store misses: 36697
Total cycles: 25807783

./csim 128 8 8 write-allocate write-through lru < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 312750
Load misses: 5447
Store hits: 179419
Store misses: 18067
Total cycles: 23160383

./csim 128 8 8 write-allocate write-back lru < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 312750
Load misses: 5447
Store hits: 179419
Store misses: 18067
Total cycles: 7694283

./csim 128 8 8 no-write-allocate write-through fifo < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 307835
Load misses: 10362
Store hits: 160071
Store misses: 37415
Total cycles: 26078183

./csim 128 8 8 write-allocate write-through fifo < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 311524
Load misses: 6673
Store hits: 178850
Store misses: 18636
Total cycles: 23462483

./csim 128 8 8 write-allocate write-back fifo < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 311524
Load misses: 6673
Store hits: 178850
Store misses: 18636
Total cycles: 8260683

./csim 512 8 16 no-write-allocate write-through fifo < swim.trace 
Total loads: 220668
Total stores: 82525
Load hits: 218116
Load misses: 2552
Store hits: 58035
Store misses: 24490
Total cycles: 16923493

./csim 512 8 16 write-allocate write-through fifo < swim.trace 
Total loads: 220668
Total stores: 82525
Load hits: 219573
Load misses: 1095
Store hits: 71985
Store misses: 10540
Total cycles: 12155693

./csim 512 8 16 write-allocate write-back fifo < swim.trace 
Total loads: 220668
Total stores: 82525
Load hits: 219573
Load misses: 1095
Store hits: 71985
Store misses: 10540
Total cycles: 7348793

./csim 512 8 16 no-write-allocate write-through lru < swim.trace 
Total loads: 220668
Total stores: 82525
Load hits: 218138
Load misses: 2530
Store hits: 58050
Store misses: 24475
Total cycles: 16910193

./csim 512 8 16 write-allocate write-through lru < swim.trace 
Total loads: 220668
Total stores: 82525
Load hits: 219650
Load misses: 1018
Store hits: 72018
Store misses: 10507
Total cycles: 12114993

./csim 512 8 16 write-allocate write-back lru < swim.trace 
Total loads: 220668
Total stores: 82525
Load hits: 219650
Load misses: 1018
Store hits: 72018
Store misses: 10507
Total cycles: 7249193