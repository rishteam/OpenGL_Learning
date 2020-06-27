#version 410 core
layout(quads, fractional_odd_spacing) in;

uniform sampler2D heightMap;

uniform mat4  um4mv;
uniform mat4  um4p;

in TCS_OUT{ vec2 tc; } tes_in[];
out TES_OUT{
	vec2 tc; 
	vec3 pos;
	vec3 nor; //Normal是為了計算光影使用
} tes_out;

void main(void) {
	vec2 tc1 = mix(tes_in[0].tc, tes_in[1].tc, gl_TessCoord.x);
	vec2 tc2 = mix(tes_in[2].tc, tes_in[3].tc, gl_TessCoord.x);
	vec2 tc = mix(tc2, tc1, gl_TessCoord.y);

	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);    	
	vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);    	
	vec4 p = mix(p2, p1, gl_TessCoord.y);

	p.y += texture(heightMap, tc).r * 10;

	tes_out.tc = tc;
	gl_Position = um4p * um4mv * p;
	tes_out.pos = gl_Position.xyz;

	//計算Normal - 計算光影(進階功能) 
	float height_diff1 = texture(heightMap, tc - vec2(0.01, 0)).r - texture(heightMap, tc + vec2(0.01, 0)).r;
	float height_diff2 = texture(heightMap, tc - vec2(0, 0.01)).r - texture(heightMap, tc + vec2(0, 0.01)).r;

	tes_out.nor = normalize(vec3(height_diff1, 2.0, height_diff2));
}
