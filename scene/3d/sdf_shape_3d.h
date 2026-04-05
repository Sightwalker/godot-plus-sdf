/**************************************************************************/
/*  sdf_shape_3d.h                                                        */
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

#pragma once

#include "scene/3d/node_3d.h"
#include "scene/resources/sdf_material_3d.h"
#include "servers/rendering/rendering_server_enums.h"

class SDFShape3D : public Node3D {
	GDCLASS(SDFShape3D, Node3D);

public:
	enum SDFPrimitiveType {
		SDF_PRIMITIVE_SPHERE,
		SDF_PRIMITIVE_BOX,
		SDF_PRIMITIVE_ROUNDED_BOX,
		SDF_PRIMITIVE_TORUS,
		SDF_PRIMITIVE_CAPPED_TORUS,
		SDF_PRIMITIVE_LINK,
		SDF_PRIMITIVE_CAPSULE,
		SDF_PRIMITIVE_CYLINDER,
		SDF_PRIMITIVE_CAPPED_CYLINDER,
		SDF_PRIMITIVE_CONE,
		SDF_PRIMITIVE_CAPPED_CONE,
		SDF_PRIMITIVE_ROUND_CONE,
		SDF_PRIMITIVE_PLANE,
		SDF_PRIMITIVE_HEX_PRISM,
		SDF_PRIMITIVE_TRI_PRISM,
		SDF_PRIMITIVE_ELLIPSOID,
		SDF_PRIMITIVE_CUT_SPHERE,
		SDF_PRIMITIVE_CUT_HOLLOW_SPHERE,
		SDF_PRIMITIVE_DEATH_STAR,
		SDF_PRIMITIVE_SOLID_ANGLE
	};

	enum SDFOperation {
		SDF_OPERATION_UNION,
		SDF_OPERATION_SUBTRACT,
		SDF_OPERATION_INTERSECT,
		SDF_OPERATION_SMOOTH_UNION,
		SDF_OPERATION_SMOOTH_SUBTRACT,
		SDF_OPERATION_SMOOTH_INTERSECT
	};

private:
	SDFPrimitiveType shape_type = SDF_PRIMITIVE_SPHERE;
	SDFOperation operation = SDF_OPERATION_UNION;
	int operation_order = 0;
	RSE::SDFRenderMode render_mode_hint = RSE::SDF_RENDER_MODE_STATIC;

	float smoothness = 0.15f;
	uint32_t membership_layers = 1;
	uint32_t affect_layers = 0xFFFFFFFFu;

	Vector3 size = Vector3(1, 1, 1);
	Vector3 axis = Vector3(0, 1, 0);
	float radius = 0.5f;
	float secondary_radius = 0.25f;
	float height = 1.0f;
	float thickness = 0.1f;
	float angle = 0.785398163f;
	float angle2 = 0.523598776f;
	float roundness = 0.1f;

	Ref<SDFMaterial3D> sdf_material_override;
	Color color = Color(1, 1, 1, 1);
	float opacity = 1.0f;

	void _notify_owner_sdf_object();
	void _sdf_material_changed();
	bool _uses_smooth_operation() const;

protected:
	static void _bind_methods();
	void _notification(int p_what);
	void _validate_property(PropertyInfo &p_property) const;

public:
	void set_shape_type(SDFPrimitiveType p_shape_type);
	SDFPrimitiveType get_shape_type() const;

	void set_operation(SDFOperation p_operation);
	SDFOperation get_operation() const;

	void set_operation_order(int p_operation_order);
	int get_operation_order() const;

	void set_render_mode_hint(RSE::SDFRenderMode p_mode);
	RSE::SDFRenderMode get_render_mode_hint() const;

	void set_smoothness(float p_smoothness);
	float get_smoothness() const;

	void set_membership_layers(uint32_t p_layers);
	uint32_t get_membership_layers() const;
	void set_membership_layer_value(int p_layer_number, bool p_value);
	bool get_membership_layer_value(int p_layer_number) const;

	void set_affect_layers(uint32_t p_layers);
	uint32_t get_affect_layers() const;
	void set_affect_layer_value(int p_layer_number, bool p_value);
	bool get_affect_layer_value(int p_layer_number) const;

	void set_size(const Vector3 &p_size);
	Vector3 get_size() const;

	void set_axis(const Vector3 &p_axis);
	Vector3 get_axis() const;

	void set_radius(float p_radius);
	float get_radius() const;

	void set_secondary_radius(float p_radius);
	float get_secondary_radius() const;

	void set_height(float p_height);
	float get_height() const;

	void set_thickness(float p_thickness);
	float get_thickness() const;

	void set_angle(float p_angle);
	float get_angle() const;

	void set_angle2(float p_angle);
	float get_angle2() const;

	void set_roundness(float p_roundness);
	float get_roundness() const;

	void set_sdf_material_override(const Ref<SDFMaterial3D> &p_material);
	Ref<SDFMaterial3D> get_sdf_material_override() const;

	void set_color(const Color &p_color);
	Color get_color() const;

	void set_opacity(float p_opacity);
	float get_opacity() const;

	AABB get_estimated_local_aabb() const;
	void append_compiled_data(PackedInt32Array &r_int_data, PackedFloat32Array &r_float_data, const Transform3D &p_object_local_transform) const;

	SDFShape3D();
	~SDFShape3D();
};

VARIANT_ENUM_CAST(SDFShape3D::SDFPrimitiveType);
VARIANT_ENUM_CAST(SDFShape3D::SDFOperation);
