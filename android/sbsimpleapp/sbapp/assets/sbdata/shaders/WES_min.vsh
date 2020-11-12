/*
gl-wes-v2:  OpenGL 2.0 to OGLESv2.0 wrapper
*/

//Attributes:
attribute highp vec4 	aPosition;
attribute lowp vec4 	aColor;
attribute mediump vec4 	aTexCoord0;
attribute mediump vec4 	aTexCoord1;
attribute mediump vec4 	aTexCoord2;
attribute mediump vec4 	aTexCoord3;

uniform highp mat4		uMVP;		//model-view-projection matrix
uniform highp mat4		uMV;		//model-view matrix
uniform highp mat3		uMVIT;		//model-view inverted & transformed matrix 

varying lowp vec4 		vColor;
varying lowp vec2		vFactor;
varying mediump vec4 	vTexCoord[4];

void main(){
	gl_Position = uMVP * aPosition;
	vColor = aColor;
	vTexCoord[0] = aTexCoord0;
	vTexCoord[1] = aTexCoord1;
	vTexCoord[2] = aTexCoord2;
	vTexCoord[3] = aTexCoord3;
	vFactor.x = 1.0;
	vFactor.y = 1.0;
}