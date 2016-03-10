#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <climits>
#include <cmath>
#include "mtrand.h"
using namespace std;


//Local Optimization 
enum {
	SWAP, TWO_OPT
};

//Input data will be saved in this struct
typedef struct graph {
	float x,y;
	int label;
}graph;

//Class for Solution
class Solution{
	private:
		float cost;
		int *circuit, size_circuit;
	public:
		Solution(int size_circuit){
			this->size_circuit = this->cost = 0;
			this->circuit = new int[size_circuit];
		}
		float getCost(){
			return this->cost;
		}
		void addCost(float cost) {
			this->cost += cost;
		}
		void setCost(float new_cost){
			this->cost = new_cost;
		}
		void includeLabel(int label) {
			circuit[size_circuit++] = label;
		}
		int getSize_circuit(){
			return size_circuit;
		}
		void setSize_circuit(int size_circuit){
			this->size_circuit = size_circuit;
		}
		int getLastLabel(){
			return circuit[size_circuit-1];
		}
		int getLabelByIndex(int index){
			if(index == size_circuit)
				return circuit[0];
			if(index < 0 )
				return circuit[size_circuit-1];
			return circuit[index];
		}
		void setLabelByIndex(int index,int label){
			circuit[index] = label;
		}		
};

class Grasp {
	private:
		int MAX_ITERATIONS,size_graph,size_circuit,size_RCL,local_search_type;
		Solution *best_solution;
		graph *input;
		MTRand drand;
		time_t seconds;
	public:
		Grasp(int MAX_ITERATIONS,graph *input,int size_graph, int size_RCL,int local_search_type){
			this->MAX_ITERATIONS = MAX_ITERATIONS;
			best_solution = NULL;
			this->input = input;
			this->size_graph = size_graph;
			this->size_circuit=0;
			time(&seconds);
			drand.seed((unsigned int)seconds);
			this->size_RCL = size_RCL;
			this->local_search_type = local_search_type;
		}
		~Grasp(){
			delete best_solution;
		}
		int getRandomNumberByInterval(int min,int max) {
			return (int)(min + drand() * (max+1 - min));
		}
		Solution *getBest_Solution(){
			return this->best_solution;
		}
		float getCostBestSolution(){
			return this->best_solution->getCost();
		}
		void execute(float alpha){
			Solution *solution;
			for (int i = 0 ; i < this->MAX_ITERATIONS ; i++) {
				solution = Greedy_Randomized_Constrution(alpha);
				solution = Local_Search(solution);	
				Update_Solution(solution);
			}
		}
		float calculateCost(int i,int j){
			i--;
			j--;
			if(i < 0) i = size_circuit-1;
			if(j < 0) j = size_circuit-1;
			return sqrt( pow(input[i].x - input[j].x,2) + pow(input[i].y - input[j].y,2));
		}
		void printCircuit(Solution *solution){
			for (int i = 0; i < solution->getSize_circuit(); i++){
				cout<<solution->getLabelByIndex(i)<<"->";
			}
			cout<<endl;
		}
		void printCircuitLatex(){
			float scale = 100.0;
			for (int i = 0; i < best_solution->getSize_circuit(); i++){
				cout<<"\\node[draw,circle] ("<<input[best_solution->getLabelByIndex(i)-1].label<<") at ("<<(input[best_solution->getLabelByIndex(i)-1].x/scale)<<","<<(input[best_solution->getLabelByIndex(i)-1].y/scale)<<"){"<<input[best_solution->getLabelByIndex(i)-1].label<<"};"<<endl;
			}
			for (int i = 0; i < size_graph-1; i++){
				cout<<"\\draw[-,line width=0.1pt] ("<<best_solution->getLabelByIndex(i)<<") -- ("<<best_solution->getLabelByIndex(i+1)<<");"<<endl;
			}
			cout<<"\\draw[-,line width=0.1pt] ("<<best_solution->getLabelByIndex(size_graph-1)<<") -- ("<<best_solution->getLabelByIndex(0)<<");"<<endl;
		}
		Solution *Update_Cost(Solution *solution) {
			solution->setCost(0);
			for (int i = 0; i < solution->getSize_circuit()-1; i++) {
				solution->addCost(calculateCost(solution->getLabelByIndex(i),solution->getLabelByIndex(i+1)));
			}
			solution->addCost(calculateCost(solution->getLabelByIndex(solution->getSize_circuit()-1),solution->getLabelByIndex(0)));
			return solution;
		}

		Solution *Greedy_Randomized_Constrution(float alpha) {
			Solution *partial_solution = new Solution(size_graph);
			int counter_restrict_candidate_list = 0, selected_candidate;
			int *restrict_candidate_list = new int[this->size_RCL], *candidate_set = new int[size_graph];;
			float c_min, c_max, cost,cost_threshold;

			for(int i=0;i<size_graph;i++)
				candidate_set[i] = input[i].label;
			
			//putting first vertex in the solution and set it to 'unavaiable'
			partial_solution->includeLabel(candidate_set[0]);
			candidate_set[0] = -1;

			while(partial_solution->getSize_circuit() < size_graph) {
				c_min = INT_MAX;//+inf
				c_max = INT_MIN;//-inf

				for (int i = 1; i < size_graph; i++){

					if(candidate_set[i] == -1) 
						continue;

					cost = calculateCost(partial_solution->getLastLabel(),candidate_set[i]);
					if(cost > c_max)
						c_max = cost;
					if(cost < c_min)
						c_min = cost;
				}

				cost_threshold = c_min + alpha * (c_max - c_min);

				counter_restrict_candidate_list = 0;

				// fill the RCL
				for (int i = 1; i < size_graph && counter_restrict_candidate_list != this->size_RCL; i++){
					if(candidate_set[i] == -1)
						continue;
					cost = calculateCost(partial_solution->getLastLabel(),candidate_set[i]);
					if(cost <= cost_threshold) {
						restrict_candidate_list[counter_restrict_candidate_list++] = candidate_set[i];
					}
				}
				selected_candidate = restrict_candidate_list[getRandomNumberByInterval(0,counter_restrict_candidate_list-1)];
				partial_solution->addCost( calculateCost(partial_solution->getLastLabel(),selected_candidate) );
				partial_solution->includeLabel( selected_candidate );

				candidate_set[selected_candidate-1] = -1;
			}
			//add the cost from the last vertex to the first vertex
			partial_solution->addCost( calculateCost(partial_solution->getLastLabel(),candidate_set[0]));
			return partial_solution;
		}

		Solution *Swap(Solution *partial_solution) {			
			int best_index, temp_label;
			float best_cost,current_cost,i_cost,new_cost;

			for (int i = 1; i < partial_solution->getSize_circuit(); i++) {
				best_index = i;
				best_cost = INT_MAX;
				i_cost = 	calculateCost(partial_solution->getLabelByIndex(i),partial_solution->getLabelByIndex(i+1)) +
							calculateCost(partial_solution->getLabelByIndex(i),partial_solution->getLabelByIndex(i-1));

				for (int j = i+1; j < partial_solution->getSize_circuit(); j++) {
					temp_label = partial_solution->getLabelByIndex(j);
					current_cost = i_cost + 
								calculateCost(partial_solution->getLabelByIndex(j),partial_solution->getLabelByIndex(j+1)) + 
								calculateCost(partial_solution->getLabelByIndex(j),partial_solution->getLabelByIndex(j-1));

					//when we replace i by j
					new_cost = 	calculateCost(partial_solution->getLabelByIndex(i),partial_solution->getLabelByIndex(j+1)) +
								calculateCost(partial_solution->getLabelByIndex(i),partial_solution->getLabelByIndex(j-1)) +
								calculateCost(partial_solution->getLabelByIndex(j),partial_solution->getLabelByIndex(i+1)) +
								calculateCost(partial_solution->getLabelByIndex(j),partial_solution->getLabelByIndex(i-1));

					if(current_cost < best_cost){
						best_cost = new_cost;
					}
					if(new_cost < best_cost) {
						best_cost = new_cost;
						best_index = j;
					}
				}
				if(best_index != i){
					//temp = a; a=b;b=temp;
					temp_label = partial_solution->getLabelByIndex(i);
					partial_solution->setLabelByIndex(i,partial_solution->getLabelByIndex(best_index));
					partial_solution->setLabelByIndex(best_index,temp_label);
				}	
			}
			partial_solution = Update_Cost(partial_solution);
			return partial_solution;
		}
		Solution *Two_Opt(Solution *partial_solution){//only a draft, not the final version of this function
			int improve = 0;
			float new_cost = 0;
			Solution *new_solution = NULL, *temp = NULL;
			while(improve < 20) {
				float best_cost = partial_solution->getCost();
				for (int i = 0; i < partial_solution->getSize_circuit()-1; i++) {

					for (int j = i+1; j < partial_solution->getSize_circuit(); j++) {
						new_solution = NULL;
						new_solution = Two_Opt_Swap(partial_solution,i,j);

						new_cost = new_solution->getCost();
						if(new_cost < best_cost	){
							improve = 0;
							temp = partial_solution;
							partial_solution = new_solution;
							delete temp;
							best_cost = new_cost;
						}
					}
				}
				improve++;
			}
			partial_solution = Update_Cost(partial_solution);
			return partial_solution;
		}
		Solution *Two_Opt_Swap(Solution *partial_solution,int i,int j){
			Solution *new_solution = new Solution(size_graph);
	 		for ( int c = 0; c <= i - 1; c++ ) {
	 			new_solution->setLabelByIndex(c,partial_solution->getLabelByIndex(c));
	   		}
	   		int dec = 0;
		    for ( int c = i; c <= j; c++ ) {
		    	new_solution->setLabelByIndex(c,partial_solution->getLabelByIndex(j-dec));
		        dec++;
		    }
		    for ( int c = j + 1; c < partial_solution->getSize_circuit(); c++ ) {
		    	new_solution->setLabelByIndex(c,partial_solution->getLabelByIndex(c));
			}
			new_solution->setSize_circuit(size_graph);
			new_solution = Update_Cost(new_solution);
			return new_solution;
		}
		Solution *Local_Search(Solution *partial_solution){
			if(this->local_search_type == SWAP)
				partial_solution = Swap(partial_solution);
			else if(this->local_search_type == TWO_OPT)
				partial_solution = Two_Opt(partial_solution);
			return partial_solution;
		}
		void Update_Solution(Solution *partial_solution){
			// if first solution or partial_solution is better than the best_solution then update
			if(best_solution && partial_solution->getCost() < best_solution->getCost()){
				delete best_solution;
				best_solution = partial_solution;
			} else if(!best_solution)
				best_solution = partial_solution;
			else 
				delete partial_solution;
		}
};


int main(int argc, char *argv[]){
	//MAIN PARAMETERS
	if(argc != 6){
		printf("Incorrect parameters number.\n");
		return 0;
	}
	ifstream file(argv[1]);
	float alpha = atof(argv[2]);
	int times_execute = atoi(argv[3]), dimensions = 0,size_RCL = atoi(argv[4]);
	int local_search_type;
	if(strcmp(argv[5],"SWAP")==0) {
		local_search_type = SWAP;
	} else if(strcmp(argv[5],"TWO_OPT")==0) {
		local_search_type = TWO_OPT;
	} else {
		printf("Please, define a local search to be executed. See README.md\n");
		return 0;
	}
	string x,y,label,line;
	while(file){
		getline(file,line);
		if(line.substr(0,10)=="DIMENSION:")
			break;
	}
	dimensions = atoi(line.substr(10).c_str());
	graph *myGraph = new graph[dimensions];

	for (int i = 0; !file.eof();i++) {
		file >> label >> x >> y;
		if(label == "EOF") 
			break;
		myGraph[i].x = atof(x.c_str());
		myGraph[i].y = atof(y.c_str());
		myGraph[i].label = atoi(label.c_str());
	}

	Grasp g(times_execute,myGraph,dimensions,size_RCL,local_search_type);
	g.execute(alpha);

	cout<<"Best Solution: "<<g.getCostBestSolution()<<endl;
	cout<<"Optimal: 6656"<<endl;
	g.printCircuitLatex();	
	file.close();
	delete[] myGraph;
	return 0;
}
