#version 330 core
layout (location = 0) in vec2 topLeft;

void main()
{
    gl_Position = vec4(topLeft, 0, 1);
}
