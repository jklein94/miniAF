#include "Semantics.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Labels.h"
#include <errno.h>
#include <stdbool.h>
#include "FileParser.h"
#include <sys/types.h>
#include <fcntl.h>
#include "Util.h"
#include <sys/resource.h>

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define BUFSIZE 128





void read_off_memory_status(statm_t * result){
  unsigned long dummy;
  const char* statm_path = "/proc/self/statm";

  FILE *f = fopen(statm_path,"r");
  printf("open\n");
  if(!f){
	printf("Cant open");
    perror(statm_path);
    abort();
  }
  if(7 != fscanf(f,"%ld %ld %ld %ld %ld %ld %ld",
    &result->size,&result->resident,&result->share,&result->text,&result->lib,&result->data,&result->dt))
  {
    perror(statm_path);
    abort();
  }

  printf("Data:%ld\n", &result->data);
  fclose(f);
}





void semantics_getCompleteConstraints(struct AF * current_framework) {

	vector * complete_constraints = &current_framework->complete_constraints;
	vector * argument_names = &current_framework->argument_names;
	vector * attacks_relations = &current_framework->attacks_relations;

	//default encoding
	//int encoding[6] = { 1, 1, 1, 1, 0, 0 };
	//TODO Support parsing of other encodings

	int total = argument_names->total;
	for (int i = 0; i < total; i++) {

		vector * attackerCurrentArgument = attacks_relations->items[i + total];

		if (attackerCurrentArgument != NULL
				&& attackerCurrentArgument->total == 0) {
			vector *in_orClause = malloc(sizeof(vector));
			vector *notOut_orClause = malloc(sizeof(vector));
			vector *notUndec_orClause = malloc(sizeof(vector));

			vector_init(in_orClause);
			vector_init(notOut_orClause);
			vector_init(notUndec_orClause);

			VECTOR_ADD(*in_orClause, util_arg_in(i));
			VECTOR_ADD(*notOut_orClause, util_arg_notOut(i, total));
			VECTOR_ADD(*notUndec_orClause, util_arg_notUndec(i, total));

			vector_add(complete_constraints, in_orClause);
			vector_add(complete_constraints, notOut_orClause);
			vector_add(complete_constraints, notUndec_orClause);

		}

		else {
			vector * in_out_undec_orClause = malloc(sizeof(vector));
			vector_init(in_out_undec_orClause);
			VECTOR_ADD(*in_out_undec_orClause, util_arg_in(i));
			VECTOR_ADD(*in_out_undec_orClause, util_arg_out(i, total));
			VECTOR_ADD(*in_out_undec_orClause, util_arg_undec(i, total));

			vector_add(complete_constraints, in_out_undec_orClause);

			vector * notIn_notOut_orClause = malloc(sizeof(vector));
			vector_init(notIn_notOut_orClause);

			vector * notIn_notUndec_orClause = malloc(sizeof(vector));
			vector_init(notIn_notUndec_orClause);

			vector * notOut_notUndec_orClause = malloc(sizeof(vector));
			vector_init(notOut_notUndec_orClause);

			VECTOR_ADD(*notIn_notOut_orClause, util_arg_notIn(i));
			VECTOR_ADD(*notIn_notOut_orClause, util_arg_notOut(i, total));
			vector_add(complete_constraints, notIn_notOut_orClause);

			VECTOR_ADD(*notIn_notUndec_orClause, util_arg_notIn(i));
			VECTOR_ADD(*notIn_notUndec_orClause, util_arg_notUndec(i, total));
			vector_add(complete_constraints, notIn_notUndec_orClause);

			VECTOR_ADD(*notOut_notUndec_orClause, util_arg_notOut(i, total));
			VECTOR_ADD(*notOut_notUndec_orClause, util_arg_notUndec(i, total));
			vector_add(complete_constraints, notOut_notUndec_orClause);

			vector * c3 = malloc(sizeof(vector));
			vector_init(c3);

			vector * c6 = malloc(sizeof(vector));
			vector_init(c6);

			vector * c7 = malloc(sizeof(vector));
			vector_init(c7);

			vector * c8 = malloc(sizeof(vector));
			vector_init(c8);

			for (int j = 0; j < attackerCurrentArgument->total; j++) {

				int numberCurrentAttacker =
						(intptr_t) attackerCurrentArgument->items[j];

				vector * notIn_attacked_out_attacker = malloc(sizeof(vector));
				vector_init(notIn_attacked_out_attacker);

				VECTOR_ADD(*notIn_attacked_out_attacker, util_arg_notIn(i));
				VECTOR_ADD(*notIn_attacked_out_attacker,
						util_arg_out(numberCurrentAttacker, total));

				vector_add(complete_constraints, notIn_attacked_out_attacker);

				VECTOR_ADD(*c3, util_arg_notOut(numberCurrentAttacker, total));

				vector * notIn_attacker_out_attacked = malloc(sizeof(vector));
				vector_init(notIn_attacker_out_attacked);

				VECTOR_ADD(*notIn_attacker_out_attacked,
						util_arg_notIn(numberCurrentAttacker));
				VECTOR_ADD(*notIn_attacker_out_attacked,
						util_arg_out(i, total));

				vector_add(complete_constraints, notIn_attacker_out_attacked);

				VECTOR_ADD(*c6, util_arg_in(numberCurrentAttacker));

			}
			VECTOR_ADD(*c3, util_arg_in(i));
			vector_add(complete_constraints, c3);

			VECTOR_ADD(*c6, util_arg_notOut(i, total));
			vector_add(complete_constraints, c6);
		}

	}

}

/*Enumerate all extension*/
void complete_getAllExtensions(struct AF* framework,
		struct RunProperties* run_props) {

	int total_number_arguments = framework->argument_names.total;

	struct SAT_Input input;
	util_initInput(&input, &framework->complete_constraints,total_number_arguments);

	char * cnf_dimacs = util_getCombinedInput(&input);

	int * output_model = malloc(total_number_arguments * 3 * sizeof(int));

	vector * result_negations = NULL;

	while (util_callExternalSatSolver(cnf_dimacs, output_model, run_props)) {

		free(cnf_dimacs);
		free(input.header);

		result_negations = malloc(sizeof(vector));
		vector_init(result_negations);

		vector * current_extension = malloc(sizeof(vector));
		vector_init(current_extension);

		//filter in,out and undec arguments from solver output
		for (int i = 0; i < total_number_arguments; i++) {
			if (output_model[i] > 0) {
				int in_argument_negation = output_model[i] * -1;
				vector_add(result_negations,
						(void*) ((intptr_t) in_argument_negation));
				vector_add(current_extension, (void*) ((intptr_t) i));
				continue;
			}

			int value =
					max(output_model[i + total_number_arguments],
							output_model[i + 2 * total_number_arguments])
							* -1;
			vector_add(result_negations, (void*) ((intptr_t) value));
		}

		vector_add(&framework->result_extensions, current_extension);

		util_updateInput(&input, result_negations, false);
		vector_free(result_negations);

		cnf_dimacs = util_getCombinedInput(&input);
	}

	util_printAllExtension(&framework->result_extensions,&framework->argument_names);

	free(output_model);
	free(input.body);
	free(input.header);

}

void stable_getAllExtensions(struct AF* framework,
		struct RunProperties* run_props) {

	for (int i = 0; i < framework->argument_names.total; i++) {
		vector *undec = malloc(sizeof(vector));
		vector_init(undec);
		vector_add(undec,
				(void*) (intptr_t) (util_arg_notUndec(i,
						framework->argument_names.total)));
		vector_add(&framework->complete_constraints, undec);
	}
	complete_getAllExtensions(framework, run_props);
}

void grounded_getAllExtensions(struct AF *framework,
		struct RunProperties* run_props) {

	int total_number_arguments = framework->argument_names.total;

	struct SAT_Input input;
	util_initInput(&input, &framework->complete_constraints,
			total_number_arguments);

	char * cnf_dimacs = util_getCombinedInput(&input);

	int * output_model = malloc(total_number_arguments * 3 * sizeof(int));

	vector * grounded = NULL;

	vector * in_Args = malloc(sizeof(vector));
	vector_init(in_Args);

	int num_undec_args = 0;
	int * index_single_argument = malloc(sizeof(int));

	if (run_props->single_argument != NULL) {
		index_single_argument = map_get_(&framework->names_to_index,
				run_props->single_argument);
	} else {
		*index_single_argument = -1;
		run_props->single_argument = malloc(1);

	}
	run_props->single_argument[0] = 'N';
	while (true) {

		if (!util_callExternalSatSolver(cnf_dimacs, output_model, run_props)) {
			break;
		}
		run_props->single_argument[0] = 'N';

		free(cnf_dimacs);
		free(input.header);

		vector_free(in_Args);
		in_Args = malloc(sizeof(vector));
		vector_init(in_Args);

		vector * inOut_toUndec = malloc(sizeof(vector));
		vector_init(inOut_toUndec);
		vector * undec_Args = malloc(sizeof(vector));
		vector_init(undec_Args);

		for (int i = 0; i < total_number_arguments; i++) {
			if (output_model[i] > 0) {
				vector_add(in_Args, (void *) (intptr_t) i);
				int num_Argument_undec = output_model[i]
						+ 2 * total_number_arguments;

				vector_add(inOut_toUndec,
						(void *) ((intptr_t) num_Argument_undec));
				if (output_model[i] - 1 == *index_single_argument) {
					run_props->single_argument[0] = 'Y';
				}

				continue;
			}
			if (output_model[i + total_number_arguments] > 0) {

				int num_Argument_undec =
						output_model[i + total_number_arguments]
								+ total_number_arguments;
				vector_add(inOut_toUndec,
						(void *) ((intptr_t) num_Argument_undec));
				continue;
			}
			if (output_model[i + 2 * total_number_arguments] > 0) {

				num_undec_args++;
				vector *current_undec_arg = malloc(sizeof(vector));
				vector_init(current_undec_arg);
				int num_Argument_undec = output_model[i
						+ 2 * total_number_arguments];
				vector_add(current_undec_arg,
						(void *) ((intptr_t) num_Argument_undec));
				vector_add(undec_Args, current_undec_arg);

				continue;
			}
		}

		if (num_undec_args == total_number_arguments) {
			break;
		}
		util_updateInput(&input, undec_Args, true);
		util_updateInput(&input, inOut_toUndec, false);
		vector_free(undec_Args);
		vector_free(inOut_toUndec);

		cnf_dimacs = util_getCombinedInput(&input);

	}
	vector_add(&framework->result_extensions, in_Args);
}

bool preferred_getAllExtensions(struct AF * framework,
		struct RunProperties * run_props,
		bool only_some_extension) {

	int total_number_arguments = framework->argument_names.total;

	struct SAT_Input input;
	util_initInput(&input, &framework->complete_constraints,
			total_number_arguments);

	vector * all_in = malloc(sizeof(vector));
	vector_init(all_in);

	for (int i = 0; i < total_number_arguments; i++) {
		vector_add(all_in, (void*) ((intptr_t) util_arg_in(i)));

	}

	util_updateInput(&input, all_in, false);

	vector_free(all_in);

	size_t * rest_clause_length = malloc(sizeof(size_t));

	int * output_model = malloc(total_number_arguments * 3 * sizeof(int));

	struct SAT_Input  cnfdf_input;

	while (true) {

		Labels* preferred = malloc(sizeof(Labels));
		labels_init(preferred);



		cnfdf_input.body_length = input.body_length;
		cnfdf_input.header_length = input.header_length;
		cnfdf_input.clauses_number = input.clauses_number;
		cnfdf_input.variables_number = input.variables_number;

		cnfdf_input.body = malloc(input.body_length + 1);
		memcpy(cnfdf_input.body, input.body, input.body_length + 1);

		cnfdf_input.header = malloc(input.header_length + 1);
		memcpy(cnfdf_input.header, input.header, input.header_length + 1);

		while (true) {

			char * cnfdf_temp = util_getCombinedInput(&cnfdf_input);
			if (!util_callExternalSatSolver(cnfdf_temp, output_model,
					run_props)) {
				break;
			}

			labels_destroy(preferred);
			free(cnfdf_temp);
			free(cnfdf_input.header);
			Labels* resultLabels = malloc(sizeof(Labels));
			labels_init(resultLabels);

			labels_assignLabelsFromSatModel(resultLabels, output_model,&framework->argument_names);

			preferred = labels_clone(resultLabels);
			bool isUndecEmpty = isEmpty(resultLabels->undecLabels);

			vector * cnfdf_new_clause = malloc(sizeof(vector));
			vector_init(cnfdf_new_clause);

			if (!isUndecEmpty) {

				for (int i = 0; i < resultLabels->inLabels->total; i++) {
					vector *in = malloc(sizeof(vector));
					vector_init(in);
					int * numArg = map_get(&framework->names_to_index,
							resultLabels->inLabels->items[i]);

					VECTOR_ADD(*in, util_arg_in(*numArg));
					vector_add(cnfdf_new_clause, in);

				}

			}

			vector * rest = malloc(sizeof(vector));
			vector_init(rest);
			for (int i = 0; i < resultLabels->outLabels->total; i++) {

				int * numArg = map_get(&framework->names_to_index,
						resultLabels->outLabels->items[i]);

				if (!isUndecEmpty) {

					vector * out = malloc(sizeof(vector));
					vector_init(out);
					VECTOR_ADD(*out,
							util_arg_out(*numArg, total_number_arguments));
					vector_add(cnfdf_new_clause, out);
				}

				VECTOR_ADD(*rest, util_arg_in(*numArg));

			}

			vector * rest_df = malloc(sizeof(vector));
			vector_init(rest_df);

			for (int i = 0; i < resultLabels->undecLabels->total; i++) {
				int * numArg = map_get(&framework->names_to_index,
						resultLabels->undecLabels->items[i]);

				VECTOR_ADD(*rest, util_arg_in(*numArg));

				if (!isUndecEmpty) {
					VECTOR_ADD(*rest_df, util_arg_in(*numArg));
				}

			}

			if (!isUndecEmpty) {
				vector_add(cnfdf_new_clause, rest_df);
			}

			util_updateInput(&cnfdf_input, cnfdf_new_clause, true);

			util_updateInput(&input, rest, false);

            // clean up
			vector_free(rest);

			for(int i = 0; i < cnfdf_new_clause->total;++i){

				vector_free(cnfdf_new_clause->items[i]);
			}
			vector_free(cnfdf_new_clause);

			labels_destroy(resultLabels);


			//cnfdf_temp = util_getCombinedInput(&cnfdf_input);

			if (isUndecEmpty) {
				break;
			}

		}

		free(cnfdf_input.body);

		if (labels_isEmpty(preferred))
			break;

		if (run_props->single_argument != NULL) {
			if (preferred->allLabels->base.nnodes > 0
					&& *(map_get(preferred->allLabels,
							run_props->single_argument)) != INARG)
				return false;
		}

		vector_add(&framework->result_extensions, preferred);

		if (only_some_extension) {
			break;
		}

		if (preferred->inLabels->total == total_number_arguments)
			break;

	}
	if (isEmpty(&framework->result_extensions)) {
		Labels* emptyLabelling = malloc(sizeof(Labels));
		labels_init(emptyLabelling);
		vector_add(&framework->result_extensions, emptyLabelling);
	}

	return true;

}

/*Eumerate some extension for different semantics*/
bool stable_getSomeExtension(struct AF* framework,
		struct RunProperties * run_props) {

	int total_number_arguments = framework->argument_names.total;

	for (int i = 0; i < total_number_arguments; i++) {
		vector *undec = malloc(sizeof(vector));
		vector_init(undec);
		VECTOR_ADD(*undec, util_arg_notUndec(i, total_number_arguments));
		vector_add(&framework->complete_constraints, undec);
	}

	struct SAT_Input input;
	util_initInput(&input, &framework->complete_constraints,
			total_number_arguments);

	char * cnf_dimacs = util_getCombinedInput(&input);

	int * output_model = malloc(total_number_arguments * 3 * sizeof(int));

	vector* result_extension = malloc(sizeof(vector));
	vector_init(result_extension);

	if (util_callExternalSatSolver(cnf_dimacs, output_model, run_props)) {
		for (int i = 0; i < total_number_arguments; i++) {
			if (output_model[i] > 0) {
				vector_add(result_extension, (void*) ((intptr_t) i));
			}
		}

		util_printSingleExtension(result_extension, &framework->argument_names,
		true);
		free(output_model);
		free(cnf_dimacs);
		vector_free(result_extension);

		return true;

	}

	free(output_model);
	free(cnf_dimacs);
	vector_free(result_extension);

	return false;
}

bool complete_getSomeExtension(struct AF* framework,
		struct RunProperties * run_props) {

	int total_number_arguments = framework->argument_names.total;

	struct SAT_Input input;
	util_initInput(&input, &framework->complete_constraints,
			total_number_arguments);

	char * cnf_dimacs = util_getCombinedInput(&input);

	int * output_model = malloc(total_number_arguments * 3 * sizeof(int));

	if (util_callExternalSatSolver(cnf_dimacs, output_model, run_props)) {
		for (int i = 0; i < total_number_arguments; i++) {
			if (output_model[i] > 0) {
				vector_add(&framework->result_extensions,
						(void*) ((intptr_t) i));
			}
		}

		free(output_model);
		free(cnf_dimacs);

		return true;

	}
	free(output_model);
	free(cnf_dimacs);

	return false;
}

bool grounded_getSomeExtension(struct AF * framework,struct RunProperties * run_props) {

	grounded_getAllExtensions(framework, run_props);

	vector* res = framework->result_extensions.items[0];

	if (res->total > 0) {
		return true;
	} else {
		return false;
	}
}

bool preferred_getSomeExtension(struct AF * framework,
		struct RunProperties * run_props) {
	if (preferred_getAllExtensions(framework, run_props, true))
		return true;
	else
		return false;
}


bool stable_credulous(struct AF* framework, struct RunProperties * run_props) {

	int total_number_arguments = framework->argument_names.total;

	for (int i = 0; i < total_number_arguments; i++) {
		vector *undec = malloc(sizeof(vector));
		vector_init(undec);
		VECTOR_ADD(*undec, util_arg_notUndec(i, total_number_arguments));
		vector_add(&framework->complete_constraints, undec);
	}

	int * index_single_argument = map_get(&framework->names_to_index,
			run_props->single_argument);
	vector *in_single_argument = malloc(sizeof(vector));
	vector_init(in_single_argument);
	VECTOR_ADD(*in_single_argument, util_arg_in(*index_single_argument));
	vector_add(&framework->complete_constraints, in_single_argument);

	struct SAT_Input input;
	util_initInput(&input, &framework->complete_constraints,
			total_number_arguments);

	char * cnf_dimacs = util_getCombinedInput(&input);

	int * output_model = malloc(total_number_arguments * 3 * sizeof(int));

	bool status = util_callExternalSatSolver(cnf_dimacs, output_model,
			run_props);

	vector_free(in_single_argument);
	free(output_model);
	free(cnf_dimacs);
	return status;

}

bool complete_credulous(struct AF* framework, struct RunProperties * run_props) {
	int * index_single_argument = map_get(&framework->names_to_index,
			run_props->single_argument);
	vector *in_single_argument = malloc(sizeof(vector));
	vector_init(in_single_argument);
	VECTOR_ADD(*in_single_argument, util_arg_in(*index_single_argument));

	vector_add(&framework->complete_constraints, in_single_argument);

	bool status = complete_getSomeExtension(framework, run_props);

	vector_free(in_single_argument);
	return status;
}

bool grounded_credulous(struct AF* framework, struct RunProperties * run_props) {

	if (!grounded_getSomeExtension(framework, run_props))
		return false;
	else {
		if (run_props->single_argument[0] == 'Y')
			return true;
		else
			false;

	}

	return false;

}

bool preferred_credulous(struct AF * framework,
		struct RunProperties * run_props) {
	return complete_credulous(framework, run_props);
}


bool stable_skeptical(struct AF* framework, struct RunProperties * run_props) {

	int total_number_arguments = framework->argument_names.total;
	for (int i = 0; i < total_number_arguments; i++) {
		vector *undec = malloc(sizeof(vector));
		vector_init(undec);
		VECTOR_ADD(*undec, util_arg_notUndec(i, total_number_arguments));
		vector_add(&framework->complete_constraints, undec);
	}

	int * index_single_argument = map_get_(&framework->names_to_index,
			run_props->single_argument);

	vector *in_single_argument = malloc(sizeof(vector));
	vector_init(in_single_argument);
	VECTOR_ADD(*in_single_argument,
			util_arg_out(*index_single_argument, total_number_arguments));
	vector_add(&framework->complete_constraints, in_single_argument);
	int * output_model = malloc(total_number_arguments * 3 * sizeof(int));

	struct SAT_Input input;
	util_initInput(&input, &framework->complete_constraints,
			total_number_arguments);

	char * cnf_dimacs = util_getCombinedInput(&input);

	bool isInExtension = false;

	while (1) {

		if (!util_callExternalSatSolver(cnf_dimacs, output_model, run_props)) {
			break;
		}
		free(cnf_dimacs);
		free(input.header);

		vector * undec_args = malloc(sizeof(vector));
		vector_init(undec_args);

		vector *in_out = malloc(sizeof(vector));
		vector_init(in_out);

		for (int i = 0; i < total_number_arguments; ++i) {
			if (output_model[i + 2 * total_number_arguments] > 0) {
				vector *undec = malloc(sizeof(vector));
				vector_init(undec);
				VECTOR_ADD(*undec,
						output_model[i + 2 * total_number_arguments]);
				vector_add(undec_args, undec);

			} else {
				int argument_value = 0;

				if (output_model[i] > 0) {
					argument_value = output_model[i]
							+ 2 * total_number_arguments;
					if (output_model[i] - 1 == *index_single_argument)
						isInExtension = true;
				} else
					argument_value = output_model[i + total_number_arguments]
							+ total_number_arguments;

				VECTOR_ADD(*in_out, argument_value);
			}
		}

		if (!isInExtension)
			return false;

		isInExtension = false;

		if (undec_args->total == total_number_arguments)
			break;

		util_updateInput(&input, undec_args, true);
		util_updateInput(&input, in_out, false);

		cnf_dimacs = util_getCombinedInput(&input);

		vector_free(in_out);
		vector_free(undec_args);

	}
	return true;

}

bool complete_skeptical(struct AF* framework, struct RunProperties * run_props) {
	int total_number_arguments = framework->argument_names.total;
	int * index_single_argument = map_get_(&framework->names_to_index,
			run_props->single_argument);

	int * output_model = malloc(total_number_arguments * 3 * sizeof(int));

	struct SAT_Input input;
	util_initInput(&input, &framework->complete_constraints,
			total_number_arguments);

	char * cnf_dimacs = util_getCombinedInput(&input);

	bool isInExtension = false;

	while (1) {

		if (!util_callExternalSatSolver(cnf_dimacs, output_model, run_props)) {
			break;
		}
		free(cnf_dimacs);
		free(input.header);

		vector * undec_args = malloc(sizeof(vector));
		vector_init(undec_args);

		vector *in_out = malloc(sizeof(vector));
		vector_init(in_out);

		for (int i = 0; i < total_number_arguments; ++i) {
			if (output_model[i + 2 * total_number_arguments] > 0) {
				vector *undec = malloc(sizeof(vector));
				vector_init(undec);
				VECTOR_ADD(*undec,
						output_model[i + 2 * total_number_arguments]);
				vector_add(undec_args, undec);

			} else {
				int argument_value = 0;
				if (output_model[i] > 0) {
					argument_value = output_model[i]
							+ 2 * total_number_arguments;
					if (output_model[i] - 1 == *index_single_argument)
						isInExtension = true;
				} else
					argument_value = output_model[i + total_number_arguments]
							+ total_number_arguments;

				VECTOR_ADD(*in_out, argument_value);
			}
		}

		if (!isInExtension)
			return false;

		isInExtension = false;

		if (undec_args->total == total_number_arguments)
			break;

		util_updateInput(&input, undec_args, true);
		util_updateInput(&input, in_out, false);

		cnf_dimacs = util_getCombinedInput(&input);

		vector_free(in_out);
		vector_free(undec_args);

	}

	return true;

}

bool grounded_skeptical(struct AF* framework, struct RunProperties * run_props) {
	return grounded_credulous(framework, run_props);
}

bool preferred_skeptical(struct AF* framework, struct RunProperties * run_props) {

	vector * clone_complete = malloc(sizeof(vector));
	vector_init(clone_complete);

	//clone complete constraints
	for (int i = 0; i < framework->complete_constraints.total; i++) {
		vector * currentClause = framework->complete_constraints.items[i];

		vector * temp = malloc(sizeof(vector));
		vector_init(temp);
		for (int j = 0; j < currentClause->total; j++) {
			VECTOR_ADD(*temp, currentClause->items[j]);
		}
		vector_add(clone_complete, temp);
	}

	if (!preferred_credulous(framework, run_props))
		return false;

	framework->complete_constraints = *clone_complete;

	return preferred_getAllExtensions(framework, run_props, false);
}
