#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "vhcl_public.h"

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

#include "vhcl_timer.h"
#include "Camera.h"
#include "SBDebuggerCommon.h"
#include "SettingsDialog.h"


using std::string;
using std::vector;

#define SELECT_BUFF_SIZE 1024
#define ENTITY_PICKING_OFFSET 1000
#define FLOOR_LAYER 100

#define ENTITY_TO_PICK(index) index * ENTITY_PICKING_OFFSET + ENTITY_PICKING_OFFSET
#define PICK_TO_CHARACTER(pick) pick / ENTITY_PICKING_OFFSET - 1
#define PICK_TO_JOINT(pick) pick % ENTITY_PICKING_OFFSET

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(Scene* scene, QWidget *parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    virtual void Update();

    void SetScene(Scene* scene) { m_pScene = scene; }
    void SetMinimumSize(int w, int h) { m_MinSize.setWidth(w); m_MinSize.setHeight(h); }
    void CheckCoordinateSystem();
    double GetFps();
    string GetCameraPositionAsString();
    string GetFpsAsString();
    void SetObjectScale(float scale);

public slots:
    void OnCloseSettingsDialog(const SettingsDialog* dlg, int result);
    void ToggleFreeLook();
    void sceneTreeCurrentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );
    void sceneTreeItemDoubleClicked(QTreeWidgetItem * item, int column);
    void ToggleShowAxes(bool enabled);
    void ToggleShowEyeBeams(bool enabled);
    void ToggleAllowBoneUpdates(bool enabled);

signals:
    void JointPicked(const Pawn* jointOwner, const Joint* jointSelected);

Q_SIGNALS:
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    //void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *key);
    
private:

    enum GLMode
    {
      RENDER,
      SELECT,
    };

    enum RenderFlags
    {
      Show_EyeBeams = 1,
      Show_Axes = 1 << 1,
    };

    struct SelectionData
    {
         Pawn* m_pObj;
         Joint* m_pJoint;
         SelectionData() { m_pObj = NULL; m_pJoint = NULL; }
    };

    QPoint lastPos;
    QColor qtPurple;
    Camera m_Camera;
    float m_fPawnSize;
    float m_fJointRadius;
    float m_fAxisLength;
    float m_fEyeBeamLength;
    Scene* m_pScene;
    GLUquadric* m_quadric;
    QSize m_MinSize;
    int m_RenderFlags;

    double m_msSinceLastFrame;
    double m_msSinceLastFramePrev;

    QBasicTimer timer;
    vhcl::Timer m_StopWatch;

    GLMode m_GLMode;
    SelectionData m_SelData;
    int m_nPickingOffset;
    GLuint selectBuf[SELECT_BUFF_SIZE];

    virtual void timerEvent(QTimerEvent * event);

    // picking Functions
    void StartPicking();
    Joint* FindPickedJoint(int pickIndex);
    Joint* FindPickedJointRecursive(const Joint* joint);
    void ProcessHits(GLint hits, GLuint buffer[]);
    void StopPicking();
    void SetSelectedObject(Pawn* obj, Joint* joint);
    QVector3D GetWorldPositionFromScreenCoords(int x, int y);

    // Drawing Functions
    void DrawFloor();
    void DrawScene();
    void DrawCharacter(const Character* character);
    void DrawJoint(Joint* joint);
    void DrawPawn(const Pawn* pawn);
    void DrawCylinder(const float baseRadius, const float topRadius, const float height, const int slices = 10, const int stacks = 10);
    void DrawSphere(double radius, int slices = 10, int stacks = 10);
    void DrawBox(double width, double height, double depth);
    void DrawAxis(float axisLength);
};


#endif
