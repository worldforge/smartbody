#include "vhcl.h"

#include "GLGraphwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLGraphWidget::GLGraphWidget(const QRect& renderSize, Scene* scene, QWidget* parent) : QGLWidget(parent)
{
   setGeometry(renderSize);
   m_pScene = scene;
   //qtClearColor = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
   qtClearColor.setRgb(0, 0, 0);
   timer.start(100, this);
   m_SceneScale = 1;
}

GLGraphWidget::~GLGraphWidget()
{
   map<string, list<LineGraphPoint*> >::iterator itMap;
   for (itMap = m_DataLines.begin(); itMap != m_DataLines.end(); ++itMap)
   {
      while ((*itMap).second.size() != 0)
      {
         LineGraphPoint* pPoint = (*itMap).second.front();
         (*itMap).second.pop_front();
         delete pPoint;
      }
   }
  
   m_DataLines.clear();
}

void GLGraphWidget::initializeGL()
{
    qglClearColor(qtClearColor.dark());

    //float pos1[4] = {1500.0, 1500.0, 1500.0, 1.0};
	float pos0[4] = {-15000.0f, -12000.0f, 15000.0f, 1.0f};
	float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
	float diffuse[4] = {0.6f, 0.6f, 0.6f, 1.0f};
	float specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	//float lmodel_ambient[4] = {0.4, 0.4, 0.4, 1.0};

	glLightfv(GL_LIGHT1, GL_POSITION, pos0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);

	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);       
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);

   QRect renderSize = geometry();
	//glViewport ( 0, 0, renderSize.width(), renderSize.height() );
   glViewport(geometry().x() - 50, 0, renderSize.width() + 50, renderSize.height());

    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);
    glEnable(GL_MULTISAMPLE);
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glEnable ( GL_COLOR_MATERIAL );
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
    

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5 * m_SceneScale, +0.5 * m_SceneScale, -10.5 * m_SceneScale, +10.5 * m_SceneScale, 0.1, 15.0);
    //glOrtho(renderSize.x(), renderSize.x() + renderSize.width(), -20, 20, 0.1f, 1000);

    glMatrixMode(GL_MODELVIEW);
}

void GLGraphWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //glOrtho(-0.5 * m_SceneScale, +0.5 * m_SceneScale, -10.5 * m_SceneScale, +10.5 * m_SceneScale, 0.1, 15.0);
    //glMatrixMode(GL_MODELVIEW);

    if (!m_pScene->m_rendererIsRightHanded)
       glScaled(-1, 1, 1); 
    
    glPushMatrix();
      Draw();
    glPopMatrix();
}

void GLGraphWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    QRect renderSize = geometry();
    glViewport(geometry().x() - 50, 0, width + 50, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5 * m_SceneScale, +0.5 * m_SceneScale, -10.5 * m_SceneScale, +10.5 * m_SceneScale, 0.1, 15.0);

    glMatrixMode(GL_MODELVIEW);
}

void GLGraphWidget::Draw()
{
   const int y_label_num = 25;
   const int x_label_num = 10;
   const float x_size = 1.0f * m_SceneScale;
	const float y_size = 1.0f * m_SceneScale;
   const float ZPos = -10.0f;

   glTranslatef(-5.5f * m_SceneScale, 0, 0);

   float y_length = 8 * m_SceneScale;
	float x_length = 11.65f * m_SceneScale;
	glColor3f(0.25f, 0.25f, 0.25f);

   // draw the grid
	glBegin(GL_LINES); 
      // vertical lines
      for (int i = 1; i < y_label_num; i++)
	   {
		   glVertex3f(i * x_size, y_length, ZPos);
		   glVertex3f(i * x_size, -y_length, ZPos);
	   }
      
      // horizontal lines
      for (int i = 1; i < x_label_num; i++)
	   {
		   glVertex3f(0, y_size * i, ZPos);
		   glVertex3f(x_length, y_size * i, ZPos);
         glVertex3f(0, -y_size * i, ZPos);
		   glVertex3f(x_length, -y_size * i, ZPos);
	   }
	glEnd();

   // draw the world axis darker
   glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, ZPos);
		glVertex3f(x_length, 0.0f, ZPos);

		glVertex3f(0.01f, y_length, ZPos);
		glVertex3f(0.01f, -y_length, ZPos);
	glEnd();

   // render line graph points
   map<string, list<LineGraphPoint*> >::iterator itMap;
   list<LineGraphPoint*>::iterator itList;
   LineGraphPoint* pPoint = NULL;
   for (itMap = m_DataLines.begin(); itMap != m_DataLines.end(); ++itMap)
   {
      glBegin(GL_LINE_STRIP);
      for (itList = (*itMap).second.begin(); itList != (*itMap).second.end(); ++itList)
      {
         pPoint = (*itList);
         glColor3d(pPoint->color.x, pPoint->color.y, pPoint->color.z);
         glVertex3d(pPoint->position.x, pPoint->position.y, pPoint->position.z);
      }
      glEnd();
   }
}

void GLGraphWidget::timerEvent(QTimerEvent * event)
{
   if (event->timerId() == timer.timerId())
   {
      updateGL();
   }
   else
   {
      QWidget::timerEvent(event);
   }
}

void GLGraphWidget::AddLineGraphPoint(const string& lineName, const Vector3& position, const Vector3& color)
{
   if (m_DataLines.find(lineName) == m_DataLines.end())
   {
      m_DataLines[lineName] = list<LineGraphPoint*>();
   }
   else
   {
      m_DataLines[lineName].emplace_back(new LineGraphPoint(position, color));
   }
}

void GLGraphWidget::AddLineGraphPoint(const string& lineName, const Vector3& position, const Vector3& color, unsigned int maxSize)
{
   AddLineGraphPoint(lineName, position, color);

   list<LineGraphPoint*> points = m_DataLines[lineName];
   while (points.size() > maxSize)
   {
      delete points.front();
      points.pop_front();
   }

   //while (points.size() > 10)
   //{
   //   list<LineGraphPoint*>::iterator it = points.begin();
   //   delete *it;
   //   points.erase(it);
   //}
}

void GLGraphWidget::RemoveLineGraph(const string& lineName)
{
   map<string, list<LineGraphPoint*> >::iterator itMap = m_DataLines.find(lineName);
   if (itMap != m_DataLines.end())
   {
      while ((*itMap).second.size() != 0)
      {
         LineGraphPoint* pPoint = (*itMap).second.front();
         (*itMap).second.pop_front();
         delete pPoint;
      }

      m_DataLines.erase(itMap);
   }
}
