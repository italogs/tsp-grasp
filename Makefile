exec: compile
	./main dj38.tsp 0.0025 500 5 TWO_OPT > exit.txt
 
compile: mtrand
	g++ main.cpp mtrand.o -Wall -Werror -pedantic -Wextra -O3 -o main 
mtrand:
	g++ mtrand.cpp -c