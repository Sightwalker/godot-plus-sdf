/**************************************************************************/
/*  sdf_material_3d.cpp                                                   */
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

#include "sdf_material_3d.h"

#include "core/object/class_db.h"

void SDFMaterial3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_shading_material", "material"), &SDFMaterial3D::set_shading_material);
	ClassDB::bind_method(D_METHOD("get_shading_material"), &SDFMaterial3D::get_shading_material);

	ClassDB::bind_method(D_METHOD("set_albedo", "color"), &SDFMaterial3D::set_albedo);
	ClassDB::bind_method(D_METHOD("get_albedo"), &SDFMaterial3D::get_albedo);
	ClassDB::bind_method(D_METHOD("set_emission", "color"), &SDFMaterial3D::set_emission);
	ClassDB::bind_method(D_METHOD("get_emission"), &SDFMaterial3D::get_emission);
	ClassDB::bind_method(D_METHOD("set_emission_energy", "energy"), &SDFMaterial3D::set_emission_energy);
	ClassDB::bind_method(D_METHOD("get_emission_energy"), &SDFMaterial3D::get_emission_energy);
	ClassDB::bind_method(D_METHOD("set_metallic", "metallic"), &SDFMaterial3D::set_metallic);
	ClassDB::bind_method(D_METHOD("get_metallic"), &SDFMaterial3D::get_metallic);
	ClassDB::bind_method(D_METHOD("set_roughness", "roughness"), &SDFMaterial3D::set_roughness);
	ClassDB::bind_method(D_METHOD("get_roughness"), &SDFMaterial3D::get_roughness);
	ClassDB::bind_method(D_METHOD("set_alpha", "alpha"), &SDFMaterial3D::set_alpha);
	ClassDB::bind_method(D_METHOD("get_alpha"), &SDFMaterial3D::get_alpha);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shading_material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), "set_shading_material", "get_shading_material");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "albedo"), "set_albedo", "get_albedo");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "emission"), "set_emission", "get_emission");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "emission_energy", PROPERTY_HINT_RANGE, "0,128,0.01,or_greater"), "set_emission_energy", "get_emission_energy");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "metallic", PROPERTY_HINT_RANGE, "0,1,0.001"), "set_metallic", "get_metallic");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "roughness", PROPERTY_HINT_RANGE, "0,1,0.001"), "set_roughness", "get_roughness");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "alpha", PROPERTY_HINT_RANGE, "0,1,0.001"), "set_alpha", "get_alpha");
}

void SDFMaterial3D::set_shading_material(const Ref<Material> &p_material) {
	shading_material = p_material;
	emit_changed();
}

Ref<Material> SDFMaterial3D::get_shading_material() const {
	return shading_material;
}

void SDFMaterial3D::set_albedo(const Color &p_color) {
	albedo = p_color;
	emit_changed();
}

Color SDFMaterial3D::get_albedo() const {
	return albedo;
}

void SDFMaterial3D::set_emission(const Color &p_color) {
	emission = p_color;
	emit_changed();
}

Color SDFMaterial3D::get_emission() const {
	return emission;
}

void SDFMaterial3D::set_emission_energy(float p_energy) {
	emission_energy = p_energy;
	emit_changed();
}

float SDFMaterial3D::get_emission_energy() const {
	return emission_energy;
}

void SDFMaterial3D::set_metallic(float p_metallic) {
	metallic = CLAMP(p_metallic, 0.0f, 1.0f);
	emit_changed();
}

float SDFMaterial3D::get_metallic() const {
	return metallic;
}

void SDFMaterial3D::set_roughness(float p_roughness) {
	roughness = CLAMP(p_roughness, 0.0f, 1.0f);
	emit_changed();
}

float SDFMaterial3D::get_roughness() const {
	return roughness;
}

void SDFMaterial3D::set_alpha(float p_alpha) {
	alpha = CLAMP(p_alpha, 0.0f, 1.0f);
	emit_changed();
}

float SDFMaterial3D::get_alpha() const {
	return alpha;
}

RID SDFMaterial3D::get_material_rid() const {
	if (shading_material.is_valid()) {
		return shading_material->get_rid();
	}
	return RID();
}

