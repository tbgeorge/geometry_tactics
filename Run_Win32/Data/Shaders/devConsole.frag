#version 330 core

uniform sampler2D gTexture;

// modifiers for pass variables
// smooth [default], flat, noperspective
in vec4 passColor;
in vec3 passPos;
in vec2 passUV;

out vec4 outColor;

void main( void )
{
    outColor = passColor;
}