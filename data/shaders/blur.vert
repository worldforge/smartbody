#version 120

varying vec2 texCoord; 
/*----------------------------------------------------------------------------*/
void main() {
//	texcoord:	
	texCoord = gl_Vertex.xy * 0.5 + 0.5;	
	gl_Position = gl_Vertex;
}