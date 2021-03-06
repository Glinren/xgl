#version 450 core

layout(std140, binding = 0) uniform Block
{
    mat4 m4;
    int  i;
} block;

void main()
{
    int i = block.i;
    gl_Position = block.m4[i] + block.m4[1];
}
// BEGIN_SHADERTEST
/*
; RUN: amdllpc -v %gfxip %s | FileCheck -check-prefix=SHADERTEST %s
; SHADERTEST-LABEL: {{^// LLPC}} SPIRV-to-LLVM translation results
; SHADERTEST: AMDLLPC SUCCESS
*/
// END_SHADERTEST
