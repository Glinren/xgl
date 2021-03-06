#version 450

layout(binding = 0) uniform Uniforms
{
    dmat4 dm4;
};

layout(location = 0) out vec4 fragColor;

void main()
{
    mat4 m4 = mat4(dm4);

    fragColor = (m4[0] != m4[1]) ? vec4(0.0) : vec4(1.0);
}
// BEGIN_SHADERTEST
/*
; RUN: amdllpc -v %gfxip %s | FileCheck -check-prefix=SHADERTEST %s
; SHADERTEST-LABEL: {{^// LLPC}} SPIRV-to-LLVM translation results
; SHADERTEST: AMDLLPC SUCCESS
*/
// END_SHADERTEST
