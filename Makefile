all: sample 

sample: graph.o sample.o
	g++ -Wall graph.o sample.o -o sample 

graph.o: graph.cpp
	g++ -Wall -c graph.cpp

clean:
	rm *o multilayer

