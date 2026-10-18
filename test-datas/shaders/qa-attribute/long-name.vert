#version 330 core
// The fixture replaces QA_LONG_ATTRIBUTE with the exact 300-letter identifier
// (or 256 only if the 300-letter shader cannot compile/link), before compilation.
layout(location = 0) in vec3 QA_LONG_ATTRIBUTE;
void main()
{
    gl_Position = vec4(QA_LONG_ATTRIBUTE, 1.0);
}
