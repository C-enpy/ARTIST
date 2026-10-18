#version 330 core
layout(location = 0) in vec3 position;
uniform float f2Scalar;
uniform vec3 f2Vector;
uniform mat3 f2Matrix;
uniform vec3 f2Array[2];
void main()
{
    // Both array elements and every nominal value remain active after linking.
    gl_Position = vec4(f2Matrix * (position + f2Vector + f2Array[0] + f2Array[1])
        + vec3(f2Scalar), 1.0);
}
