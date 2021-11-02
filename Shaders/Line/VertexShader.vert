#version 330 core
layout (location = 0) in float vertex;

out float tick;

void main()
{
    tick = vertex;
}
