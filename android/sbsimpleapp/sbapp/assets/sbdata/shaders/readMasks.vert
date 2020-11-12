//
//	Adds the masking file to the alpha channel of the texture.
//
// Inputs:
//	-uExpressionSampler ->	texture map
//	-uMaskSampler		->	gray scale image that represents the masking to be applied (white: use, blank: don't use)

uniform highp mat4		uMVP;		//model-view-projection matrix

attribute highp vec4 	aPosition;
attribute mediump vec4 	aTexCoord0;
attribute mediump vec4 	aTexCoord1;
attribute mediump vec4 	aTexCoord2;
attribute mediump vec4 	aTexCoord3;

varying mediump vec4 	gl_TexCoord[4];
void main()
{
    //gl_TexCoord[0]	= gl_MultiTexCoord0;
    //gl_Position		= ftransform();
	gl_Position = uMVP * aPosition;
	gl_TexCoord[0] = aTexCoord0;
	gl_TexCoord[1] = aTexCoord1;
	gl_TexCoord[2] = aTexCoord2;
	gl_TexCoord[3] = aTexCoord3;
}
