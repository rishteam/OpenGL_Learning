#version 460

in vec2 coord;
out vec4 color; 
uniform sampler2D tex;

void main(void)
{
	// color = vec4(1,0,0,0);		//table 07-02
	//顏色通道切換
	// color.rb = color.br;		//table 07-03

	float alpha = texture(tex, coord).a;		//table 07-06
	if (alpha < 0.1)
		discard;

	color = texture(tex, coord);		//table 07-05

	vec4 red = vec4(1, 0, 0, 1);
	color = texture(tex, coord) * red;		//table 07-07
	// color = texture(tex, coord) + red;		//table 07-08
}