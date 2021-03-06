#version 450

layout(set = 1, binding = 0) coherent buffer Buffer
{
    readonly vec4 f4;
    restrict vec2 f2;
    volatile writeonly float f1;
} buf;

void main()
{
    vec4 texel = vec4(0.0);

    texel += buf.f4;
    texel.xy += buf.f2;
    buf.f1 = texel.w;
}
// BEGIN_SHADERTEST
/*
; RUN: amdllpc -v %gfxip %s | FileCheck -check-prefix=SHADERTEST %s
; SHADERTEST-LABEL: {{^// LLPC}} SPIRV-to-LLVM translation results
; SHADERTEST: AMDLLPC SUCCESS
*/
// END_SHADERTEST
