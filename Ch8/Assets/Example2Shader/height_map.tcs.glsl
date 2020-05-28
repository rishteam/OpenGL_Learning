#version 410 core
layout(vertices = 4) out;

in VS_OUT{ vec2 tc; } tcs_in[];
out TCS_OUT{ vec2 tc; } tcs_out[];
uniform mat4  um4mv;
uniform mat4  um4p;
void main(void)
{
	mat4 mvp = um4p * um4mv;

	if (gl_InvocationID == 0)
	{
		vec4 p0 = mvp * gl_in[0].gl_Position;
		vec4 p1 = mvp * gl_in[1].gl_Position;
		vec4 p2 = mvp * gl_in[2].gl_Position;
		vec4 p3 = mvp * gl_in[3].gl_Position;
		

		if (p0.z <= -1.0 && p1.z <= -1.0 && p2.z <= -1.0 && p3.z <= -1.0) {
			gl_TessLevelOuter[0] = 0.0;
			gl_TessLevelOuter[1] = 0.0;
			gl_TessLevelOuter[2] = 0.0;
			gl_TessLevelOuter[3] = 0.0;
		}
		else {
			gl_TessLevelOuter[0] = 64; gl_TessLevelOuter[1] = 64;
			gl_TessLevelOuter[2] = 64; gl_TessLevelOuter[3] = 64;
			gl_TessLevelInner[0] = 64;
			gl_TessLevelInner[1] = 64;
		}
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	tcs_out[gl_InvocationID].tc = tcs_in[gl_InvocationID].tc;
}
