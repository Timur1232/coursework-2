#version 430 core

layout (location = 0) in vec4 iPosition;

uniform vec2 uCameraPos;
uniform float uCameraZoom;

void main()
{
	vec2 pos = iPosition.xy;
	pos = pos - uCameraPos;
	gl_Position = vec4(pos, 0.0, uCameraZoom);
}