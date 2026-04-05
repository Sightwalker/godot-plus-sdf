/**************************************************************************/
/*  sdf_object_3d.cpp                                                     */
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

#include "sdf_object_3d.h"

#include "core/object/callable_mp.h"
#include "core/object/class_db.h"
#include "core/os/os.h"
#include "scene/3d/sdf_shape_3d.h"
#include "servers/rendering/rendering_server.h"

void SDFObject3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_sdf_material", "material"), &SDFObject3D::set_sdf_material);
	ClassDB::bind_method(D_METHOD("get_sdf_material"), &SDFObject3D::get_sdf_material);
	ClassDB::bind_method(D_METHOD("set_render_mode", "mode"), &SDFObject3D::set_render_mode);
	ClassDB::bind_method(D_METHOD("get_render_mode"), &SDFObject3D::get_render_mode);
	ClassDB::bind_method(D_METHOD("set_inside_render_mode", "mode"), &SDFObject3D::set_inside_render_mode);
	ClassDB::bind_method(D_METHOD("get_inside_render_mode"), &SDFObject3D::get_inside_render_mode);
	ClassDB::bind_method(D_METHOD("set_operation", "operation"), &SDFObject3D::set_operation);
	ClassDB::bind_method(D_METHOD("get_operation"), &SDFObject3D::get_operation);
	ClassDB::bind_method(D_METHOD("set_operation_order", "order"), &SDFObject3D::set_operation_order);
	ClassDB::bind_method(D_METHOD("get_operation_order"), &SDFObject3D::get_operation_order);
	ClassDB::bind_method(D_METHOD("set_smoothness", "smoothness"), &SDFObject3D::set_smoothness);
	ClassDB::bind_method(D_METHOD("get_smoothness"), &SDFObject3D::get_smoothness);
	ClassDB::bind_method(D_METHOD("set_membership_layers", "layers"), &SDFObject3D::set_membership_layers);
	ClassDB::bind_method(D_METHOD("get_membership_layers"), &SDFObject3D::get_membership_layers);
	ClassDB::bind_method(D_METHOD("set_membership_layer_value", "layer_number", "value"), &SDFObject3D::set_membership_layer_value);
	ClassDB::bind_method(D_METHOD("get_membership_layer_value", "layer_number"), &SDFObject3D::get_membership_layer_value);
	ClassDB::bind_method(D_METHOD("set_affect_layers", "layers"), &SDFObject3D::set_affect_layers);
	ClassDB::bind_method(D_METHOD("get_affect_layers"), &SDFObject3D::get_affect_layers);
	ClassDB::bind_method(D_METHOD("set_affect_layer_value", "layer_number", "value"), &SDFObject3D::set_affect_layer_value);
	ClassDB::bind_method(D_METHOD("get_affect_layer_value", "layer_number"), &SDFObject3D::get_affect_layer_value);
	ClassDB::bind_method(D_METHOD("rebuild"), &SDFObject3D::_request_rebuild_from_child);

	ADD_GROUP("SDF", "sdf_");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "sdf_material", PROPERTY_HINT_RESOURCE_TYPE, "SDFMaterial3D"), "set_sdf_material", "get_sdf_material");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "sdf_render_mode", PROPERTY_HINT_ENUM, "Static,Dynamic,Character"), "set_render_mode", "get_render_mode");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "sdf_inside_render_mode", PROPERTY_HINT_ENUM, "Discard,Exit Surface,Two Sided"), "set_inside_render_mode", "get_inside_render_mode");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "sdf_operation", PROPERTY_HINT_ENUM, "Union,Subtract,Intersect,Smooth Union,Smooth Subtract,Smooth Intersect"), "set_operation", "get_operation");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "sdf_operation_order", PROPERTY_HINT_RANGE, "-4096,4096,1"), "set_operation_order", "get_operation_order");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sdf_smoothness", PROPERTY_HINT_RANGE, "0.001,4.0,0.001,or_greater"), "set_smoothness", "get_smoothness");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "sdf_membership_layers", PROPERTY_HINT_LAYERS_3D_RENDER), "set_membership_layers", "get_membership_layers");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "sdf_affect_layers", PROPERTY_HINT_LAYERS_3D_RENDER), "set_affect_layers", "get_affect_layers");

	BIND_ENUM_CONSTANT(SDF_OPERATION_UNION);
	BIND_ENUM_CONSTANT(SDF_OPERATION_SUBTRACT);
	BIND_ENUM_CONSTANT(SDF_OPERATION_INTERSECT);
	BIND_ENUM_CONSTANT(SDF_OPERATION_SMOOTH_UNION);
	BIND_ENUM_CONSTANT(SDF_OPERATION_SMOOTH_SUBTRACT);
	BIND_ENUM_CONSTANT(SDF_OPERATION_SMOOTH_INTERSECT);

	BIND_ENUM_CONSTANT(SDF_INSIDE_RENDER_DISCARD);
	BIND_ENUM_CONSTANT(SDF_INSIDE_RENDER_EXIT_SURFACE);
	BIND_ENUM_CONSTANT(SDF_INSIDE_RENDER_TWO_SIDED);
}

bool SDFObject3D::_uses_smooth_operation() const {
	return operation == SDF_OPERATION_SMOOTH_UNION || operation == SDF_OPERATION_SMOOTH_SUBTRACT || operation == SDF_OPERATION_SMOOTH_INTERSECT;
}

void SDFObject3D::_validate_property(PropertyInfo &p_property) const {
	if (p_property.name == "sdf_smoothness" && !_uses_smooth_operation()) {
		p_property.usage = PROPERTY_USAGE_NONE;
	}
	if (p_property.name == "sdf_affect_layers" && operation == SDF_OPERATION_UNION) {
		p_property.usage = PROPERTY_USAGE_NONE;
	}
}

void SDFObject3D::_collect_shapes_recursive(Node *p_root, Vector<SDFShape3D *> &r_shapes) const {
	const int child_count = p_root->get_child_count();
	for (int i = 0; i < child_count; i++) {
		Node *child = p_root->get_child(i);
		SDFObject3D *nested_object = Object::cast_to<SDFObject3D>(child);
		if (nested_object && nested_object != this) {
			continue;
		}

		SDFShape3D *shape = Object::cast_to<SDFShape3D>(child);
		if (shape) {
			r_shapes.push_back(shape);
		}

		_collect_shapes_recursive(child, r_shapes);
	}
}

void SDFObject3D::_sdf_material_changed() {
	if (!sdf_object.is_valid()) {
		return;
	}
	RenderingServer *rs = RS::get_singleton();
	if (rs == nullptr) {
		return;
	}

	RID material_rid;
	if (sdf_material.is_valid()) {
		material_rid = sdf_material->get_material_rid();
	}
	rs->sdf_object_set_material(sdf_object, material_rid);
	_request_rebuild_from_child();
}

void SDFObject3D::_sdf_operation_data_changed() {
	if (!sdf_object.is_valid()) {
		return;
	}

	RenderingServer *rs = RS::get_singleton();
	if (rs == nullptr) {
		return;
	}

	rs->sdf_object_set_operation_data(sdf_object, int(operation), operation_order, smoothness, membership_layers, affect_layers, int(inside_render_mode));
}

void SDFObject3D::_rebuild_compiled_data() {
	if (!sdf_object.is_valid()) {
		return;
	}
	RenderingServer *rs = RS::get_singleton();
	if (rs == nullptr) {
		return;
	}

	Vector<SDFShape3D *> shapes;
	_collect_shapes_recursive(this, shapes);
	struct SDFShapeOrderSort {
		_FORCE_INLINE_ bool operator()(const SDFShape3D *l, const SDFShape3D *r) const {
			if (l == nullptr || r == nullptr) {
				return l != nullptr;
			}
			const int l_order = l->get_operation_order();
			const int r_order = r->get_operation_order();
			if (l_order == r_order) {
				return l->get_instance_id() < r->get_instance_id();
			}
			return l_order < r_order;
		}
	};
	shapes.sort_custom<SDFShapeOrderSort>();

	PackedInt32Array int_data;
	PackedFloat32Array float_data;
	int_data.push_back(shapes.size());
	int_data.push_back(1); // Layout version.

	Color object_tint(1.0f, 1.0f, 1.0f, 1.0f);
	if (sdf_material.is_valid()) {
		object_tint = sdf_material->get_albedo();
	}

	AABB merged_aabb;
	bool has_bounds = false;

	for (int i = 0; i < shapes.size(); i++) {
		SDFShape3D *shape = shapes[i];
		ERR_CONTINUE(shape == nullptr);

		Transform3D local_xform(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
		Node *node = shape;
		bool reached_owner = false;
		while (node) {
			if (node == this) {
				reached_owner = true;
				break;
			}

			Node3D *node_3d = Object::cast_to<Node3D>(node);
			if (node_3d) {
				local_xform = node_3d->get_transform() * local_xform;
			}

			node = node->get_parent();
		}
		ERR_CONTINUE(!reached_owner);

		const int shape_float_base = float_data.size();
		shape->append_compiled_data(int_data, float_data, local_xform);

		Color shape_tint = object_tint;
		const Ref<SDFMaterial3D> &shape_material_override = shape->get_sdf_material_override();
		if (shape_material_override.is_valid()) {
			shape_tint = shape_material_override->get_albedo();
		}

		if (float_data.size() >= shape_float_base + 30) {
			float_data.set(shape_float_base + 26, float_data[shape_float_base + 26] * shape_tint.r);
			float_data.set(shape_float_base + 27, float_data[shape_float_base + 27] * shape_tint.g);
			float_data.set(shape_float_base + 28, float_data[shape_float_base + 28] * shape_tint.b);
		}

		AABB shape_bounds = local_xform.xform(shape->get_estimated_local_aabb());
		if (!has_bounds) {
			merged_aabb = shape_bounds;
			has_bounds = true;
		} else {
			merged_aabb.merge_with(shape_bounds);
		}
	}

	if (!has_bounds) {
		merged_aabb = AABB(Vector3(-0.05, -0.05, -0.05), Vector3(0.1, 0.1, 0.1));
	}

	cached_shape_count = shapes.size();
	cached_aabb = merged_aabb;

	rs->sdf_object_set_compiled_data(sdf_object, int_data, float_data);
	rs->sdf_object_set_bounds(sdf_object, merged_aabb);
	rs->sdf_object_set_render_mode(sdf_object, render_mode);
	_sdf_operation_data_changed();
	update_gizmos();
	update_configuration_warnings();
}

void SDFObject3D::_rebuild_compiled_data_deferred() {
	rebuild_queued = false;
	_rebuild_compiled_data();
}

void SDFObject3D::_request_rebuild_from_child() {
	if (rebuild_queued) {
		return;
	}
	rebuild_queued = true;
	callable_mp(this, &SDFObject3D::_rebuild_compiled_data_deferred).call_deferred();
}

void SDFObject3D::set_sdf_material(const Ref<SDFMaterial3D> &p_material) {
	if (sdf_material == p_material) {
		return;
	}

	if (sdf_material.is_valid()) {
		sdf_material->disconnect_changed(callable_mp(this, &SDFObject3D::_sdf_material_changed));
	}

	sdf_material = p_material;

	if (sdf_material.is_valid()) {
		sdf_material->connect_changed(callable_mp(this, &SDFObject3D::_sdf_material_changed), CONNECT_REFERENCE_COUNTED);
	}

	_sdf_material_changed();
}

Ref<SDFMaterial3D> SDFObject3D::get_sdf_material() const {
	return sdf_material;
}

void SDFObject3D::set_render_mode(RSE::SDFRenderMode p_mode) {
	if (render_mode == p_mode) {
		return;
	}
	render_mode = p_mode;

	if (sdf_object.is_valid()) {
		RenderingServer *rs = RS::get_singleton();
		if (rs) {
			rs->sdf_object_set_render_mode(sdf_object, render_mode);
		}
	}
}

RSE::SDFRenderMode SDFObject3D::get_render_mode() const {
	return render_mode;
}

void SDFObject3D::set_inside_render_mode(SDFInsideRenderMode p_mode) {
	ERR_FAIL_COND(p_mode < SDF_INSIDE_RENDER_DISCARD || p_mode > SDF_INSIDE_RENDER_TWO_SIDED);
	if (inside_render_mode == p_mode) {
		return;
	}
	inside_render_mode = p_mode;
	_sdf_operation_data_changed();
}

SDFObject3D::SDFInsideRenderMode SDFObject3D::get_inside_render_mode() const {
	return inside_render_mode;
}

void SDFObject3D::set_operation(SDFOperation p_operation) {
	if (operation == p_operation) {
		return;
	}
	operation = p_operation;
	notify_property_list_changed();
	_sdf_operation_data_changed();
}

SDFObject3D::SDFOperation SDFObject3D::get_operation() const {
	return operation;
}

void SDFObject3D::set_operation_order(int p_operation_order) {
	if (operation_order == p_operation_order) {
		return;
	}
	operation_order = p_operation_order;
	_sdf_operation_data_changed();
}

int SDFObject3D::get_operation_order() const {
	return operation_order;
}

void SDFObject3D::set_smoothness(float p_smoothness) {
	smoothness = MAX(0.001f, p_smoothness);
	_sdf_operation_data_changed();
}

float SDFObject3D::get_smoothness() const {
	return smoothness;
}

void SDFObject3D::set_membership_layers(uint32_t p_layers) {
	membership_layers = p_layers;
	_sdf_operation_data_changed();
}

uint32_t SDFObject3D::get_membership_layers() const {
	return membership_layers;
}

void SDFObject3D::set_membership_layer_value(int p_layer_number, bool p_value) {
	ERR_FAIL_COND(p_layer_number < 1 || p_layer_number > 32);
	if (p_value) {
		membership_layers |= 1u << (p_layer_number - 1);
	} else {
		membership_layers &= ~(1u << (p_layer_number - 1));
	}
	_sdf_operation_data_changed();
}

bool SDFObject3D::get_membership_layer_value(int p_layer_number) const {
	ERR_FAIL_COND_V(p_layer_number < 1 || p_layer_number > 32, false);
	return (membership_layers & (1u << (p_layer_number - 1))) != 0;
}

void SDFObject3D::set_affect_layers(uint32_t p_layers) {
	affect_layers = p_layers;
	_sdf_operation_data_changed();
}

uint32_t SDFObject3D::get_affect_layers() const {
	return affect_layers;
}

void SDFObject3D::set_affect_layer_value(int p_layer_number, bool p_value) {
	ERR_FAIL_COND(p_layer_number < 1 || p_layer_number > 32);
	if (p_value) {
		affect_layers |= 1u << (p_layer_number - 1);
	} else {
		affect_layers &= ~(1u << (p_layer_number - 1));
	}
	_sdf_operation_data_changed();
}

bool SDFObject3D::get_affect_layer_value(int p_layer_number) const {
	ERR_FAIL_COND_V(p_layer_number < 1 || p_layer_number > 32, false);
	return (affect_layers & (1u << (p_layer_number - 1))) != 0;
}

AABB SDFObject3D::get_aabb() const {
	return cached_aabb;
}

PackedStringArray SDFObject3D::get_configuration_warnings() const {
	PackedStringArray warnings = GeometryInstance3D::get_configuration_warnings();

	if (OS::get_singleton()->get_current_rendering_method() != "forward_plus") {
		warnings.push_back(RTR("SDFObject3D runtime rendering is only supported with the Forward+ renderer. Other renderers keep SDF data for compatibility only."));
	}

	if (cached_shape_count == 0) {
		warnings.push_back(RTR("SDFObject3D has no SDFShape3D descendants. Add one or more SDFShape3D nodes to render anything."));
	}

	return warnings;
}

void SDFObject3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE:
		case NOTIFICATION_CHILD_ORDER_CHANGED: {
			_request_rebuild_from_child();
		} break;
	}
}

SDFObject3D::SDFObject3D() {
	set_notify_transform(true);
	RenderingServer *rs = RS::get_singleton();
	ERR_FAIL_NULL_MSG(rs, "RenderingServer singleton must exist before creating SDFObject3D.");
	sdf_object = rs->sdf_object_create();
	set_base(sdf_object);
	rs->sdf_object_set_render_mode(sdf_object, render_mode);
	rs->sdf_object_set_operation_data(sdf_object, int(operation), operation_order, smoothness, membership_layers, affect_layers, int(inside_render_mode));
}

SDFObject3D::~SDFObject3D() {
	if (sdf_material.is_valid()) {
		sdf_material->disconnect_changed(callable_mp(this, &SDFObject3D::_sdf_material_changed));
	}

	if (sdf_object.is_valid()) {
		RenderingServer *rs = RenderingServer::get_singleton();
		if (rs) {
			set_base(RID());
			rs->free_rid(sdf_object);
		}
	}
}
