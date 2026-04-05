/**************************************************************************/
/*  sdf_shape_3d.cpp                                                      */
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

#include "sdf_shape_3d.h"

#include "core/object/callable_mp.h"
#include "core/object/class_db.h"
#include "scene/3d/sdf_object_3d.h"
#include "servers/rendering/rendering_server.h"

void SDFShape3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_shape_type", "shape_type"), &SDFShape3D::set_shape_type);
	ClassDB::bind_method(D_METHOD("get_shape_type"), &SDFShape3D::get_shape_type);
	ClassDB::bind_method(D_METHOD("set_operation", "operation"), &SDFShape3D::set_operation);
	ClassDB::bind_method(D_METHOD("get_operation"), &SDFShape3D::get_operation);
	ClassDB::bind_method(D_METHOD("set_operation_order", "order"), &SDFShape3D::set_operation_order);
	ClassDB::bind_method(D_METHOD("get_operation_order"), &SDFShape3D::get_operation_order);
	ClassDB::bind_method(D_METHOD("set_render_mode_hint", "mode"), &SDFShape3D::set_render_mode_hint);
	ClassDB::bind_method(D_METHOD("get_render_mode_hint"), &SDFShape3D::get_render_mode_hint);
	ClassDB::bind_method(D_METHOD("set_smoothness", "smoothness"), &SDFShape3D::set_smoothness);
	ClassDB::bind_method(D_METHOD("get_smoothness"), &SDFShape3D::get_smoothness);

	ClassDB::bind_method(D_METHOD("set_membership_layers", "layers"), &SDFShape3D::set_membership_layers);
	ClassDB::bind_method(D_METHOD("get_membership_layers"), &SDFShape3D::get_membership_layers);
	ClassDB::bind_method(D_METHOD("set_membership_layer_value", "layer_number", "value"), &SDFShape3D::set_membership_layer_value);
	ClassDB::bind_method(D_METHOD("get_membership_layer_value", "layer_number"), &SDFShape3D::get_membership_layer_value);

	ClassDB::bind_method(D_METHOD("set_affect_layers", "layers"), &SDFShape3D::set_affect_layers);
	ClassDB::bind_method(D_METHOD("get_affect_layers"), &SDFShape3D::get_affect_layers);
	ClassDB::bind_method(D_METHOD("set_affect_layer_value", "layer_number", "value"), &SDFShape3D::set_affect_layer_value);
	ClassDB::bind_method(D_METHOD("get_affect_layer_value", "layer_number"), &SDFShape3D::get_affect_layer_value);

	ClassDB::bind_method(D_METHOD("set_size", "size"), &SDFShape3D::set_size);
	ClassDB::bind_method(D_METHOD("get_size"), &SDFShape3D::get_size);
	ClassDB::bind_method(D_METHOD("set_axis", "axis"), &SDFShape3D::set_axis);
	ClassDB::bind_method(D_METHOD("get_axis"), &SDFShape3D::get_axis);
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &SDFShape3D::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &SDFShape3D::get_radius);
	ClassDB::bind_method(D_METHOD("set_secondary_radius", "secondary_radius"), &SDFShape3D::set_secondary_radius);
	ClassDB::bind_method(D_METHOD("get_secondary_radius"), &SDFShape3D::get_secondary_radius);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &SDFShape3D::set_height);
	ClassDB::bind_method(D_METHOD("get_height"), &SDFShape3D::get_height);
	ClassDB::bind_method(D_METHOD("set_thickness", "thickness"), &SDFShape3D::set_thickness);
	ClassDB::bind_method(D_METHOD("get_thickness"), &SDFShape3D::get_thickness);
	ClassDB::bind_method(D_METHOD("set_angle", "angle"), &SDFShape3D::set_angle);
	ClassDB::bind_method(D_METHOD("get_angle"), &SDFShape3D::get_angle);
	ClassDB::bind_method(D_METHOD("set_angle2", "angle2"), &SDFShape3D::set_angle2);
	ClassDB::bind_method(D_METHOD("get_angle2"), &SDFShape3D::get_angle2);
	ClassDB::bind_method(D_METHOD("set_roundness", "roundness"), &SDFShape3D::set_roundness);
	ClassDB::bind_method(D_METHOD("get_roundness"), &SDFShape3D::get_roundness);
	ClassDB::bind_method(D_METHOD("set_sdf_material_override", "material"), &SDFShape3D::set_sdf_material_override);
	ClassDB::bind_method(D_METHOD("get_sdf_material_override"), &SDFShape3D::get_sdf_material_override);
	ClassDB::bind_method(D_METHOD("set_color", "color"), &SDFShape3D::set_color);
	ClassDB::bind_method(D_METHOD("get_color"), &SDFShape3D::get_color);
	ClassDB::bind_method(D_METHOD("set_opacity", "opacity"), &SDFShape3D::set_opacity);
	ClassDB::bind_method(D_METHOD("get_opacity"), &SDFShape3D::get_opacity);

	ADD_GROUP("SDF Shape", "shape_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "shape_type", PROPERTY_HINT_ENUM, "Sphere,Box,Rounded Box,Torus,Capped Torus,Link,Capsule,Cylinder,Capped Cylinder,Cone,Capped Cone,Round Cone,Plane,Hex Prism,Tri Prism,Ellipsoid,Cut Sphere,Cut Hollow Sphere,Death Star,Solid Angle"), "set_shape_type", "get_shape_type");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "operation", PROPERTY_HINT_ENUM, "Union,Subtract,Intersect,Smooth Union,Smooth Subtract,Smooth Intersect"), "set_operation", "get_operation");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "operation_order", PROPERTY_HINT_RANGE, "-4096,4096,1"), "set_operation_order", "get_operation_order");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "render_mode_hint", PROPERTY_HINT_ENUM, "Static,Dynamic,Character"), "set_render_mode_hint", "get_render_mode_hint");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "smoothness", PROPERTY_HINT_RANGE, "0.001,4.0,0.001,or_greater"), "set_smoothness", "get_smoothness");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "membership_layers", PROPERTY_HINT_LAYERS_3D_RENDER), "set_membership_layers", "get_membership_layers");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "affect_layers", PROPERTY_HINT_LAYERS_3D_RENDER), "set_affect_layers", "get_affect_layers");

	ADD_GROUP("Parameters", "");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "size", PROPERTY_HINT_RANGE, "0.001,4096,0.001,or_greater,suffix:m"), "set_size", "get_size");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "axis"), "set_axis", "get_axis");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius", PROPERTY_HINT_RANGE, "0.001,4096,0.001,or_greater,suffix:m"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "secondary_radius", PROPERTY_HINT_RANGE, "0.001,4096,0.001,or_greater,suffix:m"), "set_secondary_radius", "get_secondary_radius");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "height", PROPERTY_HINT_RANGE, "0.001,4096,0.001,or_greater,suffix:m"), "set_height", "get_height");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "thickness", PROPERTY_HINT_RANGE, "0,4096,0.001,or_greater,suffix:m"), "set_thickness", "get_thickness");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "angle", PROPERTY_HINT_RANGE, "-360,360,0.1,radians_as_degrees"), "set_angle", "get_angle");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "angle2", PROPERTY_HINT_RANGE, "-360,360,0.1,radians_as_degrees"), "set_angle2", "get_angle2");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "roundness", PROPERTY_HINT_RANGE, "0,4096,0.001,or_greater,suffix:m"), "set_roundness", "get_roundness");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "sdf_material_override", PROPERTY_HINT_RESOURCE_TYPE, "SDFMaterial3D"), "set_sdf_material_override", "get_sdf_material_override");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "opacity", PROPERTY_HINT_RANGE, "0,1,0.001"), "set_opacity", "get_opacity");

	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_SPHERE);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_BOX);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_ROUNDED_BOX);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_TORUS);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_CAPPED_TORUS);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_LINK);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_CAPSULE);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_CYLINDER);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_CAPPED_CYLINDER);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_CONE);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_CAPPED_CONE);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_ROUND_CONE);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_PLANE);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_HEX_PRISM);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_TRI_PRISM);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_ELLIPSOID);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_CUT_SPHERE);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_CUT_HOLLOW_SPHERE);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_DEATH_STAR);
	BIND_ENUM_CONSTANT(SDF_PRIMITIVE_SOLID_ANGLE);

	BIND_ENUM_CONSTANT(SDF_OPERATION_UNION);
	BIND_ENUM_CONSTANT(SDF_OPERATION_SUBTRACT);
	BIND_ENUM_CONSTANT(SDF_OPERATION_INTERSECT);
	BIND_ENUM_CONSTANT(SDF_OPERATION_SMOOTH_UNION);
	BIND_ENUM_CONSTANT(SDF_OPERATION_SMOOTH_SUBTRACT);
	BIND_ENUM_CONSTANT(SDF_OPERATION_SMOOTH_INTERSECT);
}

bool SDFShape3D::_uses_smooth_operation() const {
	return operation == SDF_OPERATION_SMOOTH_UNION || operation == SDF_OPERATION_SMOOTH_SUBTRACT || operation == SDF_OPERATION_SMOOTH_INTERSECT;
}

void SDFShape3D::_validate_property(PropertyInfo &p_property) const {
	if (p_property.name == "smoothness" && !_uses_smooth_operation()) {
		p_property.usage = PROPERTY_USAGE_NONE;
	}
	if (p_property.name == "affect_layers" && operation == SDF_OPERATION_UNION) {
		p_property.usage = PROPERTY_USAGE_NONE;
	}

	const bool uses_size = shape_type == SDF_PRIMITIVE_BOX || shape_type == SDF_PRIMITIVE_ROUNDED_BOX || shape_type == SDF_PRIMITIVE_ELLIPSOID || shape_type == SDF_PRIMITIVE_HEX_PRISM || shape_type == SDF_PRIMITIVE_TRI_PRISM || shape_type == SDF_PRIMITIVE_PLANE;
	const bool uses_axis = shape_type == SDF_PRIMITIVE_PLANE;
	const bool uses_radius = shape_type == SDF_PRIMITIVE_SPHERE || shape_type == SDF_PRIMITIVE_TORUS || shape_type == SDF_PRIMITIVE_CAPPED_TORUS || shape_type == SDF_PRIMITIVE_LINK || shape_type == SDF_PRIMITIVE_CAPSULE || shape_type == SDF_PRIMITIVE_CYLINDER || shape_type == SDF_PRIMITIVE_CAPPED_CYLINDER || shape_type == SDF_PRIMITIVE_CAPPED_CONE || shape_type == SDF_PRIMITIVE_ROUND_CONE || shape_type == SDF_PRIMITIVE_CUT_SPHERE || shape_type == SDF_PRIMITIVE_CUT_HOLLOW_SPHERE || shape_type == SDF_PRIMITIVE_DEATH_STAR || shape_type == SDF_PRIMITIVE_SOLID_ANGLE;
	const bool uses_secondary_radius = shape_type == SDF_PRIMITIVE_TORUS || shape_type == SDF_PRIMITIVE_CAPPED_TORUS || shape_type == SDF_PRIMITIVE_LINK || shape_type == SDF_PRIMITIVE_CAPPED_CONE || shape_type == SDF_PRIMITIVE_ROUND_CONE || shape_type == SDF_PRIMITIVE_DEATH_STAR;
	const bool uses_height = shape_type == SDF_PRIMITIVE_CAPSULE || shape_type == SDF_PRIMITIVE_CYLINDER || shape_type == SDF_PRIMITIVE_CAPPED_CYLINDER || shape_type == SDF_PRIMITIVE_CONE || shape_type == SDF_PRIMITIVE_CAPPED_CONE || shape_type == SDF_PRIMITIVE_ROUND_CONE || shape_type == SDF_PRIMITIVE_CUT_SPHERE || shape_type == SDF_PRIMITIVE_CUT_HOLLOW_SPHERE || shape_type == SDF_PRIMITIVE_LINK;
	const bool uses_thickness = shape_type == SDF_PRIMITIVE_LINK || shape_type == SDF_PRIMITIVE_CUT_HOLLOW_SPHERE;
	const bool uses_angle = shape_type == SDF_PRIMITIVE_CAPPED_TORUS || shape_type == SDF_PRIMITIVE_CONE || shape_type == SDF_PRIMITIVE_SOLID_ANGLE;
	const bool uses_angle2 = shape_type == SDF_PRIMITIVE_CAPPED_TORUS;
	const bool uses_roundness = shape_type == SDF_PRIMITIVE_ROUNDED_BOX || shape_type == SDF_PRIMITIVE_ROUND_CONE;

	if (p_property.name == "size" && !uses_size) {
		p_property.usage = PROPERTY_USAGE_NONE;
	}
	if (p_property.name == "axis" && !uses_axis) {
		p_property.usage = PROPERTY_USAGE_NONE;
	}
	if (p_property.name == "radius" && !uses_radius) {
		p_property.usage = PROPERTY_USAGE_NONE;
	}
	if (p_property.name == "secondary_radius" && !uses_secondary_radius) {
		p_property.usage = PROPERTY_USAGE_NONE;
	}
	if (p_property.name == "height" && !uses_height) {
		p_property.usage = PROPERTY_USAGE_NONE;
	}
	if (p_property.name == "thickness" && !uses_thickness) {
		p_property.usage = PROPERTY_USAGE_NONE;
	}
	if (p_property.name == "angle" && !uses_angle) {
		p_property.usage = PROPERTY_USAGE_NONE;
	}
	if (p_property.name == "angle2" && !uses_angle2) {
		p_property.usage = PROPERTY_USAGE_NONE;
	}
	if (p_property.name == "roundness" && !uses_roundness) {
		p_property.usage = PROPERTY_USAGE_NONE;
	}
}

void SDFShape3D::_notify_owner_sdf_object() {
	Node *n = get_parent();
	while (n) {
		SDFObject3D *sdf_object = Object::cast_to<SDFObject3D>(n);
		if (sdf_object) {
			sdf_object->_request_rebuild_from_child();
			return;
		}
		n = n->get_parent();
	}
}

void SDFShape3D::_sdf_material_changed() {
	_notify_owner_sdf_object();
}

void SDFShape3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_TRANSFORM_CHANGED:
		case NOTIFICATION_ENTER_TREE:
		case NOTIFICATION_EXIT_TREE:
		case NOTIFICATION_PARENTED:
		case NOTIFICATION_UNPARENTED: {
			_notify_owner_sdf_object();
		} break;
	}
}

void SDFShape3D::set_shape_type(SDFPrimitiveType p_shape_type) {
	if (shape_type == p_shape_type) {
		return;
	}
	shape_type = p_shape_type;
	notify_property_list_changed();
	_notify_owner_sdf_object();
}

SDFShape3D::SDFPrimitiveType SDFShape3D::get_shape_type() const {
	return shape_type;
}

void SDFShape3D::set_operation(SDFOperation p_operation) {
	if (operation == p_operation) {
		return;
	}
	operation = p_operation;
	notify_property_list_changed();
	_notify_owner_sdf_object();
}

SDFShape3D::SDFOperation SDFShape3D::get_operation() const {
	return operation;
}

void SDFShape3D::set_operation_order(int p_operation_order) {
	if (operation_order == p_operation_order) {
		return;
	}
	operation_order = p_operation_order;
	_notify_owner_sdf_object();
}

int SDFShape3D::get_operation_order() const {
	return operation_order;
}

void SDFShape3D::set_render_mode_hint(RSE::SDFRenderMode p_mode) {
	if (render_mode_hint == p_mode) {
		return;
	}
	render_mode_hint = p_mode;
	_notify_owner_sdf_object();
}

RSE::SDFRenderMode SDFShape3D::get_render_mode_hint() const {
	return render_mode_hint;
}

void SDFShape3D::set_smoothness(float p_smoothness) {
	smoothness = MAX(0.001f, p_smoothness);
	_notify_owner_sdf_object();
}

float SDFShape3D::get_smoothness() const {
	return smoothness;
}

void SDFShape3D::set_membership_layers(uint32_t p_layers) {
	membership_layers = p_layers;
	_notify_owner_sdf_object();
}

uint32_t SDFShape3D::get_membership_layers() const {
	return membership_layers;
}

void SDFShape3D::set_membership_layer_value(int p_layer_number, bool p_value) {
	ERR_FAIL_COND(p_layer_number < 1 || p_layer_number > 32);
	if (p_value) {
		membership_layers |= 1u << (p_layer_number - 1);
	} else {
		membership_layers &= ~(1u << (p_layer_number - 1));
	}
	_notify_owner_sdf_object();
}

bool SDFShape3D::get_membership_layer_value(int p_layer_number) const {
	ERR_FAIL_COND_V(p_layer_number < 1 || p_layer_number > 32, false);
	return (membership_layers & (1u << (p_layer_number - 1))) != 0;
}

void SDFShape3D::set_affect_layers(uint32_t p_layers) {
	affect_layers = p_layers;
	_notify_owner_sdf_object();
}

uint32_t SDFShape3D::get_affect_layers() const {
	return affect_layers;
}

void SDFShape3D::set_affect_layer_value(int p_layer_number, bool p_value) {
	ERR_FAIL_COND(p_layer_number < 1 || p_layer_number > 32);
	if (p_value) {
		affect_layers |= 1u << (p_layer_number - 1);
	} else {
		affect_layers &= ~(1u << (p_layer_number - 1));
	}
	_notify_owner_sdf_object();
}

bool SDFShape3D::get_affect_layer_value(int p_layer_number) const {
	ERR_FAIL_COND_V(p_layer_number < 1 || p_layer_number > 32, false);
	return (affect_layers & (1u << (p_layer_number - 1))) != 0;
}

void SDFShape3D::set_size(const Vector3 &p_size) {
	size = p_size.abs().maxf(0.001f);
	_notify_owner_sdf_object();
}

Vector3 SDFShape3D::get_size() const {
	return size;
}

void SDFShape3D::set_axis(const Vector3 &p_axis) {
	if (p_axis.is_zero_approx()) {
		axis = Vector3(0, 1, 0);
	} else {
		axis = p_axis.normalized();
	}
	_notify_owner_sdf_object();
}

Vector3 SDFShape3D::get_axis() const {
	return axis;
}

void SDFShape3D::set_radius(float p_radius) {
	radius = MAX(0.001f, p_radius);
	_notify_owner_sdf_object();
}

float SDFShape3D::get_radius() const {
	return radius;
}

void SDFShape3D::set_secondary_radius(float p_radius) {
	secondary_radius = MAX(0.001f, p_radius);
	_notify_owner_sdf_object();
}

float SDFShape3D::get_secondary_radius() const {
	return secondary_radius;
}

void SDFShape3D::set_height(float p_height) {
	height = MAX(0.001f, p_height);
	_notify_owner_sdf_object();
}

float SDFShape3D::get_height() const {
	return height;
}

void SDFShape3D::set_thickness(float p_thickness) {
	thickness = MAX(0.0f, p_thickness);
	_notify_owner_sdf_object();
}

float SDFShape3D::get_thickness() const {
	return thickness;
}

void SDFShape3D::set_angle(float p_angle) {
	angle = p_angle;
	_notify_owner_sdf_object();
}

float SDFShape3D::get_angle() const {
	return angle;
}

void SDFShape3D::set_angle2(float p_angle) {
	angle2 = p_angle;
	_notify_owner_sdf_object();
}

float SDFShape3D::get_angle2() const {
	return angle2;
}

void SDFShape3D::set_roundness(float p_roundness) {
	roundness = MAX(0.0f, p_roundness);
	_notify_owner_sdf_object();
}

float SDFShape3D::get_roundness() const {
	return roundness;
}

void SDFShape3D::set_sdf_material_override(const Ref<SDFMaterial3D> &p_material) {
	if (sdf_material_override == p_material) {
		return;
	}

	if (sdf_material_override.is_valid()) {
		sdf_material_override->disconnect_changed(callable_mp(this, &SDFShape3D::_sdf_material_changed));
	}

	sdf_material_override = p_material;

	if (sdf_material_override.is_valid()) {
		sdf_material_override->connect_changed(callable_mp(this, &SDFShape3D::_sdf_material_changed), CONNECT_REFERENCE_COUNTED);
	}

	_notify_owner_sdf_object();
}

Ref<SDFMaterial3D> SDFShape3D::get_sdf_material_override() const {
	return sdf_material_override;
}

void SDFShape3D::set_color(const Color &p_color) {
	color = p_color;
	_notify_owner_sdf_object();
}

Color SDFShape3D::get_color() const {
	return color;
}

void SDFShape3D::set_opacity(float p_opacity) {
	opacity = CLAMP(p_opacity, 0.0f, 1.0f);
	_notify_owner_sdf_object();
}

float SDFShape3D::get_opacity() const {
	return opacity;
}

AABB SDFShape3D::get_estimated_local_aabb() const {
	Vector3 extents = Vector3(0.5, 0.5, 0.5);
	switch (shape_type) {
		case SDF_PRIMITIVE_SPHERE:
			extents = Vector3(radius, radius, radius);
			break;
		case SDF_PRIMITIVE_BOX:
			extents = size * 0.5f;
			break;
		case SDF_PRIMITIVE_ROUNDED_BOX:
			extents = size * 0.5f + Vector3(roundness, roundness, roundness);
			break;
		case SDF_PRIMITIVE_TORUS:
		case SDF_PRIMITIVE_CAPPED_TORUS:
		case SDF_PRIMITIVE_LINK:
			extents = Vector3(radius + secondary_radius, secondary_radius + height * 0.5f, radius + secondary_radius);
			break;
		case SDF_PRIMITIVE_CAPSULE:
		case SDF_PRIMITIVE_CYLINDER:
		case SDF_PRIMITIVE_CAPPED_CYLINDER:
			extents = Vector3(radius, height * 0.5f + radius, radius);
			break;
		case SDF_PRIMITIVE_CONE:
		case SDF_PRIMITIVE_CAPPED_CONE:
		case SDF_PRIMITIVE_ROUND_CONE:
			extents = Vector3(MAX(radius, secondary_radius), height * 0.5f + roundness, MAX(radius, secondary_radius));
			break;
		case SDF_PRIMITIVE_PLANE:
			extents = size * 0.5f;
			if (extents.y < 0.01f) {
				extents.y = 0.01f;
			}
			break;
		case SDF_PRIMITIVE_HEX_PRISM:
		case SDF_PRIMITIVE_TRI_PRISM:
		case SDF_PRIMITIVE_ELLIPSOID:
			extents = size * 0.5f;
			break;
		case SDF_PRIMITIVE_CUT_SPHERE:
		case SDF_PRIMITIVE_CUT_HOLLOW_SPHERE:
		case SDF_PRIMITIVE_DEATH_STAR:
		case SDF_PRIMITIVE_SOLID_ANGLE:
			extents = Vector3(radius, radius, radius);
			break;
	}
	return AABB(-extents, extents * 2.0f);
}

void SDFShape3D::append_compiled_data(PackedInt32Array &r_int_data, PackedFloat32Array &r_float_data, const Transform3D &p_object_local_transform) const {
	r_int_data.push_back(int32_t(shape_type));
	r_int_data.push_back(int32_t(operation));
	r_int_data.push_back(int32_t(render_mode_hint));
	r_int_data.push_back(int32_t(membership_layers));
	r_int_data.push_back(int32_t(affect_layers));
	r_int_data.push_back(int32_t(operation_order)); // Reserved/ordering.

	const Basis &b = p_object_local_transform.basis;
	const Vector3 &o = p_object_local_transform.origin;
	r_float_data.push_back(b.rows[0][0]);
	r_float_data.push_back(b.rows[0][1]);
	r_float_data.push_back(b.rows[0][2]);
	r_float_data.push_back(o.x);
	r_float_data.push_back(b.rows[1][0]);
	r_float_data.push_back(b.rows[1][1]);
	r_float_data.push_back(b.rows[1][2]);
	r_float_data.push_back(o.y);
	r_float_data.push_back(b.rows[2][0]);
	r_float_data.push_back(b.rows[2][1]);
	r_float_data.push_back(b.rows[2][2]);
	r_float_data.push_back(o.z);
	r_float_data.push_back(size.x);
	r_float_data.push_back(size.y);
	r_float_data.push_back(size.z);
	r_float_data.push_back(axis.x);
	r_float_data.push_back(axis.y);
	r_float_data.push_back(axis.z);
	r_float_data.push_back(radius);
	r_float_data.push_back(secondary_radius);
	r_float_data.push_back(height);
	r_float_data.push_back(thickness);
	r_float_data.push_back(angle);
	r_float_data.push_back(angle2);
	r_float_data.push_back(roundness);
	r_float_data.push_back(smoothness);
	r_float_data.push_back(color.r);
	r_float_data.push_back(color.g);
	r_float_data.push_back(color.b);
	r_float_data.push_back(color.a * opacity);
	r_float_data.push_back(0.0f);
	r_float_data.push_back(0.0f);
}

SDFShape3D::SDFShape3D() {
	set_notify_transform(true);
}

SDFShape3D::~SDFShape3D() {
	if (sdf_material_override.is_valid()) {
		sdf_material_override->disconnect_changed(callable_mp(this, &SDFShape3D::_sdf_material_changed));
	}
}
