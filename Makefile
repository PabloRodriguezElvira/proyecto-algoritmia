all: greedyIC greedyLT localSearchIC localSearchLT metaheuristicIC metaheuristicLT

greedyIC: greedyIC.cpp
	g++ -O3 -o greedyIC greedyIC.cpp

greedyLT: greedyLT.cpp
	g++ -O3 -o greedyLT greedyLT.cpp

localSearchIC: localSearchIC.cpp
	g++ -O3 -o localSearchIC localSearchIC.cpp
	
localSearchLT: localSearchLT.cpp
	g++ -O3 -o localSearchLT localSearchLT.cpp
	
metaheuristicIC: metaheuristicIC.cpp
	g++ -O3 -o metaheuristicIC metaheuristicIC.cpp
	
metaheuristicLT: metaheuristicLT.cpp
	g++ -O3 -o metaheuristicLT metaheuristicLT.cpp

clean: 
	rm greedyIC greedyLT localSearchIC localSearchLT metaheuristicIC metaheuristicLT
