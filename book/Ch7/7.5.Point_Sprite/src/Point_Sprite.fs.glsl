#version 460

out vec4 color;

uniform sampler2D tex_star;
flat in vec4 starColor;

void main(void)
{
	color = starColor * texture(tex_star, gl_PointCoord);
	if (color.a < 0.1)
		discard;
}