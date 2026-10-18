#version 330 core
layout(location = 0) in vec3 qaArray[1];
void main()
{
    gl_Position = vec4(qaArray[0], 1.0);
}
