#version 430
#define MAX_LIGHTS 20
layout(binding=0) uniform sampler2D uNormalTex;
layout(binding=1) uniform sampler2D uEnvLightTex;
layout(binding=2) uniform sampler2D uEnvDiffuseTex;
layout(binding=3) uniform sampler2D uDiffuseTex;
layout(binding=4) uniform sampler2D uSSAOTex;
layout(binding=5) uniform sampler2D uSpecularTex;

varying vec2 texCoord;
varying vec3 viewRay;
uniform mat4 uViewToWorldMatrix;
uniform float roughnessVal = 0.0;
uniform float metallic = 0.0;
uniform float shininess = 2.0;
uniform uvec2 Dimensions = uvec2(2048, 4096);//uvec2(4096,2048);

uniform float exposure = 1.0;
uniform float gamma = 1.0;
uniform int showEnvMap = 1;

uniform int numLights = 2;
uniform vec3 lightDir[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];

float shininessBase = 20.0;
float glossyVal = 0.0;
float episilon = 0.00001f;
float almostOne = 0.99f;

layout(location=0) out vec4 fResult;

const float PI = 3.1415926535897932384626433832795;
const float TwoPI = 3.1415926535897932384626433832795*2.0;
#define XAxis vec3(1.0, 0.0, 0.0)
#define YAxis vec3(0.0, 1.0, 0.0)
#define ZAxis vec3(0.0, 0.0, 1.0)

vec2 envMapEquirect(vec3 wcNormal, float flipEnvMap) {
  //I assume envMap texture has been flipped the WebGL way (pixel 0,0 is a the bottom)
  //therefore we flip wcNorma.y as acos(1) = 0
  float phi = acos(-wcNormal.y);
  float theta = atan(flipEnvMap * wcNormal.x + episilon, wcNormal.z + episilon) + PI;
  return vec2(theta / TwoPI, phi / PI);
}

vec2 envMapEquirect(vec3 wcNormal) {
    //-1.0 for left handed coordinate system oriented texture (usual case)
    return envMapEquirect(wcNormal, -1.0);
}

vec3 Uncharted2Tonemap( vec3 x )
{
	const float A = 0.15;
	const float B = 0.50;
	const float C = 0.10;
	const float D = 0.20;
	const float E = 0.02;
	const float F = 0.30;

	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float randAngle()
{
  uint x = uint(gl_FragCoord.x);
  uint y = uint(gl_FragCoord.y);
  return (30u * x ^ y + 10u * x * y);
}

float roughness()
{
	return (1.0-glossyVal);//roughness;
}

vec2 Hammersley(uint i, uint N)
{
  return vec2( float(i) / float(N), float(bitfieldReverse(i)) * 2.3283064365386963e-10 );
}

vec3 MakeSample(float Theta, float Phi)
{
  Phi += randAngle();
  float SineTheta = sin(Theta);

  float x = cos(Phi) * SineTheta;
  float y = sin(Phi) * SineTheta;
  float z = cos(Theta);

  return vec3(x, y, z);
}

float saturate(float v)
{
  return clamp(v, 0.0, 1.0);
}

float DPhong(float NoH)
{
  float Krough2 = roughness() * roughness();
  float ap = (2.0 / Krough2) - 2.0;
  return saturate(NoH) * ((ap + 2.0) / TwoPI) * pow(saturate(NoH), ap);
}

float DGgx(float NoH)
{
  // Note: Generally sin2 + cos2 = 1
  // Also: Dgtr = c / (a * cos2 + sin2)
  // So...
  float Krough2 = roughness() * roughness();
  float NoH2 = NoH * NoH;
  float denom = 1.0 + NoH2 * (Krough2 - 1.0);
  return Krough2 / (PI * denom * denom);
}

float FSchlick(float VoH)
{
  float Kmetallic = metallic;
  return Kmetallic + (1.0 - Kmetallic) * pow(1.0 - VoH, 5.0);
}

float GSmithSchlickBeckmann_(float NoV)
{
  float k = roughness() * roughness() * sqrt(TwoPI);
  return NoV / (NoV * (1.0 - k) + k);
}

float GSmithSchlickBeckmann(float NoL, float NoV, float NoH, float VoH)
{
  return GSmithSchlickBeckmann_(NoL) * GSmithSchlickBeckmann_(NoV);
}


vec3 DPhongSample(vec2 E)
{
  float ap = (2.0 / roughness()) - 2.0;
  float Theta = acos(pow(E.x, 2.0 / (ap + 2.0))) / (TwoPI * TwoPI);
  float Phi = TwoPI * E.y;
  return MakeSample(Theta, Phi);
}

float compute_lod(uint NumSamples, float NoH)
{
  return max(0.0, 0.5 * (log2(float(Dimensions.x * Dimensions.y) / NumSamples) - log2(DPhong(NoH))));
}

vec3 EnvBRDFApprox( vec3 SpecularColor, float Roughness, float NoV )
{
	const vec4 c0 = vec4( -1, -0.0275, -0.572, 0.022 );
	const vec4 c1 = vec4( 1, 0.0425, 1.04, -0.04 );
	vec4 r = Roughness * c0 + c1;
	float a004 = min( r.x * r.x, exp2( -9.28 * NoV ) ) * r.x + r.y;
	vec2 AB = vec2( -1.04, 1.04 ) * a004 + r.zw;
	return SpecularColor * AB.x + AB.y;
}

vec3 radiance(vec3 N, vec3 V)
{
  // Precalculate rotation for +Z Hemisphere to microfacet normal.
  vec3 UpVector = abs(N.z) < 0.999 ? ZAxis : XAxis;
  vec3 TangentX = normalize(cross( UpVector, N ));
  vec3 TangentY = cross(N, TangentX);

  float NoV = saturate(dot(N, V));

  // Approximate the integral for lighting contribution.
  vec3 fColor = vec3(0.0);
  const uint NumSamples = 20;
  for (uint i = 0; i < NumSamples; ++i)
  {
    vec2 Xi = Hammersley(i, NumSamples);
    vec3 Li = DPhongSample(Xi); 
    vec3 H  = normalize(TangentX*Li.x + TangentY* Li.y + N * Li.z);
    vec3 L  = normalize(-reflect(V, H));

    // Calculate dot products for BRDF
    float NoL = saturate(dot(N, L));
    float NoH = saturate(dot(N, H));
    float VoH = saturate(dot(V, H));    
    float lod = compute_lod(NumSamples, NoH);
    // Since the sample is skewed towards the Distribution, we don't need
    // to evaluate all of the factors for the lighting equation. Also note
    // that this function is calculating the specular portion, so we absolutely
    // do not add any more diffuse here.
    //fColor += F_ * G_ * LColor * VoH / (NoH * NoV);
    if (NoV > 0 && NoH > 0 && NoL > 0 && VoH > 0)
    {
    	#if 1
    	float F_ = FSchlick(VoH); // Defined elsewhere as subroutine
    	float G_ = GSmithSchlickBeckmann(NoL, NoV, NoH, VoH); // Defined elsewhere as subroutine
    	vec3 LColor = textureLod(uEnvLightTex, envMapEquirect(L), lod).rgb;
    	fColor += LColor * F_ * G_ * VoH / (NoV*NoH);//*NoH);//*NoH);//(NoH * NoV);
    	#else // use Phong model
    	vec3 LColor = textureLod(uEnvLightTex, envMapEquirect(L), lod).rgb;    	
    	fColor += LColor*pow(VoH, shininessBase*(1.0-roughness));
    	#endif

    }
    //fColor += LColor;
  }
  // Average the results
  return fColor / float(NumSamples);
}

vec3 phongLight(vec3 normal, vec3 viewDir, vec4 specColor)
{
	//return vec3(1.0, 0.0, 0.0);
	float shininess = specColor.a*shininessBase;
	vec3 lcolor = vec3(0,0,0);
	for (int i=0;i<numLights;i++)
	{		
		float ndotl = max(dot(lightDir[i], normal), 0.0);		
		lcolor += lightColor[i]*ndotl;
#if 1
		vec3 halfVec = normalize(viewDir + lightDir[i]);
		float ndoth = max(dot(halfVec, normal), 0.0);
		lcolor += specColor.rgb*pow(ndoth, (shininess+1.0));
#endif
	}
	return lcolor;
}

/*----------------------------------------------------------------------------*/
void main() {	
	
	#if 1
	//vec3 normal = decodeNormal(texture(uNormalTex, texCoord).rgb);	
	vec3 normal = texture(uNormalTex, texCoord).rgb;	
	vec3 wray = -normalize(mat3(uViewToWorldMatrix)*viewRay);
	if (length(normal) == 0.0)
	{
		
		vec3 lcolor = textureLod(uEnvLightTex, envMapEquirect(-wray),0).rgb;
		if (showEnvMap == 0)
			fResult = vec4(0.3, 0.3, 0.3, 1.0);
		else
			fResult = vec4(lcolor, 1.0);
			
	}
	else
	{
		vec3 wnormal = normalize(mat3(uViewToWorldMatrix)*normal);
		vec3 lcolor = texture(uEnvDiffuseTex, envMapEquirect(wnormal)).rgb;	
		//vec3 envMapColor = texture(uEnvDiffuseTex, texCoord).rgb;
		vec4 diffuseColor = texture(uDiffuseTex, texCoord);
		vec4 specularColor = texture(uSpecularTex, texCoord);
		//roughness = 1.0 - specularColor.a;
		glossyVal = specularColor.a;
		float ssaoVal = texture(uSSAOTex, texCoord).r;
		vec3 radiance = radiance(wnormal, wray);
		float NoV = saturate(dot(wnormal, wray));
		//vec3 reflectance = EnvBRDFApprox(vec3(1.0, 1.0, 1.0), pow(roughness, 4.0), NoV);
		vec3 phongColor = phongLight(wnormal, wray, specularColor);
		//fResult = vec4(envMapColor, 1.0);
		//fResult = vec4(lcolor/PI*diffuseColor.rgb, 1.0);
		fResult = vec4((lcolor.rgb/PI + phongColor + specularColor.rgb*radiance)*ssaoVal*diffuseColor.rgb, 1.0); //vec4(radiance + lcolor.rgb/PI + phongColor, 1.0);//vec4((radiance + lcolor.rgb/PI + phongColor)*ssaoVal*diffuseColor.rgb, 1.0);//vec4(specularColor + lcolor.rgb/PI, 1.0);//vec4(lcolor*diffuseColor.rgb*ssaoVal, 1.0);		
		//fResult = vec4((radiance*reflectance + lcolor.rgb/PI)*ssaoVal*diffuseColor.rgb, 1.0);
		//fResult = vec4(lcolor*ssaoVal, 1.0);
	}
	#endif	

	vec3 outColor = fResult.rgb;
	outColor				= Uncharted2Tonemap( outColor * exposure );
	// white balance
	outColor				= outColor * ( 1.0f / Uncharted2Tonemap( vec3( 20.0f ) ) );	
	// gamma correction
	outColor				= pow( outColor, vec3( 1.0f / gamma ) );

	fResult.rgb = outColor;

	//vec3 envMapColor = textureLod(uEnvLightTex, texCoord, roughness).rgb;
	//fResult = vec4(envMapColor, 1.0);
	
	//fResult = vec4(1,0,0,1);
	//fResult = vec4(wnormal, 1.0);
	//fResult = vec4(lcolor*diffuseColor.xyz*ssaoVal, 1.f);
	//fResult = vec4(lcolor, 1.0);
}