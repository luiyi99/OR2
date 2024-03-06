/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : utility.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utility.h"

#define EPSILON 1e-9

/*
* Print a boolean.
* IP lab label for the output
* IP b boolean to print
* OV print $b
*/
void printBool(const char lab[], bool b) {
    printf("%s", lab);
    if (b)
        printf("true");
    else
        printf("false");
    printf("\n");
} /* printBool */

/*
* IP n
* OR random integer value in [0, $n - 1]
* Note: rand() returns an integer value in the range [0, RAND_MAX - 1]
*/
int rand0N(int n){

    return rand() % n;

}/* rand0N */

/*
* IP a pointer to index of a node
* IP b pointer to index of a node
*/
void swapInt(int* a, int* b){
    
	int temp;

    temp = *a;
    *a = *b;
    *b = temp;

}/* swapInt */

/*
* Read integer in input.
* IP lab label for the output
* OR integer passed in input
*/
int readInt(const char lab[]){
    
	int num;

    printf("%s", lab);
    scanf("%d", &num);
    printf("\n\n");

    return num;

}/* readInt */

/*
* IP start starting value of the range
* IP end end value of the range
* IP lab label for the output
* OR integer value in [$start, $end]
*/
int readIntRange(int start, int end, const char lab[]){
	
	int val;

	printf("Please insert a value in the range [%d, %d]\n", start, end);

	do{
        val = readInt(lab);

        if(val < start || val > end)
            printf("Invalid value, please insert a value in the range [%d, %d]\n", start, end);

    }while(val < start || val > end);

	return val;

}/* readIntRange */

/*
* IP a double number
* IP b double number to check equality with $a
* IP epsilon precision of equality
* OR true if $a dists less then $epsilon to $b, false otherwise
*/
bool isEqualPrecision(double a, double b, double epsilon){
    
    return fabs(a - b) < epsilon;

}/* isEqualPrecision */

/*
* IP a double number
* IP b double number to check equality with $a
* OR true if $a dists less then EPSILON to $b, false otherwise
*/
bool isEqual(double a, double b){
    
    return isEqualPrecision(a, b, EPSILON);

}/* isEqual */

/*
* IP start starting execution time
* IP end ending execution time
* OR execution time in seconds
*/
int getSeconds(clock_t start, clock_t end){

    return (end - start) / CLOCKS_PER_SEC;

}/* getSeconds */
