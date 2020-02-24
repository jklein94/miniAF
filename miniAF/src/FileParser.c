/*
 * FileParser.c
 *
 *  Created on: 27.10.2019
 *      Author: jonas
 */

#include "FileParser.h"
#include <stdio.h>
#include  <stdlib.h>
#include "Vector.h"
#include <stdint.h>
#include <limits.h>


#define NUMBER_OF_CHARS 128

/* Parse arguments and attack relations from .apx file*/
bool parser_parseInputfile_apx(char * inputFilePath, vector* argumentNames,
		map_int_t * namesToIndex, vector * attacks) {

	bool first = true;

	FILE* inputFile = fopen(inputFilePath, "r");
	if (!inputFile) {
		printf("File not found!: %s\n", inputFilePath);
		return false;
	}

	char currentLine[NUMBER_OF_CHARS];

	int numberArguments = 0;
	while (fgets(currentLine, NUMBER_OF_CHARS, inputFile) != NULL) {

		if (strstr(currentLine, "arg")) {

			char *e;
			int indexOpenBracket;
			int indexClosedBracket;

			e = strchr(currentLine, '(');
			indexOpenBracket = (int) (e - currentLine);
			e = strchr(currentLine, ')');
			indexClosedBracket = (int) (e - currentLine);

			int nameLength = indexClosedBracket - indexOpenBracket - 1;

			char* argumentName = malloc(nameLength + 1);
			strncpy(argumentName, currentLine + indexOpenBracket + 1,
					nameLength);
			argumentName[nameLength] = '\0';

			map_set(namesToIndex, argumentName, numberArguments);
			vector_add(argumentNames, argumentName);
			numberArguments++;

		}

		if (numberArguments < 1) {
			printf("Error while parsing file: No arguments found!");
			exit(-1);
		}

		if (strcmp(currentLine, "\n") != 0 && strcmp(currentLine, "\r\n") != 0
				&& strcmp(currentLine, "\0") != 0 && 1) {

			if (strstr(currentLine, "att")) {
				if (first == 1) {
					vector_resize(attacks, numberArguments * 2);
					for (int i = 0; i < attacks->capacity; i++) {
						vector* attack = malloc(sizeof(vector));
						vector_init(attack);
						vector_add(attacks, attack);

					}
					first = false;
				}

				char *e;
				int indexOpenBracket;
				int indexClosedBracket;

				e = strchr(currentLine, '(');
				indexOpenBracket = (int) (e - currentLine);
				e = strchr(currentLine, ')');
				indexClosedBracket = (int) (e - currentLine);

				int nameLength = indexClosedBracket - indexOpenBracket - 1;

				char* argumentName = malloc(nameLength + 1);
				strncpy(argumentName, currentLine + indexOpenBracket + 1,
						nameLength);
				argumentName[nameLength] = '\0';

				char * getAttacked = NULL;
				char * attacker = strtok_r(argumentName, ",", &getAttacked);

				int indexAttacker = *((int *) map_get(namesToIndex, attacker));
				int indexGetAttacked = *((int *) map_get(namesToIndex,
						getAttacked));

				VECTOR_ADD(*((vector * )attacks->items[indexAttacker]),
						indexGetAttacked);
				VECTOR_ADD(
						*((vector * )attacks->items[indexGetAttacked
								+ numberArguments]), indexAttacker);

				 free(argumentName);
			}
		}
	}
	fclose(inputFile);

	return true;
}

/* Parse arguments and attack relations from .tgf file*/
bool parser_parseInputfile_tgf(char * inputFilePath, vector* argumentNames,
		map_int_t * namesToIndex, vector * attacks) {

	bool first = true;

	FILE* inputFile = fopen(inputFilePath, "r");
	if (!inputFile) {
		printf("File not found!: %s\n", inputFilePath);
		return false;
	}

	char currentLine[NUMBER_OF_CHARS];
	int isAttackSection = 0;

	int numberArguments = 0;
	while (fgets(currentLine, NUMBER_OF_CHARS, inputFile) != NULL) {

		if (strstr(currentLine, "#")) {	// # marks the beginning of th attack section in a tgf file
			isAttackSection = 1;
			continue;
		}

		if (isAttackSection == 0) {
			if (strcmp(currentLine, "\n") != 0
					&& strcmp(currentLine, "\r\n") != 0
					&& strcmp(currentLine, "\0") != 0 && 1) {
				size_t nameLength = strlen(currentLine);

				if (strcmp(&currentLine[nameLength - 2], "\r\n") == 0) {
					currentLine[nameLength - 2] = '\0'; // truncate string to get rid of \r\n at the end of the line
				} else if (strcmp(&currentLine[nameLength - 1], "\n") == 0) {
					currentLine[nameLength - 1] = '\0'; // truncate string to get rid of \n at the end of the line
				}

				char* argumentName = malloc(nameLength + 1);
				strncpy(argumentName, currentLine, nameLength);
				//argumentName[nameLength] = '\0';

				map_set(namesToIndex, argumentName, numberArguments);
				vector_add(argumentNames, argumentName);
				numberArguments++;
			}

		} else {
			if (numberArguments < 1) {
				printf("Error while parsing file: No arguments found!");
				exit(-1);
			}
			if (strcmp(currentLine, "\n") != 0
					&& strcmp(currentLine, "\r\n") != 0
					&& strcmp(currentLine, "\0") != 0 && 1) {
				if (first == 1) {
					vector_resize(attacks, numberArguments * 2);
					for (int i = 0; i < attacks->capacity; i++) {
						vector* attack = malloc(sizeof(vector));
						vector_init(attack);
						vector_add(attacks, attack);

					}
					first = true;
				}
				size_t nameLength = strlen(currentLine);

				if (strcmp(&currentLine[nameLength - 2], "\r\n") == 0) {
					currentLine[nameLength - 2] = '\0'; // truncate string to get rid of \n at the end of the line
				} else if (strcmp(&currentLine[nameLength - 1], "\n") == 0) {
					currentLine[nameLength - 1] = '\0'; // truncate string to get rid of \n at the end of the line
				}

				char* argumentName = malloc(nameLength + 1);
				strncpy(argumentName, currentLine, nameLength);
				//argumentName[nameLength] = '\0';

				char * next = NULL;
				char * attacker = strtok_r(currentLine, " ", &next);
				char* getAttacked = strtok_r(NULL, " ", &next);

				int indexAttacker = *((int *) map_get(namesToIndex, attacker));
				int indexGetAttacked = *((int *) map_get(namesToIndex,
						getAttacked));

				VECTOR_ADD(*((vector * )attacks->items[indexAttacker]),
						indexGetAttacked);
				VECTOR_ADD(
						*((vector * )attacks->items[indexGetAttacked
								+ numberArguments]), indexAttacker);

			}
		}
	}
	fclose(inputFile);
	return true;
}


bool parser_parseInputfile(struct  AF * current_framework,struct RunProperties * current_RunProperties) {

	char * inputFilePath = current_RunProperties->input_filePath;
	char * fileFormat = current_RunProperties->file_format;
	vector* argument_names = &current_framework->argument_names;
	map_int_t * names_to_index = &current_framework->names_to_index;

	vector * attacks_relations = &current_framework->attacks_relations;

	if (strcmp(fileFormat, "tgf") == 0) {

		return parser_parseInputfile_tgf(inputFilePath, argument_names,
				names_to_index, attacks_relations);
	} else {
		return parser_parseInputfile_apx(inputFilePath, argument_names,
				names_to_index, attacks_relations);
	}
}

/* Create header for solver input in DIMACS format*/
char * parser_header_DIMACS(size_t number_arguments,size_t number_clauses){
	    char *  header= malloc(38);
		snprintf(header, 38, "p cnf %zu %\zu", number_arguments * 3,number_clauses);
		return header;
}

/* Parse single clause to DIMACS format*/
char* parser_single_clause_DIMACS(vector * clause,size_t * clause_length){

	int initialLength = 100;
		char * str_clause = malloc(initialLength);

		char * varAsString = malloc(12);
		int string_idx = 0;


	for (int j = 0; j < clause->total; j++) {
				snprintf(varAsString, 12, "%d", clause->items[j]);

				if (string_idx + strlen(varAsString) + 4 > initialLength) {
					initialLength += string_idx + strlen(varAsString) + 4;
					str_clause = realloc(str_clause, initialLength);
				}

				strcpy(&str_clause[string_idx], varAsString);
				string_idx += strlen(varAsString);
				str_clause[string_idx++] = ' ';

			}

			str_clause[string_idx++] = '0';
			str_clause[string_idx++] = '\n';
		    str_clause[string_idx] = '\0';

		    *clause_length = string_idx;

		    free(varAsString);
		    return str_clause;
}

/* Parse multiple clauses to DIMACS foramt*/
char * parser_multipleClauses_DIMACS(vector * formula,size_t * clause_length){

	int initialLength = 100;

	char* output = malloc(initialLength);

	char * varAsString = malloc(12);
	int string_idx = 0;

	vector * lastvector = formula->items[formula->total-1];

	for (int i = 0; i < formula->total; i++) {
		vector * temp = formula->items[i];
		for (int j = 0; j < temp->total; j++) {

			snprintf(varAsString, 12, "%d", temp->items[j]);

			if (string_idx + strlen(varAsString) + 5 > initialLength) {
				initialLength += string_idx + strlen(varAsString) + 5;
				output = realloc(output, initialLength);
			}

			strcpy(&output[string_idx], varAsString);
			string_idx += strlen(varAsString);
			output[string_idx++] = ' ';

		}

		output[string_idx++] = '0';
		output[string_idx++] = '\n';

	}
	output[string_idx] = '\0';

	*clause_length = string_idx;

	free(varAsString);


	return output;

}




