#pragma once
#include "SbmShader.h"
#include <sr/sr_vec.h>
#include <sr/sr_vec2.h>
#include <string.h>
#include <sb/SBUtilities.h>


enum
{	
	VERTEX_ATTRIBUTE_0 = 0,
	VERTEX_ATTRIBUTE_1 = 1,
	VERTEX_ATTRIBUTE_2 = 2,
	VERTEX_ATTRIBUTE_3 = 3,
	VERTEX_ATTRIBUTE_4 = 4,
	VERTEX_ATTRIBUTE_5 = 5,
#if !defined(__native_client__) && !defined(__ANDROID__) && !defined(SB_IPHONE) 
	VERTEX_POSITION = GL_VERTEX_ARRAY,
	VERTEX_VBONORMAL = GL_NORMAL_ARRAY,
	VERTEX_TEXCOORD = GL_TEXTURE_COORD_ARRAY,
	VERTEX_COLOR = GL_COLOR_ARRAY,
#else
	VERTEX_POSITION,
	VERTEX_VBONORMAL,
	VERTEX_TEXCOORD,
	VERTEX_COLOR,
#endif	
};

enum
{
	VERTEX_BONE_ID_1 = 0,
	VERTEX_BONE_ID_2 = 1,
	VERTEX_BONE_WEIGHT_1 = 2,
	VERTEX_BONE_WEIGHT_2 = 3,
	VERTEX_TANGENT= 4,
	VERTEX_BINORMAL = 5,
};


#if defined(EMSCRIPTEN)
template <class S>
class VBOData
{
public:
	unsigned int m_iVBO_ID;
	bool m_bUpdate;
	GLuint m_ArrayType;

	char m_Name[20];		
	std::vector<SrVec2> *data_Vec2f;
	std::vector<SrVec> *data_Vec3f;
	std::vector<SrVec3i> *data_Vec3i;
	std::vector<SrVec4> *data_Vec4f;
	std::vector<SrVec4i> *data_Vec4i;	
	std::vector<float> *data_float;
public:	
	VBOData(char* name, int type, std::vector<SrVec2>& Data) {}

	VBOData(char* name, int type, std::vector<SrVec>& Data) {}
	VBOData(char* name, int type, std::vector<SrVec3i>& Data) {}
	VBOData(char* name, int type, std::vector<SrVec4>& Data) {}
	VBOData(char* name, int type, std::vector<SrVec4i>& Data) {}
	VBOData(char* name, int type, std::vector<float>& Data) {}
public:
	~VBOData(void) {}

	void BindBuffer() {}
	void UnbindBuffer() {}
	void Update() {}

	void Debug(const char* tag = "VBO") {}

protected:
	void EnableClient(int ArrayType) {}
	void DisableClient(int ArrayType) {}
};



#else

#if 0
class VBOData
{
public:
	unsigned int m_iVBO_ID;
	bool m_bUpdate;
	GLuint m_ArrayType;
	GLuint m_UsageType;

	char m_Name[20];		
	std::vector<SrVec2> *data_Vec2f;
	std::vector<SrVec> *data_Vec3f;
	std::vector<SrVec3i> *data_Vec3i;
	std::vector<SrVec4> *data_Vec4f;
	std::vector<SrVec4i> *data_Vec4i;	
	std::vector<float> *data_float;
public:	
	VBOData(char* name, int type, std::vector<SrVec2>& Data);
	VBOData(char* name, int type, std::vector<SrVec>& Data);
	VBOData(char* name, int type, std::vector<SrVec3i>& Data);
	VBOData(char* name, int type, std::vector<SrVec4>& Data);	
	VBOData(char* name, int type, std::vector<SrVec4i>& Data);
	VBOData(char* name, int type, std::vector<float>& Data);
public:
	~VBOData(void);
	
	void BindBuffer();
	void UnbindBuffer();
	void Update();	
	void Debug(const char* tag = "VBO");

protected:
	void EnableClient(int ArrayType);
	void DisableClient(int ArrayType);
};
#endif

template <class S>
class VBOData
{
public:
	unsigned int m_iVBO_ID;
	bool m_bUpdate;
	bool isIndexBuffer;
	GLuint m_ArrayType;
	GLuint m_UsageType;

	char m_Name[20];
	std::vector<S>* dataPtr;
public:
	VBOData(char* name, GLuint ArrayType, GLuint UsageType, std::vector<S>& Data);
	~VBOData();

	void BindBuffer();
	void UnbindBuffer();
	void Update();
	void UpdateWithData(std::vector<S>& updateData);
};


template <class S>
VBOData<S>::VBOData(char* name, GLuint ArrayType, GLuint UsageType, std::vector<S>& Data)
{
	isIndexBuffer = false;
	m_ArrayType = ArrayType;
	m_UsageType = UsageType;
	strcpy(m_Name, name);
	dataPtr = &Data;
	m_bUpdate = true;
	m_iVBO_ID = 0;
}

template <class S>
VBOData<S>::~VBOData(void)
{
	//SmartBody::util::log("Delete VBOData, m_iVBO_ID = %d", m_iVBO_ID);
	if (m_iVBO_ID)
		glDeleteBuffers(1, &m_iVBO_ID);	
}

template <class S>
void VBOData<S>::BindBuffer()
{
	//SmartBody::util::log("VBOData<S>::BindBuffer, m_iVBO_ID = %d", m_iVBO_ID);
	if (m_ArrayType == GL_ELEMENT_ARRAY_BUFFER)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iVBO_ID);
	else
		glBindBuffer(GL_ARRAY_BUFFER, m_iVBO_ID);			
}

template <class S>
void VBOData<S>::UnbindBuffer()
{	
	if (m_ArrayType == GL_ELEMENT_ARRAY_BUFFER)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	else
		glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template <class S>
void VBOData<S>::Update()
{
	m_bUpdate = true;

	if (m_bUpdate)
	{
		if (m_iVBO_ID <= 0)
			// Get A Valid Name
			glGenBuffers(1, &m_iVBO_ID);

		if (m_ArrayType == GL_ELEMENT_ARRAY_BUFFER)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iVBO_ID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataPtr->size() * sizeof(S),
				getPtr(*dataPtr), m_UsageType);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		else 
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_iVBO_ID);
			glBufferData(GL_ARRAY_BUFFER, dataPtr->size() *sizeof(S),
				getPtr(*dataPtr), m_UsageType);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}		
	}
}


template <class S>
void VBOData<S>::UpdateWithData(std::vector<S>& updateData)
{
	if (m_ArrayType == GL_ELEMENT_ARRAY_BUFFER)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iVBO_ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, updateData.size() * sizeof(S),
			getPtr(updateData), m_UsageType);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_iVBO_ID);
		glBufferData(GL_ARRAY_BUFFER, updateData.size() * sizeof(S),
			getPtr(updateData), m_UsageType);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}


#endif

template<class S>
class VBODataArray
{	
protected:
	std::vector<S> m_Data;
	VBOData<S>*   m_pVBO;	
public:
	VBODataArray(char* name, GLuint type, std::vector<S>& data);
	VBODataArray(char* name, GLuint type, std::vector<S>& data, int number_of_shapes);
	VBODataArray(char* name, GLuint type, int nSize);
	~VBODataArray();
	VBOData<S>* VBO() const { return m_pVBO; }
};

template<class S>
VBODataArray<S>::VBODataArray( char* name, GLuint type, int nSize )
{
	m_Data.resize(nSize);
	m_pVBO = new VBOData<S>(name,type, GL_STATIC_DRAW, m_Data);
	m_pVBO->Update();
}

template<class S>
VBODataArray<S>::~VBODataArray()
{
	//SmartBody::util::log("Delete VBODataArray, m_pVBO = %p", m_pVBO);
	if (m_pVBO)
		delete m_pVBO;
}

template<class S>
VBODataArray<S>::VBODataArray( char* name, GLuint type, std::vector<S>& data )
{
	m_Data.resize(data.size());
	//copy(data,m_Data);
	m_Data = data;
	m_pVBO = new VBOData<S>(name,type, GL_STATIC_DRAW,m_Data);
	m_pVBO->Update();
}

template<class S>
VBODataArray<S>::VBODataArray( char* name, GLuint type, std::vector<S>& data, int number_of_shapes )
{
	m_Data.resize(data.size());
	//copy(data,m_Data);
	m_Data = data;
	m_pVBO = new VBOData<S>(name,type, GL_STATIC_DRAW,m_Data);
	m_pVBO->Update();
}

template <class S>
S* getPtr(std::vector<S>& data)
{
	if (data.size() == 0) return NULL;
	return &data[0];
}

typedef VBODataArray<SrVec2> VBOVec2f;
typedef VBODataArray<SrVec> VBOVec3f;
typedef VBODataArray<SrVec4> VBOVec4f;
typedef VBODataArray<SrVec3i> VBOVec3i;
typedef VBODataArray<SrVec4i> VBOVec4i;


