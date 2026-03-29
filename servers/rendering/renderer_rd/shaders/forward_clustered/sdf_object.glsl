#[vertex]

#version 450

#VERSION_DEFINES

layout(location = 0) out vec2 uv_interp;

void main() {
	const vec2 vertices[3] = vec2[](
			vec2(-1.0, -1.0),
			vec2(3.0, -1.0),
			vec2(-1.0, 3.0));

	vec2 vertex = vertices[gl_VertexIndex];
	uv_interp = vertex * 0.5 + 0.5;
	gl_Position = vec4(vertex, 0.0, 1.0);
}

#[fragment]

#version 450

#VERSION_DEFINES

layout(location = 0) in vec2 uv_interp;
layout(location = 0) out vec4 frag_color;

layout(set = 0, binding = 0, std140) uniform SDFObjectUniforms {
	mat4 view_projection;
	mat4 inv_view;
	mat4 inv_projection;
	mat4 object_to_world;
	mat4 world_to_object;
	vec4 bounds_min;
	vec4 bounds_max;
	uvec4 data_info;
	vec4 march_info;
}
sdf_uniforms;

layout(set = 1, binding = 0, std430) readonly buffer SDFIntData {
	int data[];
}
sdf_int_data;

layout(set = 1, binding = 1, std430) readonly buffer SDFFloatData {
	float data[];
}
sdf_float_data;

const int SHAPE_INT_HEADER_SIZE = 2;
const int SHAPE_INT_STRIDE = 6;
const int SHAPE_FLOAT_STRIDE = 32;

const int SDF_OP_UNION = 0;
const int SDF_OP_SUBTRACT = 1;
const int SDF_OP_INTERSECT = 2;
const int SDF_OP_SMOOTH_UNION = 3;
const int SDF_OP_SMOOTH_SUBTRACT = 4;
const int SDF_OP_SMOOTH_INTERSECT = 5;

const int SDF_SHAPE_SPHERE = 0;
const int SDF_SHAPE_BOX = 1;
const int SDF_SHAPE_ROUNDED_BOX = 2;
const int SDF_SHAPE_TORUS = 3;
const int SDF_SHAPE_CAPPED_TORUS = 4;
const int SDF_SHAPE_LINK = 5;
const int SDF_SHAPE_CAPSULE = 6;
const int SDF_SHAPE_CYLINDER = 7;
const int SDF_SHAPE_CAPPED_CYLINDER = 8;
const int SDF_SHAPE_CONE = 9;
const int SDF_SHAPE_CAPPED_CONE = 10;
const int SDF_SHAPE_ROUND_CONE = 11;
const int SDF_SHAPE_PLANE = 12;
const int SDF_SHAPE_HEX_PRISM = 13;
const int SDF_SHAPE_TRI_PRISM = 14;
const int SDF_SHAPE_ELLIPSOID = 15;
const int SDF_SHAPE_CUT_SPHERE = 16;
const int SDF_SHAPE_CUT_HOLLOW_SPHERE = 17;
const int SDF_SHAPE_DEATH_STAR = 18;
const int SDF_SHAPE_SOLID_ANGLE = 19;

struct ShapeData {
	int shape_type;
	int operation;
	uint membership_layers;
	uint affect_layers;
	mat3 basis;
	vec3 origin;
	vec3 size;
	vec3 axis;
	float radius;
	float secondary_radius;
	float height;
	float thickness;
	float angle;
	float angle2;
	float roundness;
	float smoothness;
	vec4 color;
};

struct EvalResult {
	float dist;
	vec4 color;
	uint layers;
};

float sd_sphere(vec3 p, float r) {
	return length(p) - r;
}

float sd_box(vec3 p, vec3 b) {
	vec3 q = abs(p) - b;
	return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sd_rounded_box(vec3 p, vec3 b, float r) {
	return sd_box(p, b) - r;
}

float sd_torus(vec3 p, vec2 t) {
	vec2 q = vec2(length(p.xz) - t.x, p.y);
	return length(q) - t.y;
}

float sd_capped_torus(vec3 p, vec2 sc, float ra, float rb) {
	p.x = abs(p.x);
	float k = (sc.y * p.x > sc.x * p.y) ? dot(p.xy, sc) : length(p.xy);
	return sqrt(max(dot(p, p) + ra * ra - 2.0 * ra * k, 0.0)) - rb;
}

float sd_link(vec3 p, float le, float r1, float r2) {
	vec3 q = vec3(p.x, max(abs(p.y) - le, 0.0), p.z);
	return length(vec2(length(q.xy) - r1, q.z)) - r2;
}

float sd_capsule(vec3 p, float h, float r) {
	vec3 a = vec3(0.0, -h, 0.0);
	vec3 b = vec3(0.0, h, 0.0);
	vec3 pa = p - a;
	vec3 ba = b - a;
	float t = clamp(dot(pa, ba) / max(dot(ba, ba), 1e-6), 0.0, 1.0);
	return length(pa - ba * t) - r;
}

float sd_capped_cylinder(vec3 p, float h, float r) {
	vec2 d = abs(vec2(length(p.xz), p.y)) - vec2(r, h);
	return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float sd_capped_cone(vec3 p, float h, float r1, float r2) {
	vec2 q = vec2(length(p.xz), p.y);
	vec2 k1 = vec2(r2, h);
	vec2 k2 = vec2(r2 - r1, 2.0 * h);
	vec2 ca = vec2(q.x - min(q.x, (q.y < 0.0) ? r1 : r2), abs(q.y) - h);
	vec2 cb = q - k1 + k2 * clamp(dot(k1 - q, k2) / max(dot(k2, k2), 1e-6), 0.0, 1.0);
	float s = (cb.x < 0.0 && ca.y < 0.0) ? -1.0 : 1.0;
	return s * sqrt(min(dot(ca, ca), dot(cb, cb)));
}

float sd_round_cone(vec3 p, float r1, float r2, float h) {
	vec2 q = vec2(length(p.xz), p.y);
	float b = (r1 - r2) / max(h, 1e-6);
	float a = sqrt(max(1.0 - b * b, 0.0));
	float k = dot(q, vec2(-b, a));

	if (k < 0.0) {
		return length(q) - r1;
	}
	if (k > a * h) {
		return length(q - vec2(0.0, h)) - r2;
	}
	return dot(q, vec2(a, b)) - r1;
}

float sd_hex_prism(vec3 p, vec2 h) {
	vec3 q = abs(p);
	return max(q.z - h.y, max(q.x * 0.866025 + q.y * 0.5, q.y) - h.x);
}

float sd_tri_prism(vec3 p, vec2 h) {
	vec3 q = abs(p);
	return max(q.z - h.y, max(q.x * 0.866025 + p.y * 0.5, -p.y) - h.x * 0.5);
}

float sd_ellipsoid(vec3 p, vec3 r) {
	vec3 rr = max(r, vec3(1e-4));
	float k0 = length(p / rr);
	float k1 = length(p / (rr * rr));
	return k0 * (k0 - 1.0) / max(k1, 1e-6);
}

float sd_cut_sphere(vec3 p, float r, float h) {
	float w = sqrt(max(r * r - h * h, 0.0));
	vec2 q = vec2(length(p.xz), p.y);
	float s = max((h - r) * q.x * q.x + w * w * (h + r - 2.0 * q.y), h * q.x - w * q.y);
	if (s < 0.0) {
		return length(q) - r;
	}
	if (q.x < w) {
		return h - q.y;
	}
	return length(q - vec2(w, h));
}

float sd_cut_hollow_sphere(vec3 p, float r, float h, float t) {
	float w = sqrt(max(r * r - h * h, 0.0));
	vec2 q = vec2(length(p.xz), p.y);
	return ((h * q.x < w * q.y) ? length(q - vec2(w, h)) : abs(length(q) - r)) - t;
}

float sd_death_star(vec3 p, float ra, float rb, float d) {
	float a = (ra * ra - rb * rb + d * d) / max(2.0 * d, 1e-6);
	float b = sqrt(max(ra * ra - a * a, 0.0));
	vec2 q = vec2(length(p.xz), p.y);
	if (q.x * b - q.y * a > d * max(b - q.y, 0.0)) {
		return length(q - vec2(a, b));
	}
	return max(length(q) - ra, -(length(q - vec2(d, 0.0)) - rb));
}

float sd_solid_angle(vec3 p, vec2 c, float ra) {
	vec2 q = vec2(length(p.xz), p.y);
	float l = length(q) - ra;
	float m = length(q - c * clamp(dot(q, c), 0.0, ra));
	return max(l, m * sign(c.y * q.x - c.x * q.y));
}

float op_smooth_union(float d_a, float d_b, float k) {
	float h = clamp(0.5 + 0.5 * (d_b - d_a) / k, 0.0, 1.0);
	return mix(d_b, d_a, h) - k * h * (1.0 - h);
}

float op_smooth_subtract(float d_a, float d_b, float k) {
	float h = clamp(0.5 - 0.5 * (d_b + d_a) / k, 0.0, 1.0);
	return mix(d_a, -d_b, h) + k * h * (1.0 - h);
}

float op_smooth_intersect(float d_a, float d_b, float k) {
	float h = clamp(0.5 - 0.5 * (d_b - d_a) / k, 0.0, 1.0);
	return mix(d_b, d_a, h) + k * h * (1.0 - h);
}

ShapeData load_shape(uint shape_index) {
	ShapeData s;
	int i_base = SHAPE_INT_HEADER_SIZE + int(shape_index) * SHAPE_INT_STRIDE;
	int f_base = int(shape_index) * SHAPE_FLOAT_STRIDE;

	s.shape_type = sdf_int_data.data[i_base + 0];
	s.operation = sdf_int_data.data[i_base + 1];
	s.membership_layers = uint(sdf_int_data.data[i_base + 3]);
	s.affect_layers = uint(sdf_int_data.data[i_base + 4]);

	float f0 = sdf_float_data.data[f_base + 0];
	float f1 = sdf_float_data.data[f_base + 1];
	float f2 = sdf_float_data.data[f_base + 2];
	float f3 = sdf_float_data.data[f_base + 3];
	float f4 = sdf_float_data.data[f_base + 4];
	float f5 = sdf_float_data.data[f_base + 5];
	float f6 = sdf_float_data.data[f_base + 6];
	float f7 = sdf_float_data.data[f_base + 7];
	float f8 = sdf_float_data.data[f_base + 8];
	float f9 = sdf_float_data.data[f_base + 9];
	float f10 = sdf_float_data.data[f_base + 10];
	float f11 = sdf_float_data.data[f_base + 11];

	s.basis = mat3(
			vec3(f0, f4, f8),
			vec3(f1, f5, f9),
			vec3(f2, f6, f10));
	s.origin = vec3(f3, f7, f11);

	s.size = vec3(sdf_float_data.data[f_base + 12], sdf_float_data.data[f_base + 13], sdf_float_data.data[f_base + 14]);
	s.axis = vec3(sdf_float_data.data[f_base + 15], sdf_float_data.data[f_base + 16], sdf_float_data.data[f_base + 17]);
	s.radius = sdf_float_data.data[f_base + 18];
	s.secondary_radius = sdf_float_data.data[f_base + 19];
	s.height = sdf_float_data.data[f_base + 20];
	s.thickness = sdf_float_data.data[f_base + 21];
	s.angle = sdf_float_data.data[f_base + 22];
	s.angle2 = sdf_float_data.data[f_base + 23];
	s.roundness = sdf_float_data.data[f_base + 24];
	s.smoothness = sdf_float_data.data[f_base + 25];
	s.color = vec4(
			sdf_float_data.data[f_base + 26],
			sdf_float_data.data[f_base + 27],
			sdf_float_data.data[f_base + 28],
			sdf_float_data.data[f_base + 29]);

	return s;
}

float evaluate_shape_distance(vec3 p_object, ShapeData shape) {
	mat3 inv_basis = inverse(shape.basis);
	vec3 p_local = inv_basis * (p_object - shape.origin);

	float dist = 0.0;
	vec3 extents = max(shape.size * 0.5, vec3(1e-4));
	float radius = max(shape.radius, 1e-4);
	float secondary_radius = max(shape.secondary_radius, 1e-4);
	float half_height = max(shape.height * 0.5, 1e-4);

	switch (shape.shape_type) {
		case SDF_SHAPE_SPHERE: {
			dist = sd_sphere(p_local, radius);
		} break;
		case SDF_SHAPE_BOX: {
			dist = sd_box(p_local, extents);
		} break;
		case SDF_SHAPE_ROUNDED_BOX: {
			dist = sd_rounded_box(p_local, extents, max(shape.roundness, 0.0));
		} break;
		case SDF_SHAPE_TORUS: {
			dist = sd_torus(p_local, vec2(radius, secondary_radius));
		} break;
		case SDF_SHAPE_CAPPED_TORUS: {
			vec2 sc = vec2(sin(shape.angle), cos(shape.angle));
			dist = sd_capped_torus(p_local, sc, radius, secondary_radius);
		} break;
		case SDF_SHAPE_LINK: {
			dist = sd_link(p_local, half_height, radius, max(shape.thickness, 1e-4));
		} break;
		case SDF_SHAPE_CAPSULE: {
			dist = sd_capsule(p_local, half_height, radius);
		} break;
		case SDF_SHAPE_CYLINDER:
		case SDF_SHAPE_CAPPED_CYLINDER: {
			dist = sd_capped_cylinder(p_local, half_height, radius);
		} break;
		case SDF_SHAPE_CONE:
		case SDF_SHAPE_CAPPED_CONE: {
			dist = sd_capped_cone(p_local, half_height, radius, secondary_radius);
		} break;
		case SDF_SHAPE_ROUND_CONE: {
			dist = sd_round_cone(p_local, radius, secondary_radius, max(shape.height, 1e-4));
		} break;
		case SDF_SHAPE_PLANE: {
			dist = sd_box(p_local, vec3(extents.x, max(extents.y, 0.01), extents.z));
		} break;
		case SDF_SHAPE_HEX_PRISM: {
			dist = sd_hex_prism(p_local, vec2(extents.x, extents.y));
		} break;
		case SDF_SHAPE_TRI_PRISM: {
			dist = sd_tri_prism(p_local, vec2(extents.x, extents.y));
		} break;
		case SDF_SHAPE_ELLIPSOID: {
			dist = sd_ellipsoid(p_local, extents);
		} break;
		case SDF_SHAPE_CUT_SPHERE: {
			dist = sd_cut_sphere(p_local, radius, clamp(shape.height * 0.5, -radius + 1e-4, radius - 1e-4));
		} break;
		case SDF_SHAPE_CUT_HOLLOW_SPHERE: {
			dist = sd_cut_hollow_sphere(p_local, radius, clamp(shape.height * 0.5, -radius + 1e-4, radius - 1e-4), max(shape.thickness, 1e-4));
		} break;
		case SDF_SHAPE_DEATH_STAR: {
			dist = sd_death_star(p_local, radius, secondary_radius, clamp(shape.height, 1e-4, radius + secondary_radius));
		} break;
		case SDF_SHAPE_SOLID_ANGLE: {
			vec2 c = vec2(sin(shape.angle), cos(shape.angle));
			dist = sd_solid_angle(p_local, c, radius);
		} break;
		default: {
			dist = sd_sphere(p_local, radius);
		}
	}

	float min_scale = min(length(shape.basis[0]), min(length(shape.basis[1]), length(shape.basis[2])));
	return dist * max(min_scale, 1e-4);
}

EvalResult evaluate_object(vec3 p_object) {
	EvalResult result;
	result.dist = 1e20;
	result.color = vec4(1.0, 1.0, 1.0, 1.0);
	result.layers = 0u;

	uint shape_count = sdf_uniforms.data_info.x;
	for (uint shape_index = 0u; shape_index < shape_count; shape_index++) {
		ShapeData shape = load_shape(shape_index);
		int operation = shape.operation;
		if (shape_index == 0u && operation != SDF_OP_UNION && operation != SDF_OP_SMOOTH_UNION) {
			operation = SDF_OP_UNION;
		}

		bool affects = operation == SDF_OP_UNION || operation == SDF_OP_SMOOTH_UNION || ((result.layers & shape.affect_layers) != 0u);
		if (!affects) {
			continue;
		}

		float d_shape = evaluate_shape_distance(p_object, shape);
		float d_prev = result.dist;
		vec4 color_prev = result.color;
		uint layers_prev = result.layers;
		float k = max(shape.smoothness, 1e-4);

		switch (operation) {
			case SDF_OP_UNION: {
				if (d_shape < result.dist) {
					result.color = shape.color;
					result.layers = shape.membership_layers;
				}
				result.dist = min(result.dist, d_shape);
			} break;
			case SDF_OP_SUBTRACT: {
				result.dist = max(result.dist, -d_shape);
				result.color = color_prev;
				result.layers = layers_prev;
			} break;
			case SDF_OP_INTERSECT: {
				if (d_shape > result.dist) {
					result.color = shape.color;
					result.layers = shape.membership_layers;
				}
				result.dist = max(result.dist, d_shape);
			} break;
			case SDF_OP_SMOOTH_UNION: {
				float h = clamp(0.5 + 0.5 * (d_shape - d_prev) / k, 0.0, 1.0);
				result.dist = op_smooth_union(d_prev, d_shape, k);
				result.color = mix(shape.color, color_prev, h);
				result.layers = (h < 0.5) ? shape.membership_layers : layers_prev;
			} break;
			case SDF_OP_SMOOTH_SUBTRACT: {
				float h = clamp(0.5 - 0.5 * (d_shape + d_prev) / k, 0.0, 1.0);
				result.dist = op_smooth_subtract(d_prev, d_shape, k);
				result.color = mix(color_prev, shape.color, h);
				result.layers = layers_prev;
			} break;
			case SDF_OP_SMOOTH_INTERSECT: {
				float h = clamp(0.5 - 0.5 * (d_shape - d_prev) / k, 0.0, 1.0);
				result.dist = op_smooth_intersect(d_prev, d_shape, k);
				result.color = mix(shape.color, color_prev, h);
				result.layers = (h < 0.5) ? shape.membership_layers : layers_prev;
			} break;
			default: {
			}
		}
	}

	return result;
}

float map_distance(vec3 p_object) {
	return evaluate_object(p_object).dist;
}

vec3 calculate_normal(vec3 p_object) {
	const float eps = 0.0008;
	const vec2 e = vec2(1.0, -1.0) * 0.5773;
	return normalize(
			e.xyy * map_distance(p_object + e.xyy * eps) +
			e.yyx * map_distance(p_object + e.yyx * eps) +
			e.yxy * map_distance(p_object + e.yxy * eps) +
			e.xxx * map_distance(p_object + e.xxx * eps));
}

bool ray_box_intersection(vec3 ro, vec3 rd, vec3 bmin, vec3 bmax, out float tmin, out float tmax) {
	vec3 inv_rd = 1.0 / rd;
	vec3 t0 = (bmin - ro) * inv_rd;
	vec3 t1 = (bmax - ro) * inv_rd;
	vec3 t_small = min(t0, t1);
	vec3 t_big = max(t0, t1);
	tmin = max(max(t_small.x, t_small.y), t_small.z);
	tmax = min(min(t_big.x, t_big.y), t_big.z);
	return tmax >= max(tmin, 0.0);
}

void main() {
	if (sdf_uniforms.data_info.x == 0u) {
		discard;
	}

	vec2 ndc = uv_interp * 2.0 - 1.0;
	vec4 clip = vec4(ndc, 1.0, 1.0);
	vec4 view_ray = sdf_uniforms.inv_projection * clip;
	vec3 dir_view = normalize(view_ray.xyz / max(view_ray.w, 1e-6));

	vec3 ray_origin_world = sdf_uniforms.inv_view[3].xyz;
	vec3 ray_dir_world = normalize((sdf_uniforms.inv_view * vec4(dir_view, 0.0)).xyz);

	vec3 ray_origin_object = (sdf_uniforms.world_to_object * vec4(ray_origin_world, 1.0)).xyz;
	vec3 ray_dir_object = normalize((sdf_uniforms.world_to_object * vec4(ray_dir_world, 0.0)).xyz);

	float t_near;
	float t_far;
	if (!ray_box_intersection(ray_origin_object, ray_dir_object, sdf_uniforms.bounds_min.xyz, sdf_uniforms.bounds_max.xyz, t_near, t_far)) {
		discard;
	}

	float travel = max(t_near, 0.0);
	float max_travel = t_far;
	float hit_epsilon = sdf_uniforms.march_info.x;
	float min_step = sdf_uniforms.march_info.y;
	float step_scale = sdf_uniforms.march_info.z;
	float max_step = sdf_uniforms.march_info.w;
	int max_steps = int(sdf_uniforms.data_info.y);

	EvalResult result;
	bool hit = false;
	for (int i = 0; i < max_steps; i++) {
		vec3 p_object = ray_origin_object + ray_dir_object * travel;
		result = evaluate_object(p_object);
		float eps = hit_epsilon * (1.0 + travel * 0.01);
		if (result.dist <= eps) {
			hit = true;
			break;
		}

		float march_step = clamp(result.dist * step_scale, min_step, max_step);
		travel += march_step;
		if (travel > max_travel) {
			break;
		}
	}

	if (!hit) {
		discard;
	}

	vec3 hit_object = ray_origin_object + ray_dir_object * travel;
	vec3 hit_world = (sdf_uniforms.object_to_world * vec4(hit_object, 1.0)).xyz;
	vec4 hit_clip = sdf_uniforms.view_projection * vec4(hit_world, 1.0);
	if (hit_clip.w <= 0.0) {
		discard;
	}

	float depth = hit_clip.z / hit_clip.w;
	if (depth < 0.0 || depth > 1.0) {
		discard;
	}
	gl_FragDepth = clamp(depth, 0.0, 1.0);

	float alpha = clamp(result.color.a, 0.0, 1.0);
	if (alpha < 0.995) {
		discard;
	}

	vec3 normal_object = calculate_normal(hit_object);
	vec3 normal_world = normalize(transpose(mat3(sdf_uniforms.world_to_object)) * normal_object);

	vec3 light_dir = normalize(vec3(0.35, 0.78, 0.21));
	float ndotl = max(dot(normal_world, light_dir), 0.0);
	vec3 color = result.color.rgb * (0.18 + 0.82 * ndotl);

	frag_color = vec4(color, 1.0);
}
