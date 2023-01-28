#ifndef GL_GRAPH_WIDGET_H_
#define GL_GRAPH_WIDGET_H_

#include "QtCrtDbgOff.h"
#include <QGLWidget>
#include <QtGui>
#include <QtOpenGL>
#include "QtCrtDbgOn.h"

#if MAC_BUILD
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include "SBDebuggerCommon.h"

#include "vhcl_public.h"

#include <list>
#include <map>
#include <string>

using std::list;
using std::map;
using std::string;
using vhcl::Vector3;

class GLGraphWidget : QGLWidget
{
public:
   GLGraphWidget(const QRect& renderSize, Scene* scene, QWidget* parent = 0);
   ~GLGraphWidget();

   void AddLineGraphPoint(const string& lineName, const Vector3& position, const Vector3& color);
   void AddLineGraphPoint(const string& lineName, const Vector3& position, const Vector3& color, unsigned int maxSize);
   void RemoveLineGraph(const string& lineName);

Q_SIGNALS:
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void Draw();
    
private:
   void timerEvent(QTimerEvent * event);
   QColor qtClearColor;
   QBasicTimer timer;
   Scene* m_pScene;
   double m_SceneScale;

   struct LineGraphPoint
   {
      //string lineName;
      Vector3 position;
      Vector3 color;
      LineGraphPoint(/*const string& _lineName, */Vector3 _position, Vector3 _color)
      {
         //lineName = _lineName;
         position = _position;
         color = _color;
      }
   };

   map<string, list<LineGraphPoint*> > m_DataLines;
};

#endif
