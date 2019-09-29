#version 330

layout (location = 0) in vec3 VertexPosition;
layout(location=1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 Position;
out vec2 TexCoord;
out vec3 normalP;

void main()
{
  Position = VertexPosition;
  TexCoord = VertexTexCoord;
  normalP= VertexNormal;
  gl_Position = vec4(VertexPosition, 1.0);
}
