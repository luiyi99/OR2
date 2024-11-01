/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : random.c
*/

#include "random.h"
#include "../../utility/utility.h"

/*
* IP inst tsp instance to solve
* IOP sol random solution
* OR int execution seconds 
*/
int randomSol(const TSPInstance* inst, TSPSolution* sol){
	
	time_t start = time(0);
	int i;

	ascendentSol(inst, sol);

	for(i = 0; i < (*inst).dimension - 1; i++){
		
		int nn = i + rand0N((*inst).dimension - i);

		swapInt(&((*sol).path[i]), &((*sol).path[nn]));

	}/* for */

	(*sol).val = getSolCost(inst, sol);

	return getSeconds(start);

}/* random */
