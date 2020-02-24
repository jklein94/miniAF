#ifndef LABELS_H
#define LABELS_H
#include "Vector.h"
#include <stdbool.h>
#include "map.h"
enum{INARG = 0,OUTARG,UNDECARG};

typedef struct {
	vector* inLabels;
	vector * outLabels;
	vector * undecLabels;
	map_int_t * allLabels;
} Labels;
/*Initialize empty labels*/
int labels_init(Labels * labels);

bool labels_isEmpty(Labels * labels);
bool isEmpty(vector* type);

/*Adds argument to the vector inLabels*/
void labels_setInLabel(Labels * labels, char * argumentName);

/*Adds argument to the vector outLabels */
void labels_setOutLabel(Labels * labels, char * argumentName);

void labels_setUndecLabel(Labels * labels, char * argumentName);

/* Returns the label(IN,OUT,UNDEC) of an argument, returns NULL when argument is not found*/
int labels_getLabelOfArgument(Labels * label, char * argumentName);

/*Assigns the correspondig labelling to the variables of a model */
void labels_assignLabelsFromSatModel_e(Labels * labels, int * satModel, vector * argumentNames, vector * negations);

void labels_assignLabelsFromSatModel(Labels * labels, int * satModel, vector * argumentNames);

Labels * labels_clone(Labels * labels);

void labels_print(Labels * labels);

void printlabelsVector(vector * labels);

/**/
char*  labels_inLabelsToString(Labels * labels);

/*Free memory occupied by labels*/
void labels_destroy(Labels * labels);

#endif // !LABELS_H

