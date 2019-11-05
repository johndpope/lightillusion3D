#version 330

//uniform sampler2D image;

out vec4 FragColor;


uniform sampler2D myTex;

layout(pixel_center_integer) in vec4 gl_FragCoord;

in vec3 Position;
in vec2 TexCoord;
in vec3 Normal;

vec3 light=vec3(0.0f,0.0f,-5.0f);
vec3 ambient=vec3(0.25f,0.25f,0.25f);

void main()
{
	vec3 N=normalize(Normal);
  vec3 l=normalize(light-Position);
  float v=max(dot(l,N),0.0);
  //v=rand(TexCoord);
  vec3 color=vec3(1.0,1.0,1.0)*v;

  ivec2 screenpos=ivec2(gl_FragCoord.xy);

  vec4 bcolor=texelFetch(myTex,screenpos,0);



	FragColor =(vec4(color,1.0)-bcolor)*0.2+0.2;
	//FragColor=vec4(0.2f);
	//FragColor=vec4(color,1.0)-bcolor;
	//FragColor=vec4(color,1.0)+0.2f;
  
 
}
