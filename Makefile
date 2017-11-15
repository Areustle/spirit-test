all: timeline testtimeline

timeline:
	g++ -c  -o ./timeline.o  -O2 -Wall --pedantic -Wno-comment -Wno-long-long -fexceptions -g  -I./ timeline.cxx


testtimeline:
	g++ -O2 -Wall --pedantic -Wno-comment -Wno-long-long -fexceptions -g  -I./ testtimeline.cpp timeline.o
