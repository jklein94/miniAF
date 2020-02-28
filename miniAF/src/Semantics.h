/*
 * Semantics.h
 *
 *  Created on: 28.10.2019
 *      Author: jonas
 */

#ifndef SRC_SEMANTICS_H_
#define SRC_SEMANTICS_H_
#include "Vector.h"
#include "map.h"
#include "Labels.h"
#include <stdbool.h>
#include "Util.h"

typedef struct {
    long size,resident,share,text,lib,data,dt;
} statm_t;

void read_off_memory_status(statm_t *result);

void semantics_getCompleteConstraints(struct AF * current_framework);

void stable_getAllExtensions(struct AF* framework,struct RunProperties* run_props);
void complete_getAllExtensions(struct AF* framework,struct RunProperties* run_props);
void grounded_getAllExtensions(struct AF *framework, struct RunProperties* run_props);
bool preferred_getAllExtensions(struct AF* framework,struct RunProperties* run_props, bool only_some_extension);


bool stable_getSomeExtension(struct AF* framework,struct RunProperties * run_props);
bool complete_getSomeExtension(struct AF* framework,struct RunProperties * run_props);
bool grounded_getSomeExtension(struct AF* framework,struct RunProperties * run_props);
bool preferred_getSomeExtension(struct AF * framework,struct RunProperties * run_props);

bool stable_credulous(struct AF* framework,struct RunProperties * run_props);
bool complete_credulous(struct AF* framework,struct RunProperties * run_props);
bool grounded_credulous(struct AF* framework,struct RunProperties * run_props);
bool preferred_credulous(struct AF* framework,struct RunProperties * run_props);


bool stable_skeptical(struct AF* framework,struct RunProperties * run_props);
bool complete_skeptical(struct AF* framework,struct RunProperties * run_props);
bool preferred_skeptical(struct AF* framework,struct RunProperties * run_props);
bool grounded_skeptical(struct AF* framework,struct RunProperties * run_props);

#endif /* SRC_SEMANTICS_H_ */
