#version 330

layout (location = 0) in vec3 VertexPosition;
layout(location=1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 Position;
out vec2 TexCoord;
out vec3 Normal;
out vec3 color;
vec3 light=vec3(0.0,5.0f,-5.0f);
void main()
{
  Position = VertexPosition;
  TexCoord = VertexTexCoord;
  Normal= VertexNormal;
  vec3 N=normalize(VertexNormal);
  vec3 l=normalize(light-VertexPosition);
  float v=max(dot(l,N),0.0);
  color=vec3(1.0,1.0,1.0)*v;
  gl_Position = vec4(VertexPosition, 1.0);
}
