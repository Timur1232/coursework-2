#version 430 core

uniform vec2 uResolution;
uniform vec2 uCameraPosition;
uniform float uYOffset;
uniform float uZoomFactor;
uniform float uTime;
uniform float uDeepDarkFactor;

float WaveMask(vec2 uv, float h, float t, float i) {
    vec2 st = uv;
    st.x += t;
    st.y += (sin(st.x * 10.0) * 0.5 + 0.5) * i;

    float softness = 0.001;
    float c = smoothstep(h + softness, h, st.y);
    return c;
}

float WaterMask(vec2 uv, float h, float t, float i) {
    float waterMask1 = WaveMask(uv, h, t * 0.4, i * 0.05);
    float waterMask2 = WaveMask(uv, h, -t * 0.3, i * 0.05);
    float waterMask3 = WaveMask(uv, h, t * 0.5, i * 0.05);

    float waterMask = mix(waterMask1, waterMask2, 0.5);
    waterMask = mix(waterMask, waterMask3, 0.5);
    return waterMask;
}

out vec4 fragColor;

void main()
{
    float v = uCameraPosition.x / uResolution.x * 2.0 - 1.0;
    vec2 uv = gl_FragCoord.xy / uResolution.xy * 2.0 - 1.0;
    uv *= uZoomFactor;
    uv.x += v;
    
    float h = (uCameraPosition.y + uYOffset) / uResolution.y * 2.0;

    vec3 waterColor = vec3(26.0, 71.0, 128.0) / 255.0;
    vec3 deepWaterColor = vec3(0.0, 51.0, 98.0) / 255.0;

    float o1 = sin(uTime * 2.0) * 0.02;
    float o2 = sin(uTime * 0.5) * 0.02;
    float o3 = sin(uTime) * 0.02;

    float w1 = WaterMask(uv * vec2(1.0, 1.0), o1 + h - 0.15, uTime, 1.0);
    float w2 = WaterMask(uv * vec2(2.0, 1.0), o2 + h - 0.06, uTime * 0.5, 0.8);
    float w3 = WaterMask(uv * vec2(3.0, 1.0), o3 + h, uTime * 0.3, 0.5);

    float w = mix(w1, w2, 0.5);
    w = mix(w, w3, 0.5);

    float deepnesFactor = (uCameraPosition.y / uResolution.y * 2.0 - uv.y) / uDeepDarkFactor;
    fragColor = vec4(w * waterColor * (1.0 - deepnesFactor) + w * deepWaterColor * deepnesFactor, w);
}