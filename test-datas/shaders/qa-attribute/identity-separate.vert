#version 330 core
layout(location = 0) in float a0;
layout(location = 4) in int a4;
layout(location = 8) in uint a8;
layout(location = 3) in vec4 a3;
out float b0;
flat out int b4;
flat out uint b8;
out vec4 b3;
void main()
{
    b0 = a0; b4 = a4; b8 = a8; b3 = a3;
    gl_Position = vec4(0.0);
}
