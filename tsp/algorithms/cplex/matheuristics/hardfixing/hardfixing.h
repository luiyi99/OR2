/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : hardfixing.h
*/

#include "../../../../tsp.h"
#include "../../../../input/settings/settings.h"

int hard_fixing(const Settings*, const TSPInstance*, double, CPXENVptr, CPXLPptr, TSPSolution*, double*);
