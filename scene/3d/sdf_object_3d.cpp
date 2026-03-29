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
	ClassDB::bind_method(D_METHOD("rebuild"), &SDFObject3D::_request_rebuild_from_child);

	ADD_GROUP("SDF", "sdf_");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "sdf_material", PROPERTY_HINT_RESOURCE_TYPE, "SDFMaterial3D"), "set_sdf_material", "get_sdf_material");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "sdf_render_mode", PROPERTY_HINT_ENUM, "Static,Dynamic,Character"), "set_render_mode", "get_render_mode");
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

	PackedInt32Array int_data;
	PackedFloat32Array float_data;
	int_data.push_back(shapes.size());
	int_data.push_back(1); // Layout version.

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

		shape->append_compiled_data(int_data, float_data, local_xform);

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
	_sdf_material_changed();
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
