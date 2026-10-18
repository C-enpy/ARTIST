#version 330 core
#extension GL_ARB_gpu_shader_fp64 : require
layout(location = 0) in vec3 position;
uniform double f2Double;
uniform dvec3 f2DoubleVector;
uniform dmat2 f2DoubleMatrix;
void main()
{
    dvec2 xy = f2DoubleMatrix * (dvec2(position.xy) + f2DoubleVector.xy);
    gl_Position = vec4(vec2(xy), float(f2Double + f2DoubleVector.z), 1.0);
}
