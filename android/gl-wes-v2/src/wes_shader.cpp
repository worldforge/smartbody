/*
gl-wes-v2:  OpenGL 2.0 to OGLESv2.0 wrapper
Contact:    lachlan.ts@gmail.com
Copyright (C) 2009  Lachlan Tychsen - Smith aka Adventus

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include "wes.h"
#include "wes_shader.h"
#include "wes_matrix.h"
#include "wes_fragment.h"

#define WES_PBUFFER_SIZE    128

//shader global variables:
program_t       *sh_program;
GLboolean       sh_program_mod;
program_t       sh_pbuffer[WES_PBUFFER_SIZE];
GLuint          sh_pbuffer_count;
GLuint          sh_vertex;

std::string wesShaderStr = 
"attribute highp vec4 	aPosition;\n\
attribute lowp vec4 	aColor;\n\
attribute mediump vec4 	aTexCoord0;\n\
attribute mediump vec4 	aTexCoord1;\n\
attribute mediump vec4 	aTexCoord2;\n\
attribute mediump vec4 	aTexCoord3;\n\
uniform highp mat4		uMVP;		//model-view-projection matrix\n\
uniform highp mat4		uMV;		//model-view matrix\n\
uniform highp mat3		uMVIT;		//model-view inverted & transformed matrix \n\
\n\
varying lowp vec4 		vColor;\n\
varying lowp vec2		vFactor;\n\
varying mediump vec4 	vTexCoord[4];\n\
\n\
void main(){\n\
	gl_Position = uMVP * aPosition;\n\
	vColor = aColor;\n\
	vTexCoord[0] = aTexCoord0;\n\
	vTexCoord[1] = aTexCoord1;\n\
	vTexCoord[2] = aTexCoord2;\n\
	vTexCoord[3] = aTexCoord3;\n\
	vFactor.x = 1.0;\n\
	vFactor.y = 1.0;\n\
}\n\ ";

std::string wesShaderTestStr = 
"#define LIGHT_NUM						8\n\
#define FACE_NUM						2\n\
#define FACE_FRONT						0\n\
#define FACE_BACK						1\n\
struct sLightModel {\n\
	lowp vec4 	ColorAmbient;\n\
	bool		TwoSided;\n\
	bool		LocalViewer;\n\
	int			ColorControl;\n\
};\n\
struct sLight {\n\
	highp vec4 	Position;\n\
	lowp vec4 	ColorAmbient, ColorDiffuse, ColorSpec;\n\
	highp vec3 	Attenuation; 	// Constant, Linear & Quadratic factors\n\
	highp vec3	SpotDir;\n\
	highp vec2 	SpotVar;		// Spot Shinniness & Cutoff angle\n\
};\n\
struct sMaterial {\n\
	int 		ColorMaterial;\n\
	lowp vec4 	ColorAmbient, ColorDiffuse, ColorSpec, ColorEmissive;\n\
	float 		SpecExponent;\n\
};\n\
attribute highp vec4 	aPosition;\n\
attribute highp vec3 	aNormal;\n\
attribute lowp vec4 	aColor;\n\
attribute mediump vec4 	aTexCoord0;\n\
attribute mediump vec4 	aTexCoord1;\n\
attribute mediump vec4 	aTexCoord2;\n\
attribute mediump vec4 	aTexCoord3;\n\
uniform highp mat4		uMVP;		//model-view-projection matrix\n\
uniform highp mat4		uMV;		//model-view matrix\n\
uniform highp mat3		uMVIT;		//model-view inverted & transformed matrix \n\
uniform	bool			uEnableLight[LIGHT_NUM];\n\
uniform sLight			uLight[LIGHT_NUM];\n\
uniform sLightModel		uLightModel;\n\
uniform sMaterial		uMaterial[FACE_NUM];\n\
\n\
varying lowp vec4 		vColor;\n\
varying lowp vec2		vFactor;\n\
varying mediump vec4 	vTexCoord[4];\n\
\n\
highp vec4				lEye;\n\
highp vec3				lNormal;\n\
lowp vec4				lMaterialAmbient;\n\
lowp vec4				lMaterialDiffuse;\n\
lowp vec4				lMaterialSpecular;\n\
lowp vec4				lMaterialEmissive;\n\
float					lMaterialSpecExponent;\n\
int						lFace;\n\
\n\
vec4 ComputeLightFrom(int i){\n\
	highp vec3 	dpos;\n\
	highp vec4	col;\n\
	float 	dist, dist2, spot;\n\
	float 	att;\n\
	float 	ndoth, ndotl;\n\
	dpos = uLight[i].Position.xyz;\n\
	att = 1.0;\n\
	col = (uLight[i].ColorAmbient * lMaterialAmbient);\n\
	ndotl = dot(lNormal, dpos);\n\
	if (ndotl > 0.0){\n\
		col += ndotl * (uLight[i].ColorDiffuse * lMaterialDiffuse);\n\
	}\n\
	dpos.z += 1.0;\n\
	dpos = normalize(dpos);\n\
	ndoth = dot(lNormal, dpos);\n\
	if (ndoth > 0.0){ \n\
		col += pow(ndoth, lMaterialSpecExponent) * (lMaterialSpecular * uLight[i].ColorSpec);\n\
	}\n\
	return att * col;\n\
}\n\
vec4 ComputeLighting(){\n\
	lFace = FACE_FRONT;\n\
	/* Determine which materials are to be used	*/\n\
	lMaterialAmbient = uMaterial[lFace].ColorAmbient;\n\
	lMaterialDiffuse = uMaterial[lFace].ColorDiffuse;\n\
	lMaterialSpecular = uMaterial[lFace].ColorSpec;\n\
	lMaterialEmissive = uMaterial[lFace].ColorEmissive;\n\
	lMaterialSpecExponent = uMaterial[lFace].SpecExponent;\n\
	//vColor = vec4(1.0,1.0,0.0,1.0);//lMaterialDiffuse;\n\
	vec4 curColor = lMaterialEmissive + lMaterialAmbient;\n\
	for(int i = 0; i < LIGHT_NUM; i++){\n\
		//if (uEnableLight[i]){\n\
		curColor += ComputeLightFrom(i);\n\
		//curColor += uLight[i].ColorDiffuse;\n\
		//}\n\
	}\n\
	//curColor.rgb = vec3(1.0,0.0,0.0);\n\
	curColor.w = lMaterialDiffuse.w;\n\
	return curColor;\n\
}\n\
\n\
void main(){\n\
	gl_Position = uMVP * aPosition;\n\
	lNormal = uMVIT * aNormal;\n\
	//vColor = vec4(1.0,1.0,1.0,1.0);\n\
	vColor = ComputeLighting();\n\
	vTexCoord[0] = aTexCoord0;\n\
	vTexCoord[1] = aTexCoord1;\n\
	vTexCoord[2] = aTexCoord2;\n\
	vTexCoord[3] = aTexCoord3;\n\
	vFactor.x = 1.0;\n\
	vFactor.y = 1.0;\n\
}\n\ ";

std::string wesShaderLightingStr =
"\n\
/*#define LIGHT_NUM						8\n\
#define FACE_NUM						2\n\
#define FACE_FRONT						0\n\
#define FACE_BACK						1\n\
struct sLightModel {\n\
	lowp vec4 	ColorAmbient;\n\
	bool		TwoSided;\n\
	bool		LocalViewer;\n\
	int			ColorControl;\n\
};\n\
struct sLight {\n\
	highp vec4 	Position;\n\
	lowp vec4 	ColorAmbient, ColorDiffuse, ColorSpec;\n\
	highp vec3 	Attenuation; 	// Constant, Linear & Quadratic factors\n\
	highp vec3	SpotDir;\n\
	highp vec2 	SpotVar;		// Spot Shinniness & Cutoff angle\n\
};\n\
struct sMaterial {\n\
	int 		ColorMaterial;\n\
	lowp vec4 	ColorAmbient, ColorDiffuse, ColorSpec, ColorEmissive;\n\
	float 		SpecExponent;\n\
};*/\n\
attribute highp vec4 	aPosition;\n\
attribute lowp vec4 	aColor;\n\
attribute mediump vec4 	aTexCoord0;\n\
attribute mediump vec4 	aTexCoord1;\n\
attribute mediump vec4 	aTexCoord2;\n\
attribute mediump vec4 	aTexCoord3;\n\
attribute highp vec3 	aNormal;\n\
uniform highp mat4		uMVP;		//model-view-projection matrix\n\
uniform highp mat4		uMV;		//model-view matrix\n\
uniform highp mat3		uMVIT;		//model-view inverted & transformed matrix \n\
\n\
varying lowp vec4 		vColor;\n\
varying lowp vec2		vFactor;\n\
varying mediump vec4 	vTexCoord[4];\n\
/*uniform	bool			uEnableLight[LIGHT_NUM];\n\
uniform sLight			uLight[LIGHT_NUM];\n\
uniform sLightModel		uLightModel;\n\
uniform sMaterial		uMaterial[FACE_NUM];*/\n\
highp vec4				lEye;\n\
/*highp vec3				lNormal;\n\
lowp vec4				lMaterialAmbient;\n\
lowp vec4				lMaterialDiffuse;\n\
lowp vec4				lMaterialSpecular;\n\
lowp vec4				lMaterialEmissive;\n\
float					lMaterialSpecExponent;\n\
int						lFace;*/\n\
\n\
/*vec4 ComputeLightFrom(int i){\n\
	highp vec3 	dpos;\n\
	highp vec4	col;\n\
	float 	dist, dist2, spot;\n\
	float 	att;\n\
	float 	ndoth, ndotl;\n\
	dpos = uLight[i].Position.xyz;\n\
	att = 1.0;\n\
	if (uLight[i].Position.w != 0.0){\n\
		dpos -= lEye.xyz;\n\
		if (uLight[i].Attenuation.x != 1.0 || uLight[i].Attenuation.y != 0.0 ||uLight[i].Attenuation.z != 0.0){\n\
			dist2 = dot(dpos, dpos);\n\
			dist = sqrt(dist2);\n\
			att = 1.0 / dot(uLight[i].Attenuation, vec3(1.0, dist, dist2));\n\
		}\n\
		dpos = normalize(dpos);\n\
		if(uLight[i].SpotVar.y < 180.0){\n\
			spot = dot(-dpos, uLight[i].SpotDir);\n\
			if(spot >= cos(radians(uLight[i].SpotVar.y))){\n\
				att *= pow(spot, uLight[i].SpotVar.x);\n\
			} else {\n\
				return vec4(0,0,0,0);\n\
			}\n\
		}\n\
	}\n\
	col = (uLight[i].ColorAmbient * lMaterialAmbient);\n\
	ndotl = dot(lNormal, dpos);\n\
	if (ndotl > 0.0){\n\
		col += ndotl * (uLight[i].ColorDiffuse * lMaterialDiffuse);\n\
	}\n\
	dpos.z += 1.0;\n\
	dpos = normalize(dpos);\n\
	ndoth = dot(lNormal, dpos);\n\
	if (ndoth > 0.0){ \n\
		col += pow(ndoth, lMaterialSpecExponent) * (lMaterialSpecular * uLight[i].ColorSpec);\n\
	}\n\
	return att * col;\n\
}\n\
void ComputeLighting(){\n\
	/* 	Determine Face 	*/\n\
	/*if (uLightModel.TwoSided){\n\
		if (lNormal.z > 0.0){\n\
			lFace = FACE_FRONT;\n\
		} else {\n\
			lNormal = -lNormal;\n\
			lFace = FACE_BACK;\n\
		}\n\
	} else {\n\
		lFace = FACE_FRONT;\n\
	}*/\n\
	lFace = FACE_FRONT;\n\
	/* Determine which materials are to be used	*/\n\
	lMaterialAmbient = uMaterial[lFace].ColorAmbient;\n\
	lMaterialDiffuse = uMaterial[lFace].ColorDiffuse;\n\
	lMaterialSpecular = uMaterial[lFace].ColorSpec;\n\
	lMaterialEmissive = uMaterial[lFace].ColorEmissive;\n\
	lMaterialSpecExponent = uMaterial[lFace].SpecExponent;\n\
	/*if (uEnableColorMaterial){\n\
		if (uMaterial[lFace].ColorMaterial == COLORMAT_AMBIENT){\n\
			lMaterialAmbient = aColor;\n\
		} else if (uMaterial[lFace].ColorMaterial == COLORMAT_DIFFUSE){\n\
			lMaterialDiffuse = aColor;\n\
		} else if (uMaterial[lFace].ColorMaterial == COLORMAT_AMBIENT_AND_DIFFUSE){\n\
			lMaterialAmbient = aColor;\n\
			lMaterialDiffuse = aColor;\n\
		} else if (uMaterial[lFace].ColorMaterial == COLORMAT_SPECULAR){\n\
			lMaterialSpecular = aColor;\n\
		} else {\n\
			lMaterialEmissive =  aColor;\n\
		}\n\
	}*/\n\
	vColor = lMaterialEmissive + lMaterialAmbient * uLightModel.ColorAmbient;\n\
	for(int i = 0; i < LIGHT_NUM; i++){\n\
		if (uEnableLight[i]){\n\
			vColor += ComputeLightFrom(i);\n\
		}\n\
	}\n\
	vColor.w = lMaterialDiffuse.w;\n\
}*/\n\
void main(){\n\
	gl_Position = uMVP * aPosition;\n\
	//vColor = aColor;\n\
	//ComputeLighting();\n\
	vTexCoord[0] = aTexCoord0;\n\
	vTexCoord[1] = aTexCoord1;\n\
	vTexCoord[2] = aTexCoord2;\n\
	vTexCoord[3] = aTexCoord3;\n\
	vFactor.x = 1.0;\n\
	vFactor.y = 1.0;\n\
	vColor = vec4(1.0,0.0,0.0,1.0);\n\
}\n\ ";
//function declarations:
GLvoid
wes_shader_error(GLuint ind)
{
    int len;
    char* log;
    int i;
    glGetShaderiv(ind, GL_INFO_LOG_LENGTH, &len);
    log = (char*) malloc(len + 1);
    memset(log, 0, len+1);
    glGetShaderInfoLog(ind, len, &i, log);
    PRINT_DEBUG("\nShader Error: %s", log);
    free(log);
}

GLvoid
wes_program_error(GLuint ind)
{
    int len;
    char* log;
    int i;
    glGetProgramiv(ind, GL_INFO_LOG_LENGTH, &len);
    log = (char*) malloc(len * sizeof(char));
    glGetProgramInfoLog(ind, len, &i, log);
    PRINT_DEBUG("\nProgram Error: %s", log);
    free(log);
}

GLuint
wes_shader_create(char* data, GLenum type)
{
    GLuint  index;
    GLint   success;

    char *src[1];
    src[0] = data;

    //Compile:
    index = glCreateShader(type);
	//PRINT_DEBUG("glCreateShader, index = %d\n", index);
    glShaderSource(index, 1, (const char**) src, NULL);
    //PRINT_DEBUG("glShaderSource\n");
    glCompileShader(index);
    //PRINT_DEBUG("glCompileShader\n");
    //test status:
    glGetShaderiv(index, GL_COMPILE_STATUS, &success);
	//PRINT_DEBUG("glGetShaderiv\n");
    if (success){
		//PRINT_DEBUG("shader success\n");
        return index;
    } else {
		//PRINT_DEBUG("shader error\n");
        wes_shader_error(index);
        glDeleteShader(index);
        return (0xFFFFFFFF);
    }

}


GLvoid
wes_attrib_loc(GLuint prog)
{
    glBindAttribLocation(prog, WES_APOS,       "aPosition");
    glBindAttribLocation(prog, WES_ATEXCOORD0, "aTexCoord0");
    glBindAttribLocation(prog, WES_ATEXCOORD1, "aTexCoord1");
    glBindAttribLocation(prog, WES_ATEXCOORD2, "aTexCoord2");
    glBindAttribLocation(prog, WES_ATEXCOORD3, "aTexCoord3");
    glBindAttribLocation(prog, WES_ANORMAL,    "aNormal");
    glBindAttribLocation(prog, WES_AFOGCOORD,  "aFogCoord");
    glBindAttribLocation(prog, WES_ACOLOR0,    "aColor");
    glBindAttribLocation(prog, WES_ACOLOR1,    "aColor2nd");
}



GLvoid
wes_uniform_loc(program_t *p)
{
#define LocateUniform(A)                                                \
    p->uloc.A = glGetUniformLocation(p->prog, #A);
#define LocateUniformIndex(A, B, I)                                    \
    sprintf(str, #A "[%i]" #B, I);                                     \
    p->uloc.A[I]B = glGetUniformLocation(p->prog, str);

    int i;
    char str[256];

    LocateUniform(uEnableRescaleNormal);
    LocateUniform(uEnableNormalize);
    for(i = 0; i != WES_MULTITEX_NUM; i++)  {
        LocateUniformIndex(uEnableTextureGen, ,i);
    }
    for(i = 0; i != WES_CLIPPLANE_NUM; i++){
        LocateUniformIndex(uEnableClipPlane, ,i);
    }

    LocateUniform(uEnableFog);
    LocateUniform(uEnableFogCoord);
    LocateUniform(uEnableLighting);
    for(i = 0; i != WES_LIGHT_NUM; i++){
        LocateUniformIndex(uEnableLight, , i);
        LocateUniformIndex(uLight, .Position, i);
        LocateUniformIndex(uLight, .Attenuation, i);
        LocateUniformIndex(uLight, .ColorAmbient, i);
        LocateUniformIndex(uLight, .ColorDiffuse, i);
        LocateUniformIndex(uLight, .ColorSpec, i);
        LocateUniformIndex(uLight, .SpotDir, i);
        LocateUniformIndex(uLight, .SpotVar, i);
		//PRINT_DEBUG("Light %d, ColorDiffuse Loc = %d\n",i, p->uloc.uLight[i].ColorDiffuse);
    }

    LocateUniform(uLightModel.ColorAmbient);
    LocateUniform(uLightModel.TwoSided);
    LocateUniform(uLightModel.LocalViewer);
    LocateUniform(uLightModel.ColorControl);
    LocateUniform(uRescaleFactor);

    for(i = 0; i < 2; i++){
        LocateUniformIndex(uMaterial, .ColorAmbient, i);
        LocateUniformIndex(uMaterial, .ColorDiffuse, i);
        LocateUniformIndex(uMaterial, .ColorSpec, i);
        LocateUniformIndex(uMaterial, .ColorEmissive, i);
        LocateUniformIndex(uMaterial, .SpecExponent, i);
        LocateUniformIndex(uMaterial, .ColorMaterial, i);
    }

    LocateUniform(uFogMode);
    LocateUniform(uFogDensity);
    LocateUniform(uFogStart);
    LocateUniform(uFogEnd);
    LocateUniform(uFogColor);

    for(i = 0; i != WES_CLIPPLANE_NUM; i++){
        LocateUniformIndex(uClipPlane, ,i);
    }

    LocateUniform(uMVP);
    LocateUniform(uMV);
    LocateUniform(uMVIT);
    LocateUniform(uAlphaRef);

    for(i = 0; i != WES_MULTITEX_NUM; i++){
        LocateUniformIndex(uTexUnit, , i);
        LocateUniformIndex(uTexEnvColor, , i);
    }

#undef LocateUniform
#undef LocateUniformIndex
}

GLuint
wes_program_create(GLuint frag, GLuint vert)
{
    GLuint  prog;
    GLint   success;

    //Create & attach
    prog = glCreateProgram();
    glAttachShader(prog, frag);
    glAttachShader(prog, vert);
    glLinkProgram(prog);

    //check status:
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!(success || glGetError())){
        wes_program_error(prog);
        glDeleteProgram(prog);
        return (0xFFFFFFFF);
    }

    wes_attrib_loc(prog);
    glLinkProgram(prog);

    //check status:
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (success || glGetError()){
        return prog;
    } else {
        wes_program_error(prog);
        glDeleteProgram(prog);
        return (0xFFFFFFFF);
    }
};

GLvoid
wes_build_program( progstate_t *s, program_t *p)
{
    char frag[4096];
    memset(frag, 0, 4096);
    wes_frag_build(frag, s);
    p->isbound = GL_FALSE;
    p->pstate = *s;
    p->vert = sh_vertex;
    p->frag = wes_shader_create(frag, GL_FRAGMENT_SHADER);
    p->prog = wes_program_create(p->frag, p->vert);
    wes_uniform_loc(p);
}

GLboolean
wes_progstate_cmp(progstate_t* s0, progstate_t* s1)
{
    GLint i, j;

    if (s0->uEnableAlphaTest != s1->uEnableAlphaTest)
        return 1;

    if (s0->uEnableAlphaTest && (s0->uAlphaFunc != s1->uAlphaFunc))
        return 1;

    if (s0->uEnableFog != s1->uEnableFog)
        return 1;

    if (s0->uEnableClipPlane != s1->uEnableClipPlane)
        return 1;

    for(i = 0; i != WES_MULTITEX_NUM; i++)
    {
        if (s0->uTexture[i].Enable != s1->uTexture[i].Enable)
            return 1;
        else if (s0->uTexture[i].Enable){

            if (s0->uTexture[i].Mode != s1->uTexture[i].Mode)
                return 1;

            if (s0->uTexture[i].Mode == WES_FUNC_COMBINE)
            {
                if (s0->uTexture[i].RGBCombine != s1->uTexture[i].RGBCombine)
                    return 1;
                if (s0->uTexture[i].AlphaCombine != s1->uTexture[i].AlphaCombine)
                    return 1;

                for(j = 0; j != 3; j++){
                    if (s0->uTexture[i].Arg[j].RGBSrc != s1->uTexture[i].Arg[j].RGBSrc)
                        return 1;
                    if (s0->uTexture[i].Arg[j].RGBOp != s1->uTexture[i].Arg[j].RGBOp)
                        return 1;
                    if (s0->uTexture[i].Arg[j].AlphaSrc != s1->uTexture[i].Arg[j].AlphaSrc)
                        return 1;
                    if (s0->uTexture[i].Arg[j].AlphaOp != s1->uTexture[i].Arg[j].AlphaOp)
                        return 1;
                    }
            }
        }
    }

    return 0;
}

GLvoid
wes_bind_program(program_t *p)
{
	
    if (p->isbound) return;
    if (sh_program) sh_program->isbound = GL_FALSE;
    sh_program_mod = GL_TRUE;
    sh_program = p;
    sh_program->isbound = GL_TRUE;
    glUseProgram(sh_program->prog);
}

GLvoid
wes_choose_program(progstate_t *s)
{
    unsigned int i;
    program_t *p;
    for(i = 0; i < sh_pbuffer_count; i++)
    {
        if (!wes_progstate_cmp(s, &sh_pbuffer[i].pstate))
        {
			GLint curProg[1];
			glGetIntegerv(GL_CURRENT_PROGRAM,(GLint*)&curProg[0]);
            if (sh_program != &sh_pbuffer[i] || curProg[0] == 0)
            {
                p = &sh_pbuffer[i];
				if (curProg[0] != p->prog)
					p->isbound = GL_FALSE;
                wes_bind_program(p);
            }
            return;
        }
    }

    p = &sh_pbuffer[sh_pbuffer_count];
    wes_build_program(s, p);
    wes_bind_program(p);
    sh_pbuffer_count++;

    if (sh_pbuffer_count == WES_PBUFFER_SIZE){
        PRINT_ERROR("Exceeded Maximum Programs!");
    }

}

GLvoid
wes_shader_init()
{
    FILE*           file;
    unsigned int    size;
    char*           data;

    sh_pbuffer_count = 0;
    sh_program_mod = GL_TRUE;
#define USE_SHADER_FILE 0
#if USE_SHADER_FILE
    //Load file into mem:
	// load android shader
	PRINT_DEBUG("before loading shader\n");
    file = fopen("/sdcard/uscict_virtualhumandata/shaders/WES_min.vsh", "rb");
	if (!file){
	    //PRINT_ERROR("Could not find file: %s", "WES.vsh");
		PRINT_DEBUG("Could not find file: %s", "WES.vsh");
	}
	PRINT_DEBUG("before fseek\n");
 	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	PRINT_DEBUG("after fseek\n");
	data = (char*) malloc(size + 1);
	if (!data){
	    PRINT_DEBUG("Could not allocate: %i bytes", size + 1);
    }
	if (fread(data, sizeof(char), size, file) != size){
        free(data);
        PRINT_DEBUG("Could not read file: %s", "WES.vsh");
	}
	data[size] = '\0';
	fclose(file);
	PRINT_DEBUG("after fclose\n");
	PRINT_DEBUG("Shader = %s\n", data);
    sh_vertex = wes_shader_create(data, GL_VERTEX_SHADER);
	PRINT_DEBUG("after shader create\n");
    free(data);
#else
#define SHADER_COMPUTE_LIGHTING
#ifdef SHADER_COMPUTE_LIGHTING
	//data = (char*) malloc(wesShaderLightingStr.size() + 1);
	//strcpy(data, wesShaderLightingStr.c_str());
	
	data = (char*) malloc(wesShaderTestStr.size() + 1);
	strcpy(data, wesShaderTestStr.c_str());
#else
	data = (char*) malloc(wesShaderStr.size() + 1);
	strcpy(data, wesShaderStr.c_str());
#endif
	//PRINT_DEBUG("Shader = %s\n", data);
	sh_vertex = wes_shader_create(data, GL_VERTEX_SHADER);
	//PRINT_DEBUG("after shader create\n");
	free(data);
#endif
}


GLvoid
wes_shader_destroy()
{
    unsigned int i;
    glDeleteShader(sh_vertex);
    for(i = 0; i < sh_pbuffer_count; i++)
    {
        glDeleteShader(sh_pbuffer[i].frag);
        glDeleteProgram(sh_pbuffer[i].prog);
    }
}



