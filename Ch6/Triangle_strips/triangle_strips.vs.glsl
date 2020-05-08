#version 410 core                                                  
                                                                   
in vec3 position;                                                  
                                                                   
out VS_OUT                                                         
{                                                                  
    vec4 color;                                                    
} vs_out;                                                          
                                                                                                            
                                                                   
void main(void)                                                    
{                                                                  
    gl_Position = vec4(position, 1.0);
	//gl_Position = gl_Position + vec4(0, 0.75, 0, 0);
	vs_out.color = vec4(clamp(position*3, 0.0, 1.0),1.0);
}                                                                  