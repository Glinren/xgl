#version 450

layout(set = 1, binding = 0) uniform BB
{
   vec4 m1;
   vec4 m2[10];
} b[4];


layout(location = 0) out vec4 o1;

void main()
{
    o1 = b[0].m1 + b[3].m2[5];
}
// BEGIN_SHADERTEST
/*
; RUN: amdllpc -v %gfxip %s | FileCheck -check-prefix=SHADERTEST %s
; SHADERTEST-LABEL: {{^// LLPC}} SPIRV-to-LLVM translation results
; SHADERTEST: AMDLLPC SUCCESS
*/
// END_SHADERTEST
