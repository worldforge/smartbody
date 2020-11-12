/*******************************************************************************
 * GBuffer.vert
 *------------------------------------------------------------------------------
 * Calculates previous and current positions, as well as material attributes.
 ******************************************************************************/
// Framebuffer Outputs
#version 430
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec4 BoneID1;
layout(location = 4) in vec4 BoneWeight1;

uniform mat4 Transform[120]; 
uniform float meshScale = 1.0;

out vec3 deformPos;
out vec3 deformNormal;
out vec3 deformTangent;

mat4 GetTransformation(float id)
{ 
	int idx = int(id);
	return Transform[idx];
}

mat3 TransformPos(vec3 position, vec3 normal, vec3 tang, vec4 boneid, vec4 boneweight)
{
	vec3 pos = vec3(0,0,0);
	vec3 n = vec3(0,0,0);
	vec3 t = vec3(0,0,0);
	vec3 b = vec3(0,0,0);
	mat4 tempT;
	vec3 tempt;
	for (int i=0;i<4;i++)
	{
		tempT = GetTransformation(boneid[i]);
		pos += (vec4(position,1.0)*tempT).xyz*boneweight[i];
		n   += (vec4(normal,0.0)*tempT).xyz*boneweight[i];
		t   += (vec4(tang,0.0)*tempT).xyz*boneweight[i];			
	}	
	mat3 result;
	result[0] = vec3(pos);
	result[1] = vec3(n);
	result[2] = vec3(t);		
	return result;
}
void main()
{
// the following three lines provide the same result
	vec3 pos = vec3(position.xyz)*meshScale;
	mat3 skin = TransformPos(pos,normal,tangent,BoneID1,BoneWeight1);	
	deformPos = skin[0];
	deformNormal = normalize(skin[1]);
	deformTangent = normalize(skin[2]);
}
