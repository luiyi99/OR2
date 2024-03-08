/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : output.c
*/

#include <stdio.h>

#include "output.h"

/*
* IP gnuplotPipe pipe pointer
*/
void plotSettings(FILE* gnuplotPipe){

    fprintf(gnuplotPipe, "set title 'Hamiltonian Path'\n");
    fprintf(gnuplotPipe, "set xlabel 'X'\n");
    fprintf(gnuplotPipe, "set ylabel 'Y'\n");
    fprintf(gnuplotPipe, "plot '-' with linespoints pointtype 7 pointsize 2 linewidth 2 notitle\n");

}/* plotSettings */

/*
* IP gnuplotPipe pipe pointer
* IP inst tsp instance
* IP sol solution to bw plotted
*/
void plotNodes(FILE* gnuplotPipe, const TSPInstance* inst, const TSPSolution* sol){

    Point2D* p;

    for(int i = 0; i < inst->dimension; i++){
        p = &(inst->points[sol->succ[i]]);
        fprintf(gnuplotPipe, "%f %f\n", p->x, p->y);
    }
    
    p = &(inst->points[sol->succ[0]]);

    fprintf(gnuplotPipe, "%f %f\n", p->x, p->y); /* connect last to first */

    fprintf(gnuplotPipe, "e\n");

}/* plotNodes */

/*
* IP sol solution to plot
* OV hamiltonian path
*/
void plotSolution(const TSPSolution* sol, const TSPInstance* inst){
    
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
	
    if (gnuplotPipe == NULL) {
        fprintf(stderr, "Error opening Gnuplot pipe\n");
        return;
    }

    plotSettings(gnuplotPipe);

    plotNodes(gnuplotPipe, inst, sol);

    pclose(gnuplotPipe);

}/* plotSolution */
