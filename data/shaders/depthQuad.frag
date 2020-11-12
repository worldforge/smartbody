/*******************************************************************************
 * GBuffer.frag
 *------------------------------------------------------------------------------
 * Writes final information to the GBuffers.
 ******************************************************************************/
#version 430
//#include "normal.glsl"
layout(binding=0) uniform sampler2D uDiffuseTex;
layout(binding=1) uniform sampler2D uDepthTex;

varying vec2 vTexCoord;

//layout (depth_any) out float gl_FragDepth;

void main()
{
    gl_FragDepth = texture(uDepthTex,vTexCoord).r;
    gl_FragColor = vec4(texture(uDiffuseTex, vTexCoord).rgb,1.0);
}
