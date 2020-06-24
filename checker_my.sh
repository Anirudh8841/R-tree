# Build
make clean
make

# ./rtree Testcases/TC_3/bul3.txt 10 3 output_tc3.txt 
# cmp --silent output_tc3.txt Testcases/TC_3/answer_3_10_1000_10000.txt && echo 'TC3: SUCCESS' || echo 'TC3: FAILED'

# make clean
# make
# # Test case 3
./rtree in_bul3_1.txt 10 3 output_tc3.txt 

# ./rtree Testcases/TC_3/queries_3_10_1000_10000.txt 10 3 output_tc3.txt 
cmp --silent output_tc3.txt Testcases/TC_3/answer_3_10_1000_10000.txt && echo 'TC3: SUCCESS' || echo 'TC3: FAILED'

# make clean
# make
# # Test case 4
# ./rtree Testcases/TC_4/queries_5_20_1000_1000.txt 20 5 output_tc4.txt
# cmp --silent output_tc4.txt Testcases/TC_4/answer_5_20_1000_1000.txt && echo 'TC4: SUCCESS' || echo 'TC4: FAILED'

# make clean
# make
# # Test case 5
# ./rtree Testcases/TC_5/queries_2_10_100000_100000.txt 10 2 output_tc5.txt
# cmp --silent output_tc5.txt Testcases/TC_5/answer_2_10_100000_100000.txt && echo 'TC5: SUCCESS' || echo 'TC5: FAILED'

# make clean
# make
# # Test case 6
# ./rtree Testcases/TC_6/queries_2_10_100_100.txt 10 2 output_tc6.txt
# cmp --silent output_tc1.txt Testcases/TC_1/answer_2_10_100_100_bulk.txt && echo 'TC6: SUCCESS' || echo 'TC6: FAILED'

# make clean
# make
# # Test case 7
# ./rtree Testcases/TC_7/queries_2_10_100_100.txt 10 2 output_tc7.txt
# cmp --silent output_tc1.txt Testcases/TC_1/answer_2_10_100_100_insert.txt && echo 'TC7: SUCCESS' || echo 'TC7: FAILED'


# ./rtree Testcases/TC_6/queries_2_10_100_100.txt 10 2 output_tc6.txt > o6.txt
