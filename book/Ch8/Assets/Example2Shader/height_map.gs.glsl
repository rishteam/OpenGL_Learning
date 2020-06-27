#version 410

uniform mat4 um4p;
uniform mat4 um4mv;
uniform mat3 um3n;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in TES_OUT{ 
	vec2 tc; 
	vec3 pos;
	vec3 nor;
} tes_in[3];
out GS_OUT{
	vec2 tc; 
	vec3 normal;
} gs_out;

void main()
{
	vec4 offset = vec4(0,30,0,0);

	gs_out.normal = tes_in[0].nor;
	gs_out.tc = tes_in[0].tc;
	gl_Position = gl_in[0].gl_Position - offset; EmitVertex();

	gs_out.normal = tes_in[1].nor;
	gs_out.tc = tes_in[1].tc;
	gl_Position = gl_in[1].gl_Position - offset; EmitVertex();

	gs_out.normal = tes_in[2].nor;
	gs_out.tc = tes_in[2].tc;
	gl_Position = gl_in[2].gl_Position - offset; EmitVertex();

	EndPrimitive();
}