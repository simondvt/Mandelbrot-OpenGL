#version 450 core
out vec4 FragColor;
in vec2 pos;

uniform uint iterations;

float rescale(float value, float value_min, float value_max, float new_min, float new_max)
{
    float scaled = (value - value_min) / (value_max - value_min);
    return scaled * (new_max - new_min) + new_min;
}

void main()
{
    vec2 z = vec2(0.0f, 0.0f);
    vec2 c = pos;

    uint i;
    for (i = 0; i < iterations; i++)
    {
        float real = z.x*z.x - z.y*z.y;
        float imag = 2*z.x*z.y;

        z = vec2(real, imag) + c;

        if (distance(z, vec2(0.0f, 0.0f)) > 2)
            break;
    }

    float color = rescale(i, 0.0f, iterations, 1.0f, 0.0f);
	FragColor = vec4(vec3(color), 1.0f);
}