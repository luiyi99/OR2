/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : localbranching.c
*/

#include "../../cplex.h"
#include "localbranching.h"

/*
* IP set settings
* IP inst tsp instance
* IP env CPLEX environment
* IP lp CPLEX linear program
* OP sol solution
* OP et execution time in seconds
* OR 0 if no error, error code otherwise
*/
int local_branching(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp, TSPSolution* sol, double* et){

	int err;

	if((*set).v)
		printf("Running heuristics and refinement:\n\n");

	if((err = offline_run_refinement(O_NEAREST_NEIGHBOR_BEST_START, OPT2, inst, sol, set))){
		if((*set).v)
			printf("Error while computing the heuristic solution.");
		return err;
	}/* if */

	if((*set).v)
		printf("\n\nInitial cost: %lf\n\n", sol->val);

	return 0;

}/* local_branching */
