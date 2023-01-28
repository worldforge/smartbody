#include "Camera.h"
#include <QtOpenGL>

QVector3D WorldUp(0, 1, 0);

Camera::Camera() :
   m_Scale(1, 1, 1),
   m_RotX(0),
   m_RotY(0),
   m_RotZ(0),
   m_CameraType(Follow_Renderer),
   m_MovementSpeed(5),
   m_RotationSpeed(0.2f),
   m_LookAtOffset(100),
   m_MouseRotation(false)
{
   m_CameraTransformation.setToIdentity();
   m_BaseOrientation.setToIdentity();
   SetPosition(QVector3D(0, 0.2f, 3));
}

Camera::~Camera()
{
  
}

void Camera::Draw()
{
   glScaled(m_Scale.x(), m_Scale.y(), m_Scale.z());
   QMatrix4x4 viewMat = m_CameraTransformation.inverted();
   glMultMatrixd(viewMat.data());  
}


void Camera::Rotate(const QVector3D& offset)
{
   m_RotX += offset.x();
   m_RotY += offset.y();
   m_RotZ += offset.z();

   QVector3D pos = GetPosition();
   m_CameraTransformation = m_BaseOrientation;
   
   QVector4D col0 = m_CameraTransformation.column(0);
   QVector4D col1 = m_CameraTransformation.column(1);

   QVector3D side(col0.x(), col0.y(), col0.z());
   QVector3D up(col1.x(), col1.y(), col1.z());

   // create quats from axis angles
   QQuaternion xQuat = QQuaternion::fromAxisAndAngle(side, m_RotX);
   QQuaternion yQuat = QQuaternion::fromAxisAndAngle(up, m_RotY);

   // perform rotation
   QQuaternion combined = yQuat * xQuat;
   m_CameraTransformation.rotate(combined);
   SetPosition(pos);
}

void Camera::MoveX(float offset)
{
   QVector3D right(m_BaseOrientation.column(0));
   Translate(right * offset);
}

void Camera::MoveY(float offset)
{
   QVector3D up(m_BaseOrientation.column(1));
   Translate(up * offset);
}

void Camera::MoveZ(float offset)
{
   QVector3D forward(m_BaseOrientation.column(2));
   Translate(forward * offset);
}

void Camera::Translate(const QVector3D& vec)
{
   m_CameraTransformation.translate(vec);
   m_BaseOrientation.translate(vec);
}

void Camera::SetCameraType(const string& type)
{
   if (type == "Follow Renderer")
   {
      m_CameraType = Follow_Renderer;
   }
   else if (type == "Free Look")
   {
      m_CameraType = Free_Look;
      m_RotX = m_RotY = m_RotZ = 0;
   }
   else
      printf("Camera type %s doesn't exist", type.c_str());
}

void Camera::SetRotation(const QQuaternion& rot)
{
   QMatrix4x4 newMat;
   newMat.setToIdentity();
   newMat.translate(GetPosition());
   newMat.rotate(rot);
   m_BaseOrientation = m_CameraTransformation = newMat;
   m_RotX = m_RotY = m_RotZ = 0;
}

void Camera::LookAt(const QVector3D& pos)
{
   // this function handles both left and right handed coordinate systems
   // set the forward vector
   QVector3D forward = (pos - GetPosition()).normalized();
   m_CameraTransformation.setColumn(2, forward * -m_Scale.z());

   // set right vector
   QVector3D right = QVector3D::crossProduct(forward, QVector3D(0, 1, 0));
   m_CameraTransformation.setColumn(0, right * m_Scale.z());

   QVector3D up = QVector3D::crossProduct(right, forward);
   m_CameraTransformation.setColumn(1, up);
   //m_BaseOrientation = m_CameraTransformation;
   m_RotX = m_RotY = m_RotZ = 0;
}

void Camera::MoveLookAt(const QVector3D lookAtPos)
{
   LookAt(lookAtPos);

   // get forward vector
   QVector4D vec = m_CameraTransformation.column(2); 
   SetPosition(lookAtPos + ((QVector3D)vec * m_LookAtOffset));
}
