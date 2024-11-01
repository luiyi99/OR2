/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : settings.h
*/

#include <stdbool.h>

#pragma once /* Allows to do not recreate these object on further includes */
#define MAX_FILE_NAME_SIZE 41

typedef struct{

    char input_file_name[MAX_FILE_NAME_SIZE];
    int n, seed;
	double tl;
    bool v;
    /* n    := number of nodes for the random instance */
    /* tl   := execution time limit (in seconds) */
    /* seed := seed used for random generation */
    /* v    := verbosity, true or false */
	
} Settings;

typedef enum {
    ERROR,
    HELP,
    INPUT_FILE,
    RANDOM_GENERATION
} CONF;

void printSettings(const Settings*);

CONF parseCMDLine(int, char* const*, Settings*);

void cpSet(const Settings*, Settings*);
