#include "Labels.h"
#include "map.h"
#include <stdlib.h>


int labels_init(Labels * labels)
{
	labels->inLabels = malloc(sizeof(vector));
	labels->outLabels = malloc(sizeof(vector));
	labels->undecLabels = malloc(sizeof(vector));
	labels->allLabels = malloc(sizeof(map_int_t));
	vector_init(labels->inLabels);
	vector_init(labels->outLabels);
	vector_init(labels->undecLabels);
	map_init(labels->allLabels);
	return 0;
}

bool labels_isEmpty(Labels * labels)
{

	return isEmpty(labels->inLabels) && isEmpty(labels->outLabels) && isEmpty(labels->undecLabels);
}

bool isEmpty(vector * type)
{
	if (type->total <= 0) return true;
	return false;
}

void labels_setInLabel(Labels * labels, char * argumentName)
{
	vector_add(labels->inLabels, argumentName);
	map_set(labels->allLabels, argumentName, INARG);
	
}

void labels_setOutLabel(Labels * labels, char * argumentName)
{
	vector_add(labels->outLabels, argumentName);
	map_set(labels->allLabels, argumentName, OUTARG);
	
}

void labels_setUndecLabel(Labels * labels, char * argumentName)
{
	vector_add(labels->undecLabels, argumentName);
	map_set(labels->allLabels, argumentName, UNDECARG);
	
}

int labels_getLabelOfArgument(Labels * label, char * argumentName)
{
	return *(map_get(label->allLabels,argumentName));
}


void labels_assignLabelsFromSatModel(Labels * labels,int * satModel, vector * argumentNames)
{

	int totalNumberOfArguments = argumentNames->total;

	for (int i = 0; i <totalNumberOfArguments;i++) {
		if (satModel[i] > 0) {
			labels_setInLabel(labels, vector_get(argumentNames, i));

			continue;
		}
		if (satModel[i + totalNumberOfArguments] > 0) {
			labels_setOutLabel(labels, vector_get(argumentNames, i));

			continue;
		}
		if (satModel[i + 2 * totalNumberOfArguments] > 0) {
			labels_setUndecLabel(labels, vector_get(argumentNames,i));

			continue;
		}
	}
}

void labels_assignLabelsFromSatModel_e(Labels * labels,int * satModel, vector * argumentNames, vector * negation)
{

	int totalNumberOfArguments = argumentNames->total;

	for (int i = 0; i <totalNumberOfArguments;i++) {
		if (satModel[i] > 0) {
			labels_setInLabel(labels, vector_get(argumentNames, i));
			int value = satModel[i];
			value *= -1;
			VECTOR_ADD(*negation, value);
			continue;
		}
		if (satModel[i + totalNumberOfArguments] > 0) {
			labels_setOutLabel(labels, vector_get(argumentNames, i));
			int value = satModel[i+totalNumberOfArguments];
						value *= -1;
			VECTOR_ADD(*negation,value);
			continue;
		}
		if (satModel[i + 2 * totalNumberOfArguments] > 0) {
			labels_setUndecLabel(labels, vector_get(argumentNames,i));
			int value = satModel[i+ 2 * totalNumberOfArguments];
						value *= -1;
			VECTOR_ADD(*negation,value);
			continue;
		}
	}
}

Labels * labels_clone(Labels * labels)

{
	Labels* clonedLabels = malloc(sizeof(Labels));
	labels_init(clonedLabels);

	for (int i = 0; i < labels->inLabels->total;i++) {
		char * srcArg = labels->inLabels->items[i];
		char * destArg = malloc(sizeof(srcArg)+1);
		memcpy(destArg, srcArg, sizeof(srcArg));
		destArg[sizeof(srcArg)] = '\0';
		labels_setInLabel(clonedLabels, destArg);
	}
	for (int i = 0; i < labels->outLabels->total;i++) {
		char * srcArg = labels->outLabels->items[i];
		char * destArg = malloc(sizeof(srcArg));
		memcpy(destArg, srcArg, sizeof(srcArg));
		labels_setOutLabel(clonedLabels, destArg);
	}
	for (int i = 0; i < labels->undecLabels->total;i++) {
		char * srcArg = labels->undecLabels->items[i];
		char * destArg = malloc(sizeof(srcArg));
		memcpy(destArg, srcArg, sizeof(srcArg));
		labels_setUndecLabel(clonedLabels, destArg);
	}

	return clonedLabels;
}

void labels_print(Labels * labels)
{
	for (int i = 0;i < labels->inLabels->total;i++) {
		printf("%s",(char *)labels->inLabels->items[i]);
		printf(" ");
	}
	printf("\n");
}

void printlabelsVector(vector * labels)
{

	printf("[");
	for(int i = 0;i < labels->total;i++) {
		Labels * currLabels =(Labels *) (labels->items[i]);
		char * temp = labels_inLabelsToString(currLabels);
		printf("%s",temp);
		if (i < labels->total - 1) {
			printf(",");
		}
		free(temp);
	}
	printf("]\n");

}

char* labels_inLabelsToString(Labels * labels)
{
	int initialLength = 100;
	int numberOfArguments = labels->inLabels->total;
	char* output = malloc(initialLength);
	int sidx = 0;
	output[sidx++] = '[';
	int isFirst = 1;
	for (int i = 0; i < numberOfArguments; i++) {
	
		if (sidx + strlen(labels->inLabels->items[i]) + 4 > initialLength) {
			initialLength += sidx + strlen(labels->inLabels->items[i]) + 4;
			output = realloc(output, initialLength);
		}
		if (isFirst != 0) {
			strcpy(&output[sidx],labels->inLabels->items[i]);
			sidx += strlen(labels->inLabels->items[i]);
			isFirst = 0;
		}
		else {
			output[sidx++] = ',';
			strcpy(&output[sidx],labels->inLabels->items[i]);
			sidx += strlen(labels->inLabels->items[i]);
		}
	}
	output[sidx++] = ']';
	output[sidx] = '\0';
	return output;
}


void labels_destroy(Labels * labels)
{
	vector_free(labels->inLabels);
	vector_free(labels->outLabels);
	vector_free(labels->undecLabels);
	map_deinit(labels->allLabels);

}


