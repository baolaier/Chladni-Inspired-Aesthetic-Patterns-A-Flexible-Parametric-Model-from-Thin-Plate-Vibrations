#shader vertex
#version 330 core
layout(location =0) in vec2 aPos;
layout(location =1) in vec2 aTexCoord;
out vec2 TexCoord;
out vec2 aPos2;
uniform mat4 u_MVP;
void main()
{
	TexCoord=aTexCoord;
	aPos2=aPos;
	gl_Position=u_MVP*vec4(aPos.x,aPos.y,1.0f,1.0f);
}

#shader fragment
#version 330 core

#define EPSILON 1e-6
#define MAX_ITERATIONS 1000
#define PI 3.14159265359


in vec2 TexCoord;
in vec2 aPos2;
out vec4 color;
float shrink = 1.0f;
uniform sampler2D renderedTexture;
void main(){
    color = texture(renderedTexture, TexCoord);//纹理映射到中心点上
    
}