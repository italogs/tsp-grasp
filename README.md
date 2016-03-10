BEGIN

	Travelling Salesman Problem (TSP) implemented in Grasp.

	So far, I've implemented using 2-opt(TWO_OPT) and Simple Swap(SWAP) as local seach procedures.

	The Makefile outputs to src/exit.txt. But you can change it as your preference.

	By default, the source code outputs the graph vertex and edges in Latex style. You can comment the function printCircuitLatex() in main to prevent the printing.

	Format:
	./grasp <file> <alpha> <grasp_iterations> <RCL size> <local_search_type>

	local_search_type:
	TWO_OPT - 2-opt
	SWAP - simple swap
	To be developed:
	THREE_OPT - 3-opt
	OR_OPT - Or-Opt
	LK - Lin Kernighan
	CW - Clark Wright
	CI - Cheapest Insertion
	RI - Random Insertion

	The file must contains the line 'DIMENSION:' before the first line of coordinates.
	See dj38.tsp for basic example. The data before 'DIMENSION:' will be ignored.
	To avoid unexpected behavior, add EOF at the end.

	Example:
	./grasp att48.tsp 0.0025 100 5 TWO_OPT > exit.txt

	Incoming improvements.
	Suggestions are welcomed.

END