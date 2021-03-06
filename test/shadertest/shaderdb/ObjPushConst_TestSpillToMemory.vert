#version 450 core

layout(push_constant) uniform PCB
{
    vec4 m0;
    vec4 m1[16];
} g_pc;

void main()
{
    gl_Position = g_pc.m1[8];
}


// BEGIN_SHADERTEST
/*
; RUN: amdllpc -v %gfxip %s | FileCheck -check-prefix=SHADERTEST %s
; SHADERTEST-LABEL: {{^// LLPC}} SPIRV-to-LLVM translation results
; SHADERTEST: AMDLLPC SUCCESS
*/
// END_SHADERTEST
