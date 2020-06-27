#version 460

layout(location = 0) in vec3 iv3vertex;

out VS_OUT
{
	vec4 color;
} vs_out;

layout(location = 0) uniform mat4 mv_matrix;
layout(location = 1) uniform mat4 proj_matrix;

void main()
{
	gl_Position = proj_matrix * mv_matrix * vec4(iv3vertex, 1.0);
    vs_out.color = vec4(iv3vertex, 1.0) * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);
}