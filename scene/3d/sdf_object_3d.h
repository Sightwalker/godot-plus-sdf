/**************************************************************************/
/*  sdf_object_3d.h                                                       */
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

#include "scene/3d/visual_instance_3d.h"
#include "scene/resources/sdf_material_3d.h"

class SDFShape3D;

class SDFObject3D : public GeometryInstance3D {
	GDCLASS(SDFObject3D, GeometryInstance3D);

	friend class SDFShape3D;

private:
	RID sdf_object;
	Ref<SDFMaterial3D> sdf_material;
	RSE::SDFRenderMode render_mode = RSE::SDF_RENDER_MODE_STATIC;
	bool rebuild_queued = false;

	AABB cached_aabb = AABB(Vector3(-0.05, -0.05, -0.05), Vector3(0.1, 0.1, 0.1));
	int cached_shape_count = 0;

	void _collect_shapes_recursive(Node *p_root, Vector<SDFShape3D *> &r_shapes) const;
	void _rebuild_compiled_data();
	void _rebuild_compiled_data_deferred();
	void _sdf_material_changed();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void _request_rebuild_from_child();

	void set_sdf_material(const Ref<SDFMaterial3D> &p_material);
	Ref<SDFMaterial3D> get_sdf_material() const;

	void set_render_mode(RSE::SDFRenderMode p_mode);
	RSE::SDFRenderMode get_render_mode() const;

	virtual AABB get_aabb() const override;
	virtual PackedStringArray get_configuration_warnings() const override;

	SDFObject3D();
	~SDFObject3D();
};
