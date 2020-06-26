sampleobjects = buffer_manager.o file_manager.o rtree.o main.o
# sampleobjects = buffer_manager.o file_manager.o sample_run.o rtree.o main.o

# sample_run : $(sampleobjects)
# 	    g++ -std=c++11 -o sample_run $(sampleobjects)

rtree : $(sampleobjects)
		g++ -std=c++11 -o rtree $(sampleobjects)

# sample_run.o : sample_run.cpp
# 	g++ -std=c++11 -c sample_run.cpp

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	g++ -std=c++11 -c file_manager.cpp

rtree.o : rtree.cpp
	g++ -std=c++11 -c rtree.cpp

main.o : main.cpp
	g++ -std=c++11 -c main.cpp

clean :
	rm -f *.o
	rm -f temp.txt
	# rm -f sample_run
	rm -f rtree
