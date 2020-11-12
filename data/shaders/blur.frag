#version 430
layout(binding=0) uniform sampler2D uInputTex;

uniform int uBlurSize = 4; // use size of noise texture

varying vec2 texCoord;

layout(location=0) out vec4 fResult;

/*----------------------------------------------------------------------------*/
void main() {
	vec2 texelSize = 1.0 / vec2(textureSize(uInputTex, 0));
	
//	ideally use a fixed size noise and blur so that this loop can be unrolled
	fResult = vec4(0.0);
	vec2 hlim = vec2(float(-uBlurSize) * 0.5 + 0.5);
	for (int x = 0; x < uBlurSize; ++x) {
		for (int y = 0; y < uBlurSize; ++y) {
			vec2 offset = vec2(float(x), float(y));
			offset += hlim;
			offset *= texelSize;					
			fResult += texture(uInputTex, texCoord + offset);
		}
	}
	
	fResult = fResult / float(uBlurSize * uBlurSize);
}