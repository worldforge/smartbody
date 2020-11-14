#ifndef _TRANSPARENTRENDERS_H_
#define _TRANSPARENTRENDERS_H_

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

class TransparentViewer;

class TransparencyRenders
{
public:
	TransparencyRenders(TransparentViewer* viewer);
	~TransparencyRenders();

	void drawCharacters(bool shadowPass = false);
	void drawGrid();
	void initGridList();

	float gridColor[4];
	float gridHighlightColor[3];
	float gridSize;
	float gridStep;
	GLint gridList;
	float sceneScale;

	TransparentViewer* _viewer;
};

#endif