/*
 * Util.c
 *
 *  Created on: 20.01.2020
 *      Author: jonas
 */
#define _GNU_SOURCE
#include "Util.h"
#include  <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>


void util_updateHeader(struct SAT_Input * input){
	 input->header= malloc(38);
	snprintf(input->header, 38, "p cnf %zu %zu\n",input->variables_number* 3,input->clauses_number);
}

void util_initInput(struct SAT_Input * input,vector * complete_constraints,size_t variables_number){

	input->clauses_number = complete_constraints->total;
	input->variables_number = variables_number;
	util_updateHeader(input);
	input->header_length = strlen(input->header);
	size_t body_length  = 0;
	char * temp_body = parser_multipleClauses_DIMACS(complete_constraints, &body_length);
	input->body = malloc(body_length+1);
	memcpy(input->body, temp_body,body_length + 1);
	input->body_length = body_length;

}

char* util_getCombinedInput(struct SAT_Input *input){
	return util_concatStrings(input->header, input->body,input->header_length,input->body_length);
}

void util_updateInput(struct SAT_Input * input,vector * clause,bool multiple_clauses){

	size_t * new_clause_length = malloc(sizeof(size_t));
	char * str_clause = NULL;

	if(multiple_clauses){
		input->clauses_number = input->clauses_number + clause->total;
		str_clause = parser_multipleClauses_DIMACS(clause,new_clause_length);

	}
	else{
		input->clauses_number = input->clauses_number + 1;
		str_clause = parser_single_clause_DIMACS(clause,new_clause_length);
	}

    input->body  = realloc(input->body,input->body_length + *new_clause_length + 1);
	memcpy(input->body + input->body_length, str_clause,*new_clause_length + 1);
	input->body_length += *new_clause_length;

	util_updateHeader(input);
	input->header_length = strlen(input->header);

	free(new_clause_length);
	free(str_clause);
}


/* Prints all extensions given as a vector */
void util_printAllExtension(vector * extensions, vector * argument_names) {
	if (extensions->total < 1) {
		printf("[]\n");
		return;
	}

	printf("[");

	for (int i = 0; i < extensions->total; i++) {
		vector * current_extension = extensions->items[i];
		if (current_extension->total < 1)
			printf("[]");
		else {

			printf("[");
			for (int j = 0; j < current_extension->total; ++j) {

				int indexCurrentArgument =
						(intptr_t) current_extension->items[j];
				printf("%s",
						(char*) argument_names->items[indexCurrentArgument]);

				if (j < current_extension->total - 1)
					printf(",");
				else
					printf("]");
			}
		}

		if (i < extensions->total - 1) {
			printf(",");
		}
	}

	printf("]\n");

}

/* Prints a single extension given as a vector*/
void util_printSingleExtension(vector * extension, vector* argument_names,bool new_line) {
	if (extension->total < 1)
		printf("[]\n");

	else {

		printf("[");

		for (int i = 0; i < extension->total; i++) {

			int indexCurrentArgument = (intptr_t) extension->items[i];
			printf("%s", (char*) argument_names->items[indexCurrentArgument]);

			if (i < extension->total - 1)
				printf(",");
		}
		if (new_line)
			printf("]\n");
		else
			printf("]");
	}
}

void util_printDecision(bool status) {
	if (status) {
		printf("YES\n");
	} else {
		printf("NO\n");
	}

}


/* Concatenate string with known length an return address to new string */
char * util_concatStrings(char* head, char* body,size_t header_length,size_t body_length){
	char * res = malloc(header_length + body_length + 1); // +1 for the null-terminator
		memcpy(res, head, header_length);
		memcpy(res + header_length, body, body_length + 1);
		return res;
}

/* Append string to existing one*/
void util_appendString(char* head, char * body){
	head = realloc(head, strlen(head) + strlen(body) + 1);
	strcat(head, body);
}

/* Call external SAT solver and extract output model*/
bool util_callExternalSatSolver(char* cnf_dimacs,int * sat_model,struct RunProperties * run_props){

	 char * sat_path = run_props->sat_path;

		pid_t pid;
		int pipe_in[2];
		int pipe_out[2];
		char buffer[PIPE_BUF];
		FILE * solver_output;


		// Create new pipes
		pipe(pipe_in);
		pipe(pipe_out);

		// Create the child process for the SAT solver
		pid = vfork();
		if (pid == (pid_t) 0)
		{
			//close unneeded pipes
			close(pipe_in[1]);
			close(pipe_out[0]);

			// concet pipes with stdin and stdout
			dup2(pipe_in[0], 0);
			close(pipe_in[0]);
			dup2(pipe_out[1], 1);
			close(pipe_out[1]);

			// Start process for SAT solver and hand over parameters
			if(run_props->sat_parameter != NULL)

			    {
				execl(sat_path,sat_path,run_props->sat_parameter,(char *)NULL);


			}
			else{

				execl(sat_path,sat_path,(char*)NULL);


			}
			exit(1);
		} else
		{
			close(pipe_in[0]);
			close(pipe_out[1]);

			write(pipe_in[1], cnf_dimacs, strlen(cnf_dimacs));

			close(pipe_in[1]);

			 if (waitpid (pid, NULL, WNOHANG) != 0) {
			          perror("waitpid()\n");

			 }

			solver_output = fdopen(pipe_out[0], "r");
		}

bool status = util_extractModel(solver_output, sat_model);
		return status ;

}



/*Extract output model from SAT solver*/
bool util_extractModel(FILE * solver_output, int * sat_model){

	/*Extract model from sat solver output*/
			char **curLine = malloc(sizeof(char*));
			   *curLine = NULL;

			int i = 0;
			int resultSection = 0;
			int status = 0;
			size_t *t = malloc(0);


			while ((status=getline(curLine,t,solver_output)) > 0) {

				if ((*curLine)[0] == 's') {
					if (strstr(*curLine, "UNSATISFIABLE")) {
						free(t);
						free(*curLine);
						free(curLine);
						pclose(solver_output);
						return false;
					}
					if (strstr(*curLine, "SATISFIABLE")) {
						resultSection = 1;
						continue;
					} else {
						pclose(solver_output);
						free(t);
						free(*curLine);
						free(curLine);
						return false;
					}
				}

			    if(resultSection){

				char * next = NULL;
				char * pch = strtok_r(*curLine, " ", &next);	// Split line by whitespace
				if (*curLine[0] == 'v') {
					pch = strtok_r(NULL, " ", &next);// second call to get rid of "v" in the beginning of each line
				}

				while (pch != NULL) {

					if (strcmp(pch, "0\n") == 0) {
						free(t);
						free(*curLine);
						free(curLine);
						pclose(solver_output);
						return true;
					}
					sat_model[i] = atoi(pch);
					i++;
					pch = strtok_r(NULL, " ", &next);
				}


			  }
			}

			pclose(solver_output);
			free(t);
			free(*curLine);
			free(curLine);

			return false;
}

// Parse input parameter from commandline
int util_parseParametersFromCMD(int argc, char* argv[],struct RunProperties * current_run_probs) {

	current_run_probs->file_format = NULL;
	current_run_probs->sat_parameter = NULL;
	current_run_probs->sat_path = NULL;
	current_run_probs->problem = NULL;
	current_run_probs->semantics = NULL;
	current_run_probs->track = NULL;
	current_run_probs->single_argument = NULL;
	current_run_probs->input_filePath = NULL;


	const char* ACCPETED_FORMATS = "[tgf,apx]";
	const char* SUPPORTED_PROBLEMS ="[SE-ST,EE-ST,DC-ST,DS-ST,SE-CO,EE-CO,DC-CO,DS-CO,SE-GR,EE-GR,DC-GR,DS-GR,SE-PR,EE-PR,DC-PR,DS-PR]";

	if (argc <= 1) {
		printf("MiniAF v1.0\nJonas Klein\n");
		exit(1);
	}

	for (int i = 1; i < argc; i++) {

		if (strcmp(argv[i], "--formats") == 0) {
			printf("%s\n", ACCPETED_FORMATS);
			exit(EXIT_SUCCESS);
		}
		if (strcmp(argv[i], "--problems") == 0) {
			printf("%s\n", SUPPORTED_PROBLEMS);
			exit(EXIT_SUCCESS);
		}
		if (strcmp(argv[i], "-p") == 0) {
			i++;
			if (strstr(SUPPORTED_PROBLEMS, argv[i]) != NULL) {
				current_run_probs->track = argv[i];
				continue;
			} else {
				printf("Problem not supported!Please run --problems to show the supported problems");
				exit(EXIT_FAILURE);
			}
		}

		if (strcmp(argv[i], "-f") == 0) {
			current_run_probs->input_filePath = argv[++i];
			continue;
		}
		if (strcmp(argv[i], "-fo") == 0) {
			++i;
			if (strstr(ACCPETED_FORMATS, argv[i]) != NULL && strlen(argv[i]) == 3) {
				current_run_probs->file_format = argv[i];
				continue;
			} else {
				printf("Format not supported!Please run --formats to show the supported formats");
				exit(EXIT_FAILURE);
			}
		}
		if (strcmp(argv[i], "-a") == 0) {
			current_run_probs->single_argument = argv[++i];
			continue;
		}

		if (strcmp(argv[i], "-sat") == 0) {
			current_run_probs->sat_path = argv[++i];
			continue;
		}
		if (strcmp(argv[i], "-satparam") == 0) {
			current_run_probs->sat_parameter = argv[++i];
			continue;
		}
	}
	return EXIT_SUCCESS;

}



