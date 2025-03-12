#version 430 core

uniform vec2 uResolution;

void main()
{
    if (gl_FragCoord.x < 0 || gl_FragCoord.x > uResolution.x || gl_FragCoord.y < 0 || gl_FragCoord.y > uResolution.y)
    {
        discard;
    }
    gl_FragColor = vec4(1.0);
}