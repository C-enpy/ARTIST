#version 330 core
layout(location = 0) in vec3 qaPosition;
layout(location = 1) in mat4 qaMatrix;
void main()
{
    gl_Position = qaMatrix * vec4(qaPosition, 1.0);
}
