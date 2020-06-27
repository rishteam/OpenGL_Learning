#version 410 core
out vec4 color;

uniform sampler2D m_texture;

in GS_OUT{
	vec2 tc; 
	vec3 normal;
} gs_in;

uniform vec3 diffuse_albedo = vec3(0.8f, 0.8f, 0.8f);
uniform vec3 ambient = vec3(0.01f, 0.01f, 0.01f);
uniform vec3 light_pos = vec3(0.25, 0.25, 1);


void main(void)
{
	vec4 landscape = texture(m_texture, gs_in.tc);

	vec3 N = normalize(gs_in.normal);
	vec3 L = light_pos;
	float df = abs(clamp(dot(N, L),0,1)) * 1.5;
	
	color = vec4(landscape.xyz/2 + ambient + df * diffuse_albedo, 1.0);
}
