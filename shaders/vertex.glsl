#version 450 core
layout (location = 0) in vec2 aPos;

out vec2 pos;

uniform float minX, minY;
uniform float maxX, maxY;

float rescale(float value, float value_min, float value_max, float new_min, float new_max)
{
    float scaled = (value - value_min) / (value_max - value_min);
    return scaled * (new_max - new_min) + new_min;
}

void main()
{
    gl_Position = vec4(aPos, 0.0f, 1.0f);
    pos.x = rescale(aPos.x, -1.0f, 1.0f, minX, maxX);
	pos.y = rescale(aPos.y, -1.0f, 1.0f, minY, maxY);
}