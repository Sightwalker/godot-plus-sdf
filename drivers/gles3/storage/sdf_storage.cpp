/**************************************************************************/
/*  sdf_storage.cpp                                                       */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "sdf_storage.h"

#ifdef GLES3_ENABLED

using namespace GLES3;

SDFStorage *SDFStorage::singleton = nullptr;

SDFStorage::SDFStorage() {
	ERR_FAIL_COND_MSG(singleton != nullptr, "A GLES3::SDFStorage singleton already exists.");
	singleton = this;
}

SDFStorage::~SDFStorage() {
	singleton = nullptr;
}

RID SDFStorage::sdf_object_allocate() {
	return sdf_object_owner.allocate_rid();
}

void SDFStorage::sdf_object_initialize(RID p_rid) {
	sdf_object_owner.initialize_rid(p_rid, SDFObject());
}

void SDFStorage::sdf_object_free(RID p_rid) {
	SDFObject *sdf_object = sdf_object_owner.get_or_null(p_rid);
	ERR_FAIL_NULL(sdf_object);
	sdf_object->dependency.deleted_notify(p_rid);
	sdf_object_owner.free(p_rid);
}

bool SDFStorage::owns_sdf_object(RID p_rid) const {
	return sdf_object_owner.owns(p_rid);
}

void SDFStorage::sdf_object_set_compiled_data(RID p_sdf_object, const PackedInt32Array &p_int_data, const PackedFloat32Array &p_float_data) {
	SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL(sdf_object);
	sdf_object->int_data = p_int_data;
	sdf_object->float_data = p_float_data;
	sdf_object->dependency.changed_notify(Dependency::DEPENDENCY_CHANGED_MESH);
}

void SDFStorage::sdf_object_set_bounds(RID p_sdf_object, const AABB &p_bounds) {
	SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL(sdf_object);
	sdf_object->bounds = p_bounds;
	sdf_object->dependency.changed_notify(Dependency::DEPENDENCY_CHANGED_AABB);
}

AABB SDFStorage::sdf_object_get_bounds(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, AABB());
	return sdf_object->bounds;
}

void SDFStorage::sdf_object_set_material(RID p_sdf_object, RID p_material) {
	SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL(sdf_object);
	sdf_object->material = p_material;
	sdf_object->dependency.changed_notify(Dependency::DEPENDENCY_CHANGED_MATERIAL);
}

RID SDFStorage::sdf_object_get_material(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, RID());
	return sdf_object->material;
}

void SDFStorage::sdf_object_set_render_mode(RID p_sdf_object, RSE::SDFRenderMode p_mode) {
	SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL(sdf_object);
	sdf_object->render_mode = p_mode;
}

RSE::SDFRenderMode SDFStorage::sdf_object_get_render_mode(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, RSE::SDF_RENDER_MODE_STATIC);
	return sdf_object->render_mode;
}

void SDFStorage::sdf_object_set_operation_data(RID p_sdf_object, int p_operation, int p_operation_order, float p_smoothness, uint32_t p_membership_layers, uint32_t p_affect_layers, int p_inside_render_mode) {
	SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL(sdf_object);
	sdf_object->operation = p_operation;
	sdf_object->operation_order = p_operation_order;
	sdf_object->smoothness = p_smoothness;
	sdf_object->membership_layers = p_membership_layers;
	sdf_object->affect_layers = p_affect_layers;
	sdf_object->inside_render_mode = p_inside_render_mode;
	sdf_object->dependency.changed_notify(Dependency::DEPENDENCY_CHANGED_MESH);
}

int SDFStorage::sdf_object_get_operation(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, 0);
	return sdf_object->operation;
}

int SDFStorage::sdf_object_get_operation_order(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, 0);
	return sdf_object->operation_order;
}

float SDFStorage::sdf_object_get_smoothness(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, 0.15f);
	return sdf_object->smoothness;
}

uint32_t SDFStorage::sdf_object_get_membership_layers(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, 1);
	return sdf_object->membership_layers;
}

uint32_t SDFStorage::sdf_object_get_affect_layers(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, 0xFFFFFFFFu);
	return sdf_object->affect_layers;
}

int SDFStorage::sdf_object_get_inside_render_mode(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, 1);
	return sdf_object->inside_render_mode;
}

PackedInt32Array SDFStorage::sdf_object_get_compiled_int_data(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, PackedInt32Array());
	return sdf_object->int_data;
}

PackedFloat32Array SDFStorage::sdf_object_get_compiled_float_data(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, PackedFloat32Array());
	return sdf_object->float_data;
}

RID SDFStorage::sdf_object_get_compiled_int_buffer(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, RID());
	return RID();
}

RID SDFStorage::sdf_object_get_compiled_float_buffer(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, RID());
	return RID();
}

uint32_t SDFStorage::sdf_object_get_shape_count(RID p_sdf_object) const {
	const SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, 0);
	if (sdf_object->int_data.is_empty()) {
		return 0;
	}
	return uint32_t(MAX(sdf_object->int_data[0], 0));
}

Dependency *SDFStorage::sdf_object_get_dependency(RID p_sdf_object) const {
	SDFObject *sdf_object = sdf_object_owner.get_or_null(p_sdf_object);
	ERR_FAIL_NULL_V(sdf_object, nullptr);
	return &sdf_object->dependency;
}

#endif // GLES3_ENABLED
