#version 430 core

uniform vec2 uCameraPosition;
uniform float uCameraViewYSize;
uniform vec2 uResolution;
uniform float uZoomFactor;
uniform float uDarkeningFactor;
uniform float uYOffset;

out vec4 fragColor;

float sigmoid(float x)
{
    return 1.0 / (1.0 + exp(-x));
}

void main()
{
    float uvy = gl_FragCoord.y / uResolution.y * uCameraViewYSize / (uZoomFactor / 4.0);
    float h = (uCameraPosition.y + uYOffset - uvy) / uResolution.y * 2.0;
    
    vec3 deepWaterColor = vec3(0.0, 0.05, 0.08);
    
    float depth = sigmoid(h / uDarkeningFactor * (uZoomFactor / 4.0));
    if (depth < 0.01)
        discard;
    
    fragColor = vec4(depth * deepWaterColor, depth);
}