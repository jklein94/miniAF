
#include <stdio.h>
#include "FileParser.h"
#include "Semantics.h"
#include <stdlib.h>
#include "Labels.h"
#include <limits.h>
#include <time.h>
#include "Util.h"

int main(int argc, char **argv) {

	struct RunProperties run_props;
	int parameter_parse_status = util_parseParametersFromCMD(argc, argv,&run_props);

	//TODO Implement Help
	if (parameter_parse_status != EXIT_SUCCESS) {
		printf("Incorrect or incomplete input parameters!");
		return -1;
	}

	// set up datastructures
	 struct AF framework;
	map_init(&framework.names_to_index);
	vector_init(&(framework.argument_names));
	vector_init(&framework.attacks_relations);
	vector_init(&framework.complete_constraints);
	vector_init(&framework.result_extensions);


//
//	struct timeval start, end;
//
//	    gettimeofday(&start, NULL);

//


    bool parse_status = parser_parseInputfile(&framework, &run_props);

    if (!parse_status) {
		printf("Unable to parse file: %s\n", run_props.input_filePath);
		exit(EXIT_FAILURE);
	}

	// split track in problem and semantics
	char* next = NULL;
	run_props.problem = strtok_r(run_props.track, "-", &next);
	run_props.semantics = strtok_r(NULL, "-", &next);


	bool attacks_exist = true;
	if(framework.attacks_relations.total != framework.argument_names.total *2){
		attacks_exist = false;
	}

//
//	// EE, SE problem for different semantics
	if (run_props.single_argument == NULL) {

		// No attack relations, so every argument is accepted
		if(!attacks_exist){
			if (strcmp(run_props.problem, "SE") == 0){
				printf("[");
				vector_print(&(framework.argument_names));
				printf("]\n");
			}
			else{
				printf("[[");
				vector_print(&(framework.argument_names));
				printf("]]\n");
			}
			return 0;
		}

	  semantics_getCompleteConstraints(&framework);

		if (strcmp(run_props.semantics, "ST") == 0) {
			if (strcmp(run_props.problem, "EE") == 0) {
				stable_getAllExtensions(&framework,&run_props);
			}

			if (strcmp(run_props.problem, "SE") == 0) {

				if (!stable_getSomeExtension(&framework,&run_props))
					printf("NO\n");

			}

		}
		else if (strcmp(run_props.semantics, "CO") == 0) {
			if (strcmp(run_props.problem, "EE") == 0) {

				complete_getAllExtensions(&framework,&run_props);
			}

			if (strcmp(run_props.problem, "SE") == 0) {

				if (complete_getSomeExtension(&framework,&run_props))
					util_printSingleExtension(&framework.result_extensions, &framework.argument_names,true);
				else printf("NO\n");
			}
		}
//
		else if (strcmp(run_props.semantics, "GR") == 0) {
			if (strcmp(run_props.problem, "EE") == 0) {
				grounded_getAllExtensions(&framework,&run_props);
				util_printAllExtension(&framework.result_extensions, &framework.argument_names);

			}

			if (strcmp(run_props.problem, "SE") == 0) {
				if(grounded_getSomeExtension(&framework, &run_props)){
					util_printSingleExtension(framework.result_extensions.items[0], &framework.argument_names,true);
				}
				else{
					printf("NO\n");
				}
			}
		}

		else if (strcmp(run_props.semantics, "PR") == 0) {
			if (strcmp(run_props.problem, "EE") == 0) {
				preferred_getAllExtensions(&framework,&run_props,false);
				printlabelsVector(&framework.result_extensions);
			}

			if (strcmp(run_props.problem, "SE") == 0) {

				if(preferred_getSomeExtension(&framework,&run_props)){
					Labels * res = (Labels *)framework.result_extensions.items[0];
					if(res->inLabels->total < 1){
						printf("NO\n");
					}
					else{
						printf("%s",labels_inLabelsToString(res));
					}


				}
				else{
					printf("NO\n");
				}

			}

		}


	} else {

		if(attacks_exist == 0){
				printf("YES\n");
			return 0;
		}

		 semantics_getCompleteConstraints(&framework);

		if (strcmp(run_props.semantics, "ST") == 0) {
			if (strcmp(run_props.problem, "DC") == 0) {
				bool status = stable_credulous(&framework,&run_props);
				if (status) {
					printf("YES\n");
				} else {
					printf("NO\n");
				}
			}
				if (strcmp(run_props.problem, "DS") == 0) {
					bool status = stable_skeptical(&framework,&run_props);
					if (status) {
						printf("YES\n");
					} else {
						printf("NO\n");
					}
				}
			}

		if (strcmp(run_props.semantics, "CO") == 0) {
					if (strcmp(run_props.problem, "DC") == 0) {
					    bool status = complete_credulous(&framework,&run_props);
						if (status) {
							printf("YES\n");
						} else {
							printf("NO\n");
						}
					}
						if (strcmp(run_props.problem, "DS") == 0) {
							bool status = complete_skeptical(&framework,&run_props);
							if (status) {
								printf("YES\n");
							} else {
								printf("NO\n");
							}
						}
					}
		if (strcmp(run_props.semantics, "GR") == 0) {
							if (strcmp(run_props.problem, "DC") == 0) {
								bool status = grounded_credulous(&framework,&run_props);
								if (status) {
									printf("YES\n");
								} else {
									printf("NO\n");
								}
							}
								if (strcmp(run_props.problem, "DS") == 0) {
									bool status = grounded_skeptical(&framework,&run_props);
									if (status) {
										printf("YES\n");
									} else {
										printf("NO\n");
									}
								}
							}
		if (strcmp(run_props.semantics, "PR") == 0) {
									if (strcmp(run_props.problem, "DC") == 0) {
										bool status = preferred_credulous(&framework,&run_props);
										if (status) {
											printf("YES\n");
										} else {
											printf("NO\n");
										}
									}
										if (strcmp(run_props.problem, "DS") == 0) {
											bool status = preferred_skeptical(&framework,&run_props);
											if (status) {
												printf("YES\n");
											} else {
												printf("NO\n");
											}
										}
									}
		}
//
////
////	gettimeofday(&end, NULL);
////
////		    double time_taken = end.tv_sec + end.tv_usec / 1e6 -
////		                        start.tv_sec - start.tv_usec / 1e6; // in seconds
////
////		   printf("time program took %f seconds to execute\n", time_taken);
//
//
//
//	   for(int i = 0;i < complete_constraints.total;i++){
//		   vector_free(complete_constraints.items[i]);
//	   }
//	   for(int i = 0;i < attacks_relations.total;i++){
//	 		   vector_free(attacks_relations.items[i]);
//	 	   }
//
//
//
//		//clean up
//		vector_free(&complete_constraints);
//		vector_free(&argument_names);
//		vector_free(&result_extensions);
//		vector_free(&attacks_relations);
//		map_deinit_(&names_to_index);

		return 0;
	}




