/*******************************************************************************
 * GBuffer.frag
 *------------------------------------------------------------------------------
 * Writes final information to the GBuffers.
 ******************************************************************************/
#version 120
//#include "normal.glsl"

varying vec2 vTexCoord;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
    vTexCoord = gl_MultiTexCoord0.xy;
}
