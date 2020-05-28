#version 460

#define ID gl_InvocationID

layout(vertices = 3) out;
in vec3 vPosition[];
out vec3 tcPosition[];

uniform mat4 um4p;
uniform mat4 um4mv;
uniform float uf1zoom;
uniform float TessLevelInner;
uniform float TessLevelOuter;

void main()
{
	tcPosition[ID] = vPosition[ID];

	if (ID == 0) {

		vec4 p0 = um4p * um4mv * vec4(vPosition[0], 1);
		vec4 p1 = um4p * um4mv * vec4(vPosition[1], 1);
		vec4 p2 = um4p * um4mv * vec4(vPosition[2], 1);

		float newTessLevelInner = max(TessLevelInner - uf1zoom * 3, 1);
		float newTessLevelOuter = max(TessLevelOuter - uf1zoom * 3, 1);

		gl_TessLevelInner[0] = newTessLevelInner;
		gl_TessLevelOuter[0] = newTessLevelOuter;
		gl_TessLevelOuter[1] = newTessLevelOuter;
		gl_TessLevelOuter[2] = newTessLevelOuter;
	}
}