
#ifndef SRC_FILEPARSER_H_
#define SRC_FILEPARSER_H_

#include "map.h"
#include "Vector.h"
#include "Util.h"

bool parser_parseInputfile_apx(char * inputFilePath, vector* argumentNames, map_int_t * namesToIndex, vector * attacks);
bool parser_parseInputfile_tgf(char * inputFilePath, vector* argumentNames, map_int_t * namesToIndex, vector * attacks);
bool parser_parseInputfile(struct AF * current_framework, struct RunProperties * current_RunProperties);


char * parser_header_DIMACS(size_t numberArguments,size_t numberClauses);
char * parser_single_clause_DIMACS(vector * clause,size_t * clause_length);
char * parser_multipleClauses_DIMACS(vector * formula,size_t * clause_length);

#endif /* SRC_FILEPARSER_H_ */
