#version 330

//uniform sampler2D image;

layout (location = 0) out vec4 FragColor;

in vec3 Position;
in vec2 TexCoord;
in vec3 Normal;
in vec3 color;


void main()
{
  
 
	FragColor = vec4(color ,1.0f);
  
 
}
