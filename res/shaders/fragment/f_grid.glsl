#version 430 core

uniform vec2 uResolution;
uniform int uGridWidth;
uniform int uGridHeight;

out vec4 FragColor;

void main()
{
    const float cellSizeX = uResolution.x / uGridWidth;
    const float cellSizeY = uResolution.y / uGridHeight;

    for (int i = 1; i < uGridHeight; ++i)
    {
        if (gl_FragCoord.y <= i * cellSizeY + 0.5 && gl_FragCoord.y >= i * cellSizeY - 0.5)
        {
            FragColor = vec4(0.3);
            return;
        }
    }

    for (int i = 1; i < uGridWidth; ++i)
    {
        if (gl_FragCoord.x <= i * cellSizeX + 0.5 && gl_FragCoord.x >= i * cellSizeX - 0.5)
        {
            FragColor = vec4(0.3);
            return;
        }
    }
    discard;
}