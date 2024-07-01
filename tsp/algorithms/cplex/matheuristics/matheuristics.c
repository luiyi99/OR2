/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : metaheuristics.c
*/

#include <stdio.h>
#include <assert.h>
#include "../cplex.h"
#include "matheuristics.h"
#include "../usercut/usercut.h"
#include "../../../utility/utility.h"

#define MIP_TIMELIMIT_FRACTION 10.

/*
* Print algorithm configurations.
*/
void matheuristics_legend(void){
    
    printf("Available MATHEURISTICS:\n");
    printf("\t- Code: %d, Algorithm: Hard fixing\n", HARD_FIXING);
    printf("\t- Code: %d, Algorithm: Local branching\n", LOCAL_BRANCHING);
    printf("\n");
    
}/* algorithmLegend */

/*
 * IP set settings
 * IP start execution time
 * OP mipset settings to update timelimit
 * OP env CPLEX environment
 * OP lp CPLEX linear program
 */
void update_solver_time_limit(const Settings* set, clock_t start, Settings* mipset, CPXENVptr env, CPXLPptr lp)
{
	double /* remining time */ rt = step((*set).tl - getSeconds(start));

	(*mipset).tl = min_dbl((*set).tl / MIP_TIMELIMIT_FRACTION, rt);

	if(setdblparam(CPX_PARAM_TILIM, (*mipset).tl, env, lp))
		exit(1);

} /* update_time_limit */

/*
* IP inst tsp instance
* IP sol solution
* OP fe the edges fixed (index of the CPLEX variable position), array assumed to be already allocated
*/
void convertSolIndicesIntoCPXXPos(const TSPInstance* inst, const TSPSolution* sol, ArrayDinaInt* fe){

	int i;

	for (i = 0; i < (*fe).n; i++){
		int si = fe->v[i];
		int a = (*sol).path[si], b = (*sol).path[(si + 1) % (*inst).dimension];
		fe->v[i] = xpos(a, b, inst);
	}/* for */

}/* convertSolIndicesIntoCPXXPos */

/*
* IP set settings
* IP inst tsp instance
* IP sol current best tsp solution
* IP lb lower bound array
* OP env CPLEX environment
* OP lp CPLEX linear program
* OP fe the edges fixed (index of the CPLEX variable position), array assumed to be already allocated
*/
void fix_edges(const Settings* set, const TSPInstance* inst, const TSPSolution* sol, const char* lbc, double *lb[2], CPXENVptr env, CPXLPptr lp, ArrayDinaInt* fe){

	ArrayDinaInt soledges;
	
	soledges.n = (*inst).dimension;
	soledges.v = (*sol).path;

	// if((*set).v)
	// 	printf("Fixing %d edges\n", fe->n);

	reservoirSamplingIndices(&soledges, fe);

	convertSolIndicesIntoCPXXPos(inst, sol, fe);

	CPXchgbds(env, lp, fe->n, fe->v, lbc, lb[1]);

	// if((*set).v)
	// 	printf("%d edges fixed\n", fe->n);

}/* fix_edges */

/*
* IP inst tsp instance
* OP lb lower bound array to be allocated and initialized
*/
void initLB(const TSPInstance* inst, char** lbc, double *lb[2]){

	int i;

	*lbc = malloc(inst->dimension * sizeof(char));
	assert(*lbc != NULL);

	lb[0] = malloc(inst->dimension * sizeof(double));
	assert(lb[0] != NULL);

	lb[1] = malloc(inst->dimension * sizeof(double));
	assert(lb[1] != NULL);

	for(i = 0; i < inst->dimension; i++){
		(*lbc)[i] = 'L';
		lb[0][i] = 0.;
		lb[1][i] = 1.;
	}/* for */

}/* initLB */

/*
* IP set settings
* IP inst tsp instance
* IP lb lower bound
* OP sol solution
* IP env CPLEX environment
* IP lp CPLEX linear program
* OP et execution time in seconds
* OR error code
*/
int hard_fixing(const Settings* set, const TSPInstance* inst, CPXENVptr env, CPXLPptr lp, TSPSolution* sol, bool mipstart, double* et){
	
	int err;
	char *lbc;
	double mipet, *lb[2], ls = -1;
	ArrayDinaInt fe;
	Settings mipset;
	TSPSolution temp;
	clock_t start = clock();

	if((err = offline_run_refinement(O_NEAREST_NEIGHBOR_BEST_START, OPT2, inst, sol, set))){
		if((*set).v)
			printf("Error while computing the heuristic solution.");
		return err;
	}/* if */

	if((*set).v)
		printf("\n\nInitial cost: %lf\n", sol->val);
	
	cpSet(set, &mipset);
	mipset.v = 0;

	initArrayDinaInt((*inst).dimension, &fe);
	allocSol((*inst).dimension, &temp);
	
	initLB(inst, &lbc, lb);

	fe.n = ((double)(*inst).dimension) * .8; /* we can make it dinamic by analizing wheater the mip solver does not find new good solutions or it exeedes the tl */

	while (true){
		
		update_solver_time_limit(set, start, &mipset, env, lp);

		fix_edges(set, inst, sol, lbc, lb, env, lp, &fe);
		
		/* TO CHECK THAT IT IS THE BEST METHOD by doing perfprof */
		if(callback_solver(&mipset, inst, env, lp, (callback_installer)usercut, &temp, mipstart, &mipet)){
			if((*set).v)
				printf("Error while calling the solver.\nReturning best found solution so far.");
			break;
		}/* if */

		if(updateIncumbentSol(inst, &temp, sol))
			if((*set).v)
				printf("New cost: %lf\n", sol->val);

		// if(mipet > (mipset.tl)); /* we reached the timelimit */
		// update fe.n

		if(checkTimeLimitV(set, start, false, &ls))
			break;

		/* Free edges */
		CPXchgbds(env, lp, fe.n, fe.v, lbc, lb[0]);

	}/* while */

	free(lb[1]);
	free(lb[0]);
	free(lbc);
	freeSol(&temp);
	freeArrayDinaInt(&fe);

	*et = getSeconds(start);
	
	return 0;

}/* hard_fixing */

/*
* IP alg matheuristic algorithm to run
* IP set settings
* IP inst tsp instance
* OP sol solution
* OR int execution seconds, -1 if error
*/
double runMatheurConfiguration(MATHEURISTICS alg, const Settings* set, const TSPInstance* inst, TSPSolution* sol){

	int err;
	double et;

	switch (alg){
	    case HARD_FIXING:
			if((err = optimize_offline(set, inst, true, _HARD_FIXING, sol, &et)))
				return -1;
			return et;
	    case LOCAL_BRANCHING:
	        // return NN_solver(rand0N((*inst).dimension), inst, sol);
	        break;
	    default:
	        printf("Error: Algorithm code not found.\n\n");
	        break;
    }/* switch */
	
	return -1;

}/* runConfiguration */

/*
* IP set settings
* IP inst tsp instance
* IOP sol solution
* OP false if found a valid solution, true otherwise.
* OR int execution seconds, -1 if error
*/
int matheur(const Settings* set, const TSPInstance* inst, TSPSolution* sol){

	matheuristics_legend();

	return runMatheurConfiguration(readInt("Insert the configuration code you want to run: "), set, inst, sol);

}/* matheur */