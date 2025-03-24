#version 430 core

uniform vec2 uPosition;
uniform float uRadius;
//uniform float uThickness;
uniform vec2 uResolution;
uniform vec3 uColor;

out vec4 fragColor;

void main()
{
    vec2 uv = (gl_FragCoord.xy / uResolution.xy) * 2.0 - 1.0;
    
    float aspect = uResolution.x / uResolution.y;
    uv.x *= aspect;

    float radius = uRadius / uResolution.y;
    uv.xy /= radius;

    //uv += uPosition.xy / uResolution.xy * 2.0 - 1.0;

    float dist = length(uv);
    
    if (dist > 1.0)
    {
        discard;
    }
    
    fragColor = vec4(uColor, 1.0);
}