#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform float uDeltaTime;
uniform float uG_const;
uniform int uAgentsSize;
uniform vec2 uResolution;
uniform vec2 uMousePos;

struct Agent
{
    vec2 position;
    vec2 velocity;
    float mass;
    float radius;
};

layout(std430, binding = 0) buffer InputAgents
{
    Agent inAgents[];
};

layout(std430, binding = 1) buffer OutputAgents
{
    Agent outAgents[];
};

vec2 projection(vec2 a, vec2 b)
{
    return dot(a, b) / length(b) * b;
}

Agent screen_bounds(uint idx)
{
    Agent agent = inAgents[idx];
    // Левая граница
    if (agent.position.x < agent.radius)
    {
        agent.position.x = agent.radius;
        if (agent.velocity.x < 0)
        {
            agent.velocity.x *= -1.0;
            agent.velocity *= 0.9;
        }
    }
    // Правая граница
    else if (agent.position.x > uResolution.x - agent.radius)
    {
        agent.position.x = uResolution.x - agent.radius;
        if (agent.velocity.x > 0)
        {
            agent.velocity.x *= -1.0;
            agent.velocity *= 0.9;
        }
    }
    // Верхняя граница
    else if (agent.position.y < agent.radius)
    {
        agent.position.y = agent.radius;
        if (agent.velocity.y < 0)
        {
            agent.velocity.y *= -1.0;
            agent.velocity *= 0.9;
        }
    }
    // Нижняя граница
    else if (agent.position.y > uResolution.y - agent.radius)
    {
        agent.position.y = uResolution.y - agent.radius;
        if (agent.velocity.y > 0)
        {
            agent.velocity.y *= -1.0;
            agent.velocity *= 0.9;
        }
    }
    return agent;
}

Agent calc_and_apply_force(Agent agent, uint idx)
{
    vec2 forceVec = vec2(0.0);
    Agent other;
    for (int i = 0; i < uAgentsSize; ++i)
    {
        if (i == idx)
        {
            continue;
        }

        other = inAgents[i];
        vec2 direction = other.position - agent.position;
        float dist = length(direction);
        direction /= dist;
        // Сила притяжения
        vec2 force = direction * uG_const * (agent.mass * other.mass) / (dist * dist);

        // Обработка столкновения
        if (dist < agent.radius + other.radius)
        {
            // Обновление скорости
            vec2 relativeVel = other.velocity - agent.velocity;
            float impulse = 2.0 * dot(relativeVel, direction) / (1 / agent.mass + 1 / other.mass);
            agent.velocity += impulse / agent.mass * direction;

            // Корректировка позиции
            float overlay = agent.radius + other.radius - dist;
            agent.position -= overlay / 2 * direction;
        }
        forceVec += force;
    }

    // Применение силы к объекту
    vec2 acceleration = forceVec / agent.mass;
    vec2 deltaVel = acceleration * uDeltaTime;
    agent.velocity += deltaVel;
    agent.position += agent.velocity * uDeltaTime;

    return agent;
}

Agent apply_rules(uint idx)
{
    Agent agent = screen_bounds(idx);    
    agent = calc_and_apply_force(agent, idx);
    return agent;
}

void main()
{
    uint idx = gl_GlobalInvocationID.x;
    outAgents[idx] = apply_rules(idx);
}