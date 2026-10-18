#version 330 core
layout(location = 0) in vec3 qaArray2[2];
void main()
{
    gl_Position = vec4(qaArray2[0] + qaArray2[1], 1.0);
}
