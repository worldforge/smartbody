
#ifndef SBMDEBUGGERCOMMON_H
#define SBMDEBUGGERCOMMON_H

#include <cmath>
#include <string>
#include <vector>

#include "vhcl_public.h"


class Joint
{
//private:
public:
   std::string m_name;

   vhcl::Vector3 posOrig;
   vhcl::Vector4 rotOrig;

   vhcl::Vector3 pos;
   vhcl::Vector4 rot;

   std::vector<Joint *> m_joints;
   Joint * m_parent;

public:
   Joint();
   virtual ~Joint();

   vhcl::Vector3 GetWorldPosition() const;
   vhcl::Vector3 GetLocalPosition() const;
   vhcl::Vector4 GetWorldRotation() const;
   vhcl::Vector4 GetLocalRotation() const;

   std::string GetPositionAsString(bool worldPos) const;
   std::string GetRotationAsString(bool worldRot) const;
};

enum PawnShape
{
   Box,
   Capsule,
   Sphere,
};

class Pawn
{
//private:
public:
   std::string m_name;
   std::vector<Joint *> m_joints;
   double m_size;
   PawnShape m_shape;

public:
   Pawn()
   {
   }

   virtual ~Pawn()
   {
   }

   static PawnShape StringToPawnShape(std::string& s);
   vhcl::Vector3 GetWorldPosition() const;
   Joint* GetWorldOffset() const;
   Joint * FindJoint(const std::string & name) const { return FindJoint(name, m_joints); } 
   static Joint * FindJoint(const std::string & name, const std::vector<Joint *> & joints);
};


class Character : public Pawn
{
public:
   Character() : Pawn()
   {
   }

   virtual ~Character()
   {
   }
};


class DebuggerCamera
{
//private:
public:
   vhcl::Vector3 pos;
   vhcl::Vector4 rot;
   double fovY;
   double aspect;
   double zNear;
   double zFar;

public:
   DebuggerCamera()
   {
      pos.x = 0;
      pos.y = 200;
      pos.z = 350;
      //rot.x = 0.12f;
      //rot.y = 0.004f;
      //rot.z = 0;
      //rot.w = 0.992999f;
      rot.x = 0;
      rot.y = 0;
      rot.z = 0;
      rot.w = 1.0f;
      fovY = 45;
      aspect = 1.5;
      zNear = 0.1;
      zFar = 1000;
   }

   virtual ~DebuggerCamera() {}
};


class Scene
{
//private:
public:
   std::vector<Pawn> m_pawns;
   std::vector<Character> m_characters;
   DebuggerCamera m_camera;
   bool m_rendererIsRightHanded;

public:
   Scene()
   {
      m_rendererIsRightHanded = true;
   }

   virtual ~Scene()
   {
   }

   Character* FindCharacter(const std::string & name);
   Pawn* FindPawn(const std::string & name);
   Pawn* FindSbmObject(const std::string & name);
};



#endif  // SBMDEBUGGERCOMMON_H
