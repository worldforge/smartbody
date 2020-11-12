#version 120
uniform float uTanHalfFov;
uniform float uAspectRatio;

varying vec2 texCoord; 
varying vec3 viewRay;

/*----------------------------------------------------------------------------*/
void main() {
//	texcoord:	
	texCoord = gl_Vertex.xy * 0.5 + 0.5;	
//	view ray:
	viewRay = vec3(
		gl_Vertex.x * uTanHalfFov * uAspectRatio,
		gl_Vertex.y * uTanHalfFov, 
		-1.0
	);
	gl_Position = gl_Vertex;
}