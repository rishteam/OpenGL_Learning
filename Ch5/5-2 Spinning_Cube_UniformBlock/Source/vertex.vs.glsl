#version 410

layout(location = 0) in vec3 iv3vertex;

out VS_OUT
{
	vec4 color;
} vs_out;

layout (std140) uniform mvp {
	mat4 mv_matrix;		// offset: 0
	mat4 proj_matrix;	// offset: 64
};

void main()
{
	gl_Position = proj_matrix * mv_matrix * vec4(iv3vertex, 1.0);
    vs_out.color = vec4(iv3vertex, 1.0) * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);
}