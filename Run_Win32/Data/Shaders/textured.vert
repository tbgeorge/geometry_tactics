#version 330 core

uniform mat4 gView;
uniform mat4 gProj;
uniform mat4 gModel;
   
in vec3 inPosition;
in vec4 inColor;
in vec2 inUV;

// Name must match PS IN
out vec4 passColor;
out vec3 passPos;
out vec2 passUV;

void main( void )
{
   vec4 pos = gProj * gView * gModel * vec4(inPosition, 1.0f);


    passUV = inUV;
    passColor = inColor;
    

   	passPos = pos.xyz;

	gl_Position = pos;
}