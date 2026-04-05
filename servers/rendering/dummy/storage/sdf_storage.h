/**************************************************************************/
/*  sdf_storage.h                                                         */
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

#include "core/templates/rid_owner.h"
#include "servers/rendering/storage/sdf_storage.h"

namespace RendererDummy {

class SDFStorage : public RendererSDFStorage {
private:
	struct SDFObject {
		AABB bounds = AABB(Vector3(-1, -1, -1), Vector3(2, 2, 2));
		PackedInt32Array int_data;
		PackedFloat32Array float_data;
		RID material;
		RSE::SDFRenderMode render_mode = RSE::SDF_RENDER_MODE_STATIC;
		int operation = 0;
		int operation_order = 0;
		float smoothness = 0.15f;
		uint32_t membership_layers = 1;
		uint32_t affect_layers = 0xFFFFFFFFu;
		int inside_render_mode = 1;
		Dependency dependency;
	};

	static SDFStorage *singleton;
	mutable RID_Owner<SDFObject> sdf_object_owner;

public:
	static SDFStorage *get_singleton() { return singleton; }

	SDFStorage();
	~SDFStorage();

	virtual RID sdf_object_allocate() override;
	virtual void sdf_object_initialize(RID p_rid) override;
	virtual void sdf_object_free(RID p_rid) override;
	virtual bool owns_sdf_object(RID p_rid) const override;

	virtual void sdf_object_set_compiled_data(RID p_sdf_object, const PackedInt32Array &p_int_data, const PackedFloat32Array &p_float_data) override;
	virtual void sdf_object_set_bounds(RID p_sdf_object, const AABB &p_bounds) override;
	virtual AABB sdf_object_get_bounds(RID p_sdf_object) const override;
	virtual void sdf_object_set_material(RID p_sdf_object, RID p_material) override;
	virtual RID sdf_object_get_material(RID p_sdf_object) const override;
	virtual void sdf_object_set_render_mode(RID p_sdf_object, RSE::SDFRenderMode p_mode) override;
	virtual RSE::SDFRenderMode sdf_object_get_render_mode(RID p_sdf_object) const override;
	virtual void sdf_object_set_operation_data(RID p_sdf_object, int p_operation, int p_operation_order, float p_smoothness, uint32_t p_membership_layers, uint32_t p_affect_layers, int p_inside_render_mode) override;
	virtual int sdf_object_get_operation(RID p_sdf_object) const override;
	virtual int sdf_object_get_operation_order(RID p_sdf_object) const override;
	virtual float sdf_object_get_smoothness(RID p_sdf_object) const override;
	virtual uint32_t sdf_object_get_membership_layers(RID p_sdf_object) const override;
	virtual uint32_t sdf_object_get_affect_layers(RID p_sdf_object) const override;
	virtual int sdf_object_get_inside_render_mode(RID p_sdf_object) const override;
	virtual PackedInt32Array sdf_object_get_compiled_int_data(RID p_sdf_object) const override;
	virtual PackedFloat32Array sdf_object_get_compiled_float_data(RID p_sdf_object) const override;
	virtual RID sdf_object_get_compiled_int_buffer(RID p_sdf_object) const override;
	virtual RID sdf_object_get_compiled_float_buffer(RID p_sdf_object) const override;
	virtual uint32_t sdf_object_get_shape_count(RID p_sdf_object) const override;

	virtual Dependency *sdf_object_get_dependency(RID p_sdf_object) const override;
};

} // namespace RendererDummy
