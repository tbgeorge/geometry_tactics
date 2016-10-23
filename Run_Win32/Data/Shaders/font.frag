#version 330 core

uniform sampler2D gTexture;

in vec4 passColor;
in vec3 passPos;
in vec2 passUV;

out vec4 outColor;

void main( void )
{
    vec4 color = passColor;

    vec2 uv = passUV;
    vec4 texColor = texture( gTexture, uv );
	outColor = color * texColor;
}