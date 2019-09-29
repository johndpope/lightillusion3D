#version 330

uniform sampler2D image;

layout (location = 0) out vec4 FragColor;

in vec2 Position;
in vec2 TexCoord;
in vec3 normalP;



void main()
{
  vec3 light=vec3(1.0,2.0,0.0);
light=normalize(light);
  float v=dot(light,normalP);
  v=clamp(v,0.0,1.0);

  FragColor = vec4(1.0*v, 1.0*v, 0.0, 1.0);
}
