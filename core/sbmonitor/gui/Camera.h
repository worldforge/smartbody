#ifndef CAMERA_H_
#define CAMERA_H_

#include <QVector3D>
#include <QMatrix4x4>

using std::string;

class Camera
{
public:
   enum CameraControlType
   {
      Follow_Renderer,
      Free_Look,
      Follow_Character
   };

   Camera();
   ~Camera();

   void Draw();
   void Rotate(const QVector3D& offset);

   // used for moving the camera along it's orientation vectors
   void MoveX(float offset);
   void MoveY(float offset);
   void MoveZ(float offset);
   void Translate(const QVector3D& vec);
   void SetCameraType(const string& type);
   void SetCameraType(CameraControlType type) { m_CameraType = type; }
   void SetRightHanded(bool val) { m_Scale.setZ(val ? 1 : -1); }
   void SetLookAtOffset(double offset) { m_LookAtOffset = offset; }
   void SetMouseRotation(bool tf) { m_MouseRotation = tf; }
   QVector3D GetPosition() { return QVector3D(m_CameraTransformation.column(3)); }

   void SetPosition(const QVector3D& pos) 
   { 
      m_CameraTransformation.setColumn(3, QVector4D(pos, 1.0f));
      m_BaseOrientation.setColumn(3, QVector4D(pos, 1.0f));
   }
   void SetRotation(const QQuaternion& rot); 
   CameraControlType GetCameraType() { return m_CameraType; }
   bool FollowRenderer() { return m_CameraType == Follow_Renderer; }

   void SetMovementSpeed(double movementSpeed) { m_MovementSpeed = movementSpeed; }
   void SetRotationSpeed(double rotationSpeed) { m_RotationSpeed = rotationSpeed; } 
   double GetMovementSpeed() { return m_MovementSpeed; }
   double GetRotationSpeed() { return m_RotationSpeed; }
   double CoordConverter() { return -1; }
   bool GetMouseRotation() { return m_MouseRotation; } 

   void LookAt(const QVector3D& pos);
   void MoveLookAt(const QVector3D lookAtPos);

private:
   QVector3D m_Position;
   QVector3D m_Scale;
   QMatrix4x4 m_CameraTransformation;
   QMatrix4x4 m_BaseOrientation;

   CameraControlType m_CameraType;
   double m_MovementSpeed;
   double m_RotationSpeed;
   double m_LookAtOffset;
   double m_RotX, m_RotY, m_RotZ;
   bool m_MouseRotation;
};

#endif