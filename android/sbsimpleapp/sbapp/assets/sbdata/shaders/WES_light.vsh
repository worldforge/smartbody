/*
gl-wes-v2:  OpenGL 2.0 to OGLESv2.0 wrapper
*/

#define LIGHT_NUM						8
#define CLIPPLANE_NUM					6
#define MULTITEX_NUM					4
#define FACE_NUM						2

#define FACE_FRONT						0
#define FACE_BACK						1

#define COLORMAT_AMBIENT				0
#define COLORMAT_DIFFUSE				1
#define COLORMAT_AMBIENT_AND_DIFFUSE	2
#define COLORMAT_SPECULAR				3
#define COLORMAT_EMISSIVE				4

#define FUNC_NEVER                     	0
#define FUNC_LESS                     	1
#define FUNC_EQUAL                      2
#define FUNC_LEQUAL                     3
#define FUNC_GREATER                    4
#define FUNC_NOTEQUAL                   5
#define FUNC_GEQUAL                     6
#define FUNC_ALWAYS                     7

#define FOG_LINEAR						0
#define FOG_EXP							1
#define FOG_EXP2						2

#define GEN_OBJLINEAR					1
#define GEN_EYELINEAR					2
#define GEN_SPHEREMAP					3
#define GEN_REFLECTMAP					4

struct sLight {
	highp vec4 	Position;
	lowp vec4 	ColorAmbient, ColorDiffuse, ColorSpec;
	highp vec3 	Attenuation; 	// Constant, Linear & Quadratic factors
	highp vec3	SpotDir;
	highp vec2 	SpotVar;		// Spot Shinniness & Cutoff angle
};

struct sMaterial {
	int 		ColorMaterial;
	lowp vec4 	ColorAmbient, ColorDiffuse, ColorSpec, ColorEmissive;
	float 		SpecExponent;
};

struct sLightModel {
	lowp vec4 	ColorAmbient;
	bool		TwoSided;	
	bool		LocalViewer;
	int			ColorControl;		
};

//Attributes:
attribute highp vec4 	aPosition;
attribute mediump vec4 	aTexCoord0;
attribute mediump vec4 	aTexCoord1;
attribute mediump vec4 	aTexCoord2;
attribute mediump vec4 	aTexCoord3;
attribute highp vec3 	aNormal;
attribute lowp vec4 	aColor;
attribute lowp vec4 	aColor2nd;

//Uniforms:
uniform	bool			uEnableRescaleNormal;
uniform	bool			uEnableNormalize;

uniform highp mat4		uMVP;		//model-view-projection matrix
uniform highp mat4		uMV;		//model-view matrix
uniform highp mat3		uMVIT;		//model-view inverted & transformed matrix 

uniform	bool			uEnableLighting;
uniform	bool			uEnableLight[LIGHT_NUM];
uniform	bool			uEnableColorMaterial;
uniform sLight			uLight[LIGHT_NUM];
uniform sLightModel		uLightModel;
uniform sMaterial		uMaterial[FACE_NUM];
uniform float 			uRescaleFactor;
//Varyings:
varying lowp vec4 		vColor;
varying lowp vec2		vFactor;
varying mediump vec4 	vTexCoord[MULTITEX_NUM];


//local variables:
highp vec4				lEye;
highp vec3				lNormal;
lowp vec4				lMaterialAmbient;
lowp vec4				lMaterialDiffuse;
lowp vec4				lMaterialSpecular;
lowp vec4				lMaterialEmissive;
float					lMaterialSpecExponent;
int						lFace;

vec4 ComputeLightFrom(int i){
	highp vec3 	dpos;
	highp vec4	col;
	float 	dist, dist2, spot;
	float 	att;
	float 	ndoth, ndotl;
	
	dpos = uLight[i].Position.xyz;
	att = 1.0;	

	if (uLight[i].Position.w != 0.0){
		dpos -= lEye.xyz;
		
		if (uLight[i].Attenuation.x != 1.0 || uLight[i].Attenuation.y != 0.0 ||
			uLight[i].Attenuation.z != 0.0){
			dist2 = dot(dpos, dpos);
			dist = sqrt(dist2);
			att = 1.0 / dot(uLight[i].Attenuation, vec3(1.0, dist, dist2));
		}
		
		dpos = normalize(dpos);
		if(uLight[i].SpotVar.y < 180.0){
			spot = dot(-dpos, uLight[i].SpotDir);
			if(spot >= cos(radians(uLight[i].SpotVar.y))){
				att *= pow(spot, uLight[i].SpotVar.x);
			} else {
				return vec4(0,0,0,0);
			}
		}
	}
	
	col = (uLight[i].ColorAmbient * lMaterialAmbient);
	ndotl = dot(lNormal, dpos);
	if (ndotl > 0.0){
		col += ndotl * (uLight[i].ColorDiffuse * lMaterialDiffuse);
	}
	
	dpos.z += 1.0;
	dpos = normalize(dpos);
	ndoth = dot(lNormal, dpos);
	if (ndoth > 0.0){ 
		col += pow(ndoth, lMaterialSpecExponent) * (lMaterialSpecular * uLight[i].ColorSpec);
	}
	
	return att * col;
}

void ComputeLighting(){

	/* 	Determine Face 	*/
	if (uLightModel.TwoSided){
		if (lNormal.z > 0.0){
			lFace = FACE_FRONT;
		} else {
			lNormal = -lNormal;
			lFace = FACE_BACK;
		}	
	} else {
		lFace = FACE_FRONT;
	}
		
	/* Determine which materials are to be used	*/
	lMaterialAmbient = uMaterial[lFace].ColorAmbient;
	lMaterialDiffuse = uMaterial[lFace].ColorDiffuse;
	lMaterialSpecular = uMaterial[lFace].ColorSpec;
	lMaterialEmissive = uMaterial[lFace].ColorEmissive;
	lMaterialSpecExponent = uMaterial[lFace].SpecExponent;
	if (uEnableColorMaterial){
		if (uMaterial[lFace].ColorMaterial == COLORMAT_AMBIENT){
			lMaterialAmbient = aColor;
		} else if (uMaterial[lFace].ColorMaterial == COLORMAT_DIFFUSE){
			lMaterialDiffuse = aColor;
		} else if (uMaterial[lFace].ColorMaterial == COLORMAT_AMBIENT_AND_DIFFUSE){
			lMaterialAmbient = aColor;
			lMaterialDiffuse = aColor;
		} else if (uMaterial[lFace].ColorMaterial == COLORMAT_SPECULAR){
			lMaterialSpecular = aColor;
		} else {
			lMaterialEmissive =  aColor;
		}
	}
		
	vColor = lMaterialEmissive + lMaterialAmbient * uLightModel.ColorAmbient;
	for(int i = 0; i < LIGHT_NUM; i++){
		if (uEnableLight[i]){
			vColor += ComputeLightFrom(i);
		}
	}
	vColor.w = lMaterialDiffuse.w;
}

void main(){
	gl_Position = uMVP * aPosition;
	lEye = uMV * aPosition;
	lNormal = uMVIT * aNormal;
		
	if (uEnableRescaleNormal){
		lNormal = uRescaleFactor * lNormal;
	}
	if (uEnableNormalize){
		lNormal = normalize(lNormal);
	}
	
	/* Lighting 	*/
	if (uEnableLighting){
		ComputeLighting();
	} else {
		vColor = aColor;
	}
	

	/* Tex Coord Generators 	*/
	vTexCoord[0] = aTexCoord0;
	vTexCoord[1] = aTexCoord1;
	vTexCoord[2] = aTexCoord2;
	vTexCoord[3] = aTexCoord3;
}