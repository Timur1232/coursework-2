#version 430 core

layout (location = 0) in vec4 iPosition;

uniform vec2 uPosition;
uniform float uRadius;
uniform vec2 uResolution;

void main()
{
    vec2 pos = uPosition.xy / uResolution.xy * 2.0 - 1.0;

    float radius = uRadius / uResolution.y;
    pos.xy *= radius;
    
    pos = iPosition.xy + pos;


    gl_Position = vec4(pos, 0.0, 1.0);
}