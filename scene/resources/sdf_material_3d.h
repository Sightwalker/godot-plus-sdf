/**************************************************************************/
/*  sdf_material_3d.h                                                     */
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

#include "scene/resources/material.h"

class SDFMaterial3D : public Resource {
	GDCLASS(SDFMaterial3D, Resource);

	Ref<Material> shading_material;
	Color albedo = Color(1, 1, 1, 1);
	Color emission = Color(0, 0, 0, 1);
	float emission_energy = 1.0f;
	float metallic = 0.0f;
	float roughness = 1.0f;
	float alpha = 1.0f;

protected:
	static void _bind_methods();

public:
	void set_shading_material(const Ref<Material> &p_material);
	Ref<Material> get_shading_material() const;

	void set_albedo(const Color &p_color);
	Color get_albedo() const;

	void set_emission(const Color &p_color);
	Color get_emission() const;

	void set_emission_energy(float p_energy);
	float get_emission_energy() const;

	void set_metallic(float p_metallic);
	float get_metallic() const;

	void set_roughness(float p_roughness);
	float get_roughness() const;

	void set_alpha(float p_alpha);
	float get_alpha() const;

	RID get_material_rid() const;
};

