#version 330

//uniform sampler2D image;

layout (location = 0) out vec4 FragColor;

in vec3 Position;
in vec2 TexCoord;
in vec3 Normal;

vec3 light=vec3(0.0,5.0f,-5.0f);

void main()
{
  vec3 N=normalize(Normal);
  vec3 l=normalize(light-Position);
  float v=max(dot(l,N),0.0);
 
	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f)*v;
  
 
}
