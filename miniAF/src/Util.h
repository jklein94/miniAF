

#ifndef UTIL_H_
#define UTIL_H_

#include "Vector.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <fcntl.h>
#include "map.h"
#include "Vector.h"
#include "FileParser.h"

struct RunProperties {
char * input_filePath;
char * sat_path ;
char * sat_parameter;
char * file_format ;
char * single_argument;
char * problem ;
char * semantics ;
char * track;
};

struct AF  {
	map_int_t names_to_index;
	vector argument_names;
	vector attacks_relations;
	vector complete_constraints;
	vector result_extensions;

};

struct SAT_Input {
	char * header;
	char * body;
	size_t header_length;
	size_t body_length;
	size_t clauses_number;
	size_t variables_number;
};



static inline int util_arg_in(int number_argument) {
	return number_argument + 1;
}
static inline int util_arg_out(int number_argument, int total) {
	return util_arg_in(number_argument) + total;
}

static inline int util_arg_undec(int number_argument, int total) {
	return util_arg_in(number_argument) + 2 * total;
}

static inline int util_arg_notIn(int number_argument) {
	return (number_argument + 1) * -1;
}

static inline int util_arg_notOut(int number_argument, int total) {
	return util_arg_out(number_argument, total) * -1;
}

static inline int util_arg_notUndec(int number_argument, int total) {
	return util_arg_undec(number_argument, total) * -1;
}


/* Initialize SAT solver input */
void util_initInput(struct SAT_Input * input,vector * complete_constaints,size_t variables_number);

/* Append clause to SAT solver input and update header*/
void util_updateInput(struct SAT_Input * input,vector * clause, bool multiple_clauses);

/* Concatenate head and body*/
char * util_getCombinedInput(struct SAT_Input * input);

/* Update header with new number of clauses*/
void util_updateHeader(struct SAT_Input * input);


/* Concatenate strings via memcpy, with known length */
char  * util_concatStrings(char* head, char* body,size_t header_length,size_t body_length);

/* Append string "body" to existing string "head"*/
void util_appendString(char* head, char* body);


/* Print extensions given as vector*/
void util_printAllExtension(vector * extensions, vector * argument_names);

/* Print single extension given as vector */
void util_printSingleExtension(vector * extension,vector* argument_names,bool new_line);

/*Print result for DS and DC*/
void util_printDecision(bool status);


/*Parse parameters from command line*/
int util_parseParametersFromCMD(int argc, char* argv[],struct RunProperties * current_run_probs);

/* Call external SAT solver and return true if the given formula is satisfiable.*/
bool util_callExternalSatSolver(char * cnf_dimacs,int * sat_model, struct RunProperties *  run_props);

/*Extract model from SAT solver output*/
bool util_extractModel(FILE * solver_output, int * sat_model);

#endif /* UTIL_H_ */
