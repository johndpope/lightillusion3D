#version 330

layout (location = 0) in vec3 VertexPosition;
layout(location=1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 Position;
out vec2 TexCoord;
out vec3 Normal;


float rand(vec2 co){
   return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
  Position = VertexPosition;
  TexCoord = VertexTexCoord;
  Normal= VertexNormal;
  
  gl_Position = vec4(VertexPosition, 1.0);
}
