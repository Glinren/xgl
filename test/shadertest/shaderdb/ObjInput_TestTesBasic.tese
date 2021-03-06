#version 450 core

layout(triangles) in;

layout(location = 1) in vec4 inData1[];
layout(location = 2) patch in dvec4 inData2[4];

void main()
{
    gl_Position = inData1[1] + vec4(inData2[gl_PrimitiveID].z);
}
// BEGIN_SHADERTEST
/*
; RUN: amdllpc -v %gfxip %s | FileCheck -check-prefix=SHADERTEST %s
; SHADERTEST-LABEL: {{^// LLPC}} SPIRV-to-LLVM translation results
; SHADERTEST: AMDLLPC SUCCESS
*/
// END_SHADERTEST
