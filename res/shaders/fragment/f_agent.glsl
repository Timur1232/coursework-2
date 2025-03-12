#version 430 core

//in vec2 fragCoord;

uniform int uAgentsSize;
uniform vec2 uResolution;
//uniform float uAgentRadius;

out vec4 FragColor;

struct Agent
{
    vec2 position;
    vec2 velosity;
    float mass;
    float radius;
};

layout(std430, binding = 0) buffer Agents
{
    Agent agents[];
};

void main()
{
    vec2 pixelCoord = gl_FragCoord.xy;
    pixelCoord.y = uResolution.y - pixelCoord.y;

    for (int i = 0; i < uAgentsSize; ++i)
    {
        vec2 agentPos = agents[i].position;
        float dist = length(agentPos - pixelCoord);

        if (dist <= agents[i].radius)
        {
            FragColor = vec4(1.0);
            return;
        }
    }
    discard;
}