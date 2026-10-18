#version 330 core
layout(location = 0) in float a0;
layout(location = 1) in vec2 a1;
layout(location = 2) in vec3 a2;
layout(location = 3) in vec4 a3;
layout(location = 4) in int a4;
layout(location = 5) in ivec2 a5;
layout(location = 6) in ivec3 a6;
layout(location = 7) in ivec4 a7;
layout(location = 8) in uint a8;
layout(location = 9) in uvec2 a9;
layout(location = 10) in uvec3 a10;
layout(location = 11) in uvec4 a11;
out float b0;
out vec2 b1;
out vec3 b2;
out vec4 b3;
flat out int b4;
flat out ivec2 b5;
flat out ivec3 b6;
flat out ivec4 b7;
flat out uint b8;
flat out uvec2 b9;
flat out uvec3 b10;
flat out uvec4 b11;
void main()
{
    b0 = a0; b1 = a1; b2 = a2; b3 = a3;
    b4 = a4; b5 = a5; b6 = a6; b7 = a7;
    b8 = a8; b9 = a9; b10 = a10; b11 = a11;
    gl_Position = vec4(0.0);
}
