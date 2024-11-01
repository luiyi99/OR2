/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : tsp.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tsp.h"
#include "./utility/utility.h"
#include "algorithms/refinement/refinement.h"
#include "algorithms/nearestneighbor/nearestneighbor.h"
#include "algorithms/random/random.h"
#include "algorithms/cplex/cplex.h"
#include "algorithms/cplex/matheuristics/hardfixing/hardfixing.h"
#include "utility/utility.h"

/*
* IP x
* OR $x rounded to the nearest integer
* Reference:
*	- http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/
*	- Page 6: http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp95.pdf
*/
int nint(double x){

    return (int)(x + .5);

}/* nint */

/*
* IP inst instance
* IP sol solution
* OR cost of the solution $sol
*/
double getSolCost(const TSPInstance* inst, const TSPSolution* sol){
	
	int i;
	double cost = 0;

	for(i = 0; i < inst->dimension; i++){
		int s = (*sol).path[i], t = (*sol).path[(i + 1) % inst->dimension];
        cost += getDist(s, t, inst);
	}/* for */

	return cost;

}/* getSolCost */

/*
* IP inst instance
* IP sol solution
* OR cost of the solution $sol
*/
double getSSolCost(const TSPInstance* inst, const TSPSSolution* sol){
	
	int i;
	double cost = 0;

	for(i = 0; i < inst->dimension; i++)
        cost += getDist(i, (*sol).succ[i], inst);

	return cost;

}/* getSSolCost */

/*
* IP inst instance
* OP sol solution to compute
*/
void ascendentSol(const TSPInstance* inst, TSPSolution* sol){
	
	int i;

	for(i = 0; i < inst->dimension; i++)
        (*sol).path[i] = i;

	(*sol).val = getSolCost(inst, sol);

}/* ascendentSol */

/*
* IOP inst instance to initialize $inst->dist
*/
void initDist(TSPInstance* inst){

    int i;

    (*inst).dist = malloc(((*inst).dimension - 1) * sizeof(double*));
    assert((*inst).dist != NULL);

    for (i = 0; i < (*inst).dimension - 1; i++){

        (*inst).dist[i] = malloc((i + 1) * sizeof(double));
        assert((*inst).dist[i] != NULL);

    }/* for */

}/* initDist */

/*
* IOP inst, compute and store into $inst->dist all the distances
*/
void computeDistances(TSPInstance* inst){
	
	int i, j;

	for(i = 1; i < (*inst).dimension; i++)
		for(j = 0; j < i; j++)
	        (*inst).dist[i - 1][j] = distance(&((*inst).points[j]), &((*inst).points[i]));

}/* computeDistances */

/*
* IP n number of nodes of the instance
* IP inst instance to initialize
*/
void allocInst(int n, TSPInstance* inst){

    (*inst).dimension = n;
    (*inst).points = malloc((*inst).dimension * sizeof(Point2D));
    
	assert((*inst).points != NULL);

	initDist(inst);

}/* allocInst */

/*
* IOP inst instance to free $inst->dist
*/
void freeDist(TSPInstance* inst){

    int i;

    if((*inst).dimension > 1){

        for (i = 0; i < (*inst).dimension - 1; i++)
            free((*inst).dist[i]);

        free((*inst).dist);

    }/* if */

}/* freeDist */

/*
* IOP inst instance to free memory
*/
void freeInst(TSPInstance* inst){
    free(inst->points);
	freeDist(inst);
}/* freeInst */

/*
* IP n number of nodes of the instance
* IP sol solution to initialize
*/
void allocSol(int n, TSPSolution* sol){

    sol->path = malloc(n * sizeof(int));
	assert(sol->path != NULL);

}/* allocSol */

/*
* IP n number of nodes of the instance
* IP ssol solution to initialize
*/
void allocSSol(int n, TSPSSolution* ssol){
	
	TSPSolution sol;

    allocSol(n, &sol);

	ssol->succ = sol.path;

}/* allocSSol */

/*
* IOP sol solution to free memory
*/
void freeSol(TSPSolution* sol){
    free(sol->path);
}/* freeSol */

/*
* IOP ssol solution to free memory
*/
void freeSSol(TSPSSolution* ssol){
	
	TSPSolution sol;

	sol.path = ssol->succ;
    
	freeSol(&sol);

}/* freeSSol */

/*
* IP inst instance to print
*/
void printInst(const TSPInstance* inst){
	
	int i;

    printf("Instance:\n");
    printf("\tName: %s\n", inst->name);
    printf("\tDimension: %d\n", inst->dimension);
    printf("\tNodes:\n");
    for(i = 0; i < inst->dimension; i++)
        if(i < (POINTS_TO_PRINT/2) || i >= (inst->dimension - POINTS_TO_PRINT/2))
            printf("\t\t%.3d %lf %lf\n", i + 1, inst->points[i].x, inst->points[i].y);
        else if ( i == (inst->dimension - POINTS_TO_PRINT/2 - 1))
            printf("\t\t\t...\n");
    printf("\n");

}/* printInst */

/*
* Print algorithm legend.
*/
void algorithmLegend(void){
    
    printf("Available algorithms:\n");
    printf("\t- Code: %d, Algorithm: Just a random solution\n", RANDOM);
    printf("\t- Code: %d, Algorithm: Nearest neighbor search\n", NEAREST_NEIGHBOR);
	printf("\t- Code: %d, Algorithm: CPLEX exact method\n", CPLEX);
	printf("\t- Code: %d, Algorithm: MATHEURISTIC method\n", MATHEURISTIC);
    printf("\n");
    
}/* algorithmLegend */

/*
* IP alg algorithm
* OR true if it is an exact method, false otherwise
*/
bool isExactMethod(ALGORITHM alg){
	return __END_HEURISTIC < alg && alg < __END_EXACTS;
}/* isExactMethod */

/*
* IP alg algorithm to run
* IP inst tsp instance
* IOP sol solution
* IP set settings
* OR true if an error occurred, false otherwise.
*/
bool offline_run(OFFLINE_ALGORITHM alg, const TSPInstance* inst, TSPSolution* sol, const Settings* set){
    
	int et; /* execution time in seconds */
    
    switch (alg){
	    case O_RANDOM:
	        et = randomSol(inst, sol);
	        break;
	    case O_NEAREST_NEIGHBOR_START_FIRST_NODE:
	        if((et = NNRunConfiguration(START_FIRST_NODE, set, inst, sol)) == -1)
				return true;
	        break;
		case O_NEAREST_NEIGHBOR_START_RANDOM_NODE:
	        if((et = NNRunConfiguration(START_RANDOM_NODE, set, inst, sol)) == -1)
				return true;
	        break;
		case O_NEAREST_NEIGHBOR_BEST_START:
	        if((et = NNRunConfiguration(BEST_START, set, inst, sol)) == -1)
				return true;
	        break;
	    default:
	        printf("Error: Algorithm code not found.\n\n");
	        return true;
    }/* switch */

    return false;

}/* offline_run */

/*
* IP alg algorithm to run
* IP inst tsp instance
* IOP sol solution
* IP set settings
* OR true if an error occurred, false otherwise.
*/
bool offline_run_refinement(OFFLINE_ALGORITHM alg, REFINEMENT_ALGORITHM ref, const TSPInstance* inst, TSPSolution* sol, const Settings* set){
    
	return offline_run(alg, inst, sol, set) || runRefAlg(ref, set, inst, sol);

}/* offline_run_refinement */

/*
* IP alg algorithm to run
* IP inst tsp instance
* IOP sol solution
* IP set settings
* OR error true if an error occurred, false otherwise.
*/
bool run(ALGORITHM alg, const TSPInstance* inst, TSPSolution* sol, const Settings* set){
    
	int et; /* execution time in seconds */

    switch (alg){
	    case RANDOM:
	        et = randomSol(inst, sol);
	        break;
	    case NEAREST_NEIGHBOR:
	        if((et = nearestNeighbor(set, inst, sol)) == -1)
				return true;
	        break;
		case CPLEX:
			return optimize(set, inst, sol);
		case MATHEURISTIC:
			if((et = matheur(set, inst, sol)) == -1)
				return true;
			/* Matheuristics takes all the tl, we cannot apply further refinement  */
			return false;
	    default:
	        printf("Error: Algorithm code not found.\n\n");
	        return true;
    }/* switch */

	if(et >= 0 && !isExactMethod(alg)){
		Settings s;
		cpSet(set, &s);
		s.tl = (*set).tl > et ? (*set).tl - et : 0;
		return runRefinement(&s, inst, sol);
	}/* if */

    return false;

}/* run */

/*
* IP first node index
* IP second node index
* IP inst tsp instance
* OR distance between nodes i and j
*/
double getDist(int i, int j, const TSPInstance* inst){

	if(i == j)
		return 0;
	
	return (i < j) ? (*inst).dist[j - 1][i] : (*inst).dist[i - 1][j];

}/* getDist */

/*
* Checks whether the solution is valid.
* IP inst tsp instance
* IP sol solution we want to check validity of
* OP valid true if valid solution, false otherwise.
*/
bool checkSol(const TSPInstance* inst, const TSPSolution* sol){

    if(!isDistinct(inst->dimension, sol->path))
        return false;

    return isEqual(sol->val, getSolCost(inst, sol));    

}/* checkSol */

/*
* IP inst tsp instance
* IP source solution we want to copy
* OP destination solution that will be a copy of $source
*/
void cpSol(const TSPInstance* inst, const TSPSolution* source, TSPSolution* destination){
	
	int i;

	for(i = 0; i < (*inst).dimension; i++)
		(*destination).path[i] = (*source).path[i];
	
	(*destination).val = (*source).val;

}/* cpSol */

/*
* IP inst tsp instance
* IP source solution we want to copy
* OP destination solution that will be a copy of $source
*/
void cpSSol(const TSPInstance* inst, const TSPSSolution* source, TSPSSolution* destination){
	
	int i;

	for(i = 0; i < (*inst).dimension; i++)
		(*destination).succ[i] = (*source).succ[i];
	
	(*destination).val = (*source).val;

}/* cpSSol */

/*
* Check that each element of the array appears once.
* IP n dimension of the array
* IP arr array
* OP distinct true if each element appears once, false otherwise.
*/
bool isDistinct(int n, int* arr){
    
	int i;
	bool status = true;

    int* counters = malloc(n * sizeof(int));
    assert(counters != NULL);

	for(i = 0; i < n; i++)
		counters[i] = 0;
	
	for(i = 0; i < n; i++)
		if(arr[i] < 0 || arr[i] >= n || ++counters[arr[i]] > 1){
			status = false;
			break;
		}/* if */

    free(counters);

    return status;

}/* isDistinct */

/*
* IP inst tsp instance
* IP temp current solution found
* IOP sol best solution 
* OR true if the incumbent is updated, false otherwise
*/
bool updateIncumbentSol(const TSPInstance* inst, const TSPSolution* temp, TSPSolution* sol){

    if((*temp).val < (*sol).val && checkSol(inst, temp)){
		cpSol(inst, temp, sol);
		return true;
	}/* if */
        
	return false;

} /* updateIncumbentSol */

/*
* IP in solution in successor representation
* OP out solution in path representation
* This method assumes $out is already allocated
*/
void convertSSol(const TSPInstance* inst, const TSPSSolution* in, TSPSolution* out){

	int curr = 0;
	
	(*out).val = (*in).val;

	for(int i = 0; i < (*inst).dimension; i++)
		(*out).path[i] = (curr = (*in).succ[curr]);

}/* convertSSol */
