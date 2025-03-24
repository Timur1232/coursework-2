#version 430 core

vec2 rotate(vec2 v, float angleDeg)
{
    float angleRad = radians(angleDeg);
    return v * mat2(cos(angleRad), -sin(angleRad), sin(angleRad), cos(angleRad));
}

uniform vec2 uResolution;
uniform float uAngleDeg;
uniform float uArc;
uniform vec3 uColor;

out vec4 fragColor;

void main()
{
    vec2 uv = (gl_FragCoord.xy / uResolution.xy) * 2.0 - 1.0;
    
    float aspect = uResolution.x / uResolution.y;
    uv.x *= aspect;
    
    uv = rotate(uv, uAngleDeg);
    
    float dist = length(uv);
    
    if (dist > 1.0 || dot(normalize(uv), vec2(0.0, 1.0)) < uArc)
    {
        discard;
    }
    
    fragColor = uColor;
}