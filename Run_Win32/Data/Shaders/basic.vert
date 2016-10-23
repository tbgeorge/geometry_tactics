#version 330 core

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;

in vec3 inPosition;
in vec4 inColor;
in vec2 inUV;

// Name must match PS IN
out vec4 passColor;
out vec2 passUV;

void main( void )
{
	passColor = inColor;
   	passUV = inUV;
	gl_Position = gProj * gView * gModel * vec4(inPosition, 1.0f);
}