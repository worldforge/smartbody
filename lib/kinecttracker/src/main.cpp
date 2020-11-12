/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include "SceneDrawer.h"
#include "vhmsg-tt.h"
#include <math.h>
#include <sstream>
#include <iostream>

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
xn::Context g_Context;
xn::DepthGenerator g_DepthGenerator;
xn::UserGenerator g_UserGenerator;

XnBool g_bNeedPose = FALSE;
XnChar g_strPose[20] = "";
XnBool g_bDrawBackground = TRUE;
XnBool g_bDrawPixels = TRUE;
XnBool g_bDrawSkeleton = TRUE;
XnBool g_bPrintID = TRUE;
XnBool g_bPrintState = TRUE;

bool	tracking = false;

#ifndef USE_GLES
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif
#else
	#include "opengles.h"
#endif

#ifdef USE_GLES
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;
#endif

#define GL_WIN_SIZE_X 720
#define GL_WIN_SIZE_Y 480

XnBool g_bPause = false;
XnBool g_bRecord = false;

XnBool g_bQuit = false;

bool g_initSkeleton = true;
int  g_initSkeletonSampleCount = 0;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

void CleanupExit()
{
	g_Context.Shutdown();

	exit (1);
}

// Callback: New user was detected
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	printf("New User %d\n", nId);
	// New user found
	if (g_bNeedPose)
	{
		g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
	}
	else
	{
		g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
	}
}
// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	tracking = false;
	printf("Lost user %d\n", nId);
}
// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
	tracking = true;
	printf("Pose %s detected for user %d\n", strPose, nId);
	g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}
// Callback: Started calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
	tracking = false;
	printf("Calibration started for user %d\n", nId);
}
// Callback: Finished calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie)
{
	if (bSuccess)
	{
		// Calibration succeeded
		printf("Calibration complete, start tracking user %d\n", nId);		
		g_UserGenerator.GetSkeletonCap().StartTracking(nId);
		g_initSkeleton = false;
	}
	else
	{
		// Calibration failed
		printf("Calibration failed for user %d\n", nId);
		if (g_bNeedPose)
		{
			g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
		}
		else
		{
			g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

#define XN_CALIBRATION_FILE_NAME "UserCalibration.bin"

// Save calibration to file
void SaveCalibration()
{
	XnUserID aUserIDs[20] = {0};
	XnUInt16 nUsers = 20;
	g_UserGenerator.GetUsers(aUserIDs, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		// Find a user who is already calibrated
		if (g_UserGenerator.GetSkeletonCap().IsCalibrated(aUserIDs[i]))
		{
			// Save user's calibration to file
			g_UserGenerator.GetSkeletonCap().SaveCalibrationDataToFile(aUserIDs[i], XN_CALIBRATION_FILE_NAME);
			break;
		}
	}
}
// Load calibration from file
void LoadCalibration()
{
	XnUserID aUserIDs[20] = {0};
	XnUInt16 nUsers = 20;
	g_UserGenerator.GetUsers(aUserIDs, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		// Find a user who isn't calibrated or currently in pose
		if (g_UserGenerator.GetSkeletonCap().IsCalibrated(aUserIDs[i])) continue;
		if (g_UserGenerator.GetSkeletonCap().IsCalibrating(aUserIDs[i])) continue;

		// Load user's calibration from file
		XnStatus rc = g_UserGenerator.GetSkeletonCap().LoadCalibrationDataFromFile(aUserIDs[i], XN_CALIBRATION_FILE_NAME);
		if (rc == XN_STATUS_OK)
		{
			// Make sure state is coherent
			g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(aUserIDs[i]);
			g_UserGenerator.GetSkeletonCap().StartTracking(aUserIDs[i]);
		}
		break;
	}
}


void matrixToQuat(XnSkeletonJointOrientation& m, float& qw, float& qx, float& qy, float& qz)
{
	// flip z-axis
	float X1 = m.orientation.elements[0];
	float X2 = m.orientation.elements[3];
	float X3 = -m.orientation.elements[6];
	float Y1 = m.orientation.elements[1];
	float Y2 = m.orientation.elements[4];
	float Y3 = -m.orientation.elements[7];
	float Z1 = -m.orientation.elements[2];
	float Z2 = -m.orientation.elements[5];
	float Z3 = m.orientation.elements[8];
    float tr = X1 + Y2 + Z3;
    qw = 1.0f;
    qx = 0.0f;
    qy = 0.0f;
    qz = 0.0f;

    if (tr > 0)
    {
        float S = sqrt(tr+1.0f) * 2.0f; // S=4*qw
        qw = 0.25f * S;
        qx = (Y3 - Z2) / S; //(m21 - m12) / S;
        qy = (Z1 - X3) / S; //(m02 - m20) / S;
        qz = (X2 - Y1) / S; //(m10 - m01) / S;
    } 
	else if ((X1 > Y2) && (X1 > Z3))//((m00 > m11)&(m00 > m22))
    {
         float S = sqrt(1.0f + X1 - Y2 - Z3) * 2.0f; //sqrt(1.0 +m00 - m11 - m22) * 2; // S=4*qx
         qw = (Y3 - Z2) / S;//(m21 - m12) / S;
         qx = 0.25f * S;
         qy = (Y1 + X2) / S;//(m01 + m10) / S;
         qz = (Z1 + X3) / S;//(m02 + m20) / S;
    } 
	else if (Y2 > Z3)//(m11 > m22)
    {
        float S = sqrt(1.0f + Y2 - X1 - Z3) * 2.0f;//sqrt(1.0 +m11 - m00 - m22) * 2; // S=4*qy
        qw = (Z1 - X3) / S; //(m02 - m20) / S;
        qx = (Y1 + X2) / S; //(m01 + m10) / S;
        qy = 0.25f * S;
        qz = (Z2 + Y3) / S; //(m12 + m21) / S;
    } 
	else
    {
        float S = sqrt(1.0f + Z3 - X1 - Y2) * 2.0f;//sqrt(1.0 +m22 - m00 - m11) * 2; // S=4*qz
        qw = (X2 - Y1) / S;//(m10 - m01) / S;
        qx = (Z1 + X3) / S;//(m02 + m20) / S;
        qy = (Z2 + Y3) / S;//(m12 + m21) / S;
        qz = 0.25f * S;
    }

	// getting conjugate
	qx *= -1.0f;
	qy *= -1.0f;
	qz *= -1.0f;

	if (fabs(qw) > 1 || fabs(qx) > 1 || fabs(qy) > 1 || fabs(qz) > 1)
	{
		qw = 0.0f;
		qx = 0.0f;
		qy = 0.0f;
		qz = 0.0f;
	}
} 

// this function is called each frame
void glutDisplay (void)
{

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup the OpenGL viewpoint
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	xn::SceneMetaData sceneMD;
	xn::DepthMetaData depthMD;
	g_DepthGenerator.GetMetaData(depthMD);
#ifndef USE_GLES
	glOrtho(0, depthMD.XRes(), depthMD.YRes(), 0, -1.0, 1.0);
#else
	glOrthof(0, depthMD.XRes(), depthMD.YRes(), 0, -1.0, 1.0);
#endif

	glDisable(GL_TEXTURE_2D);

	if (!g_bPause)
	{
		// Read next available data
		g_Context.WaitOneUpdateAll(g_DepthGenerator);
	}

	// Process the data
	g_DepthGenerator.GetMetaData(depthMD);
	g_UserGenerator.GetUserPixels(0, sceneMD);
	DrawDepthMap(depthMD, sceneMD);

	XnUserID aUsers[15];
	XnUInt16 nUsers = 15;
	g_UserGenerator.GetUsers(aUsers, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		if (g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i]))
		{
/*
			HipCenter				0
			Spine					1
			ShoulderCenter			2
			Head					3
			ShoulderLeft			4
			ElbowLeft				5
			WristLeft				6
			HandLeft				7
			ShoulderRight			8
			ElbowRight				9
			WristRight				10
			HandRight				11
			HipLeft					12
			KneeLeft				13
			AnkleLeft				14
			FootLeft				15
			HipRight				16
			KneeRight				17
			AnkleRight				18
			FootRight				19
*/
			// rotation data
			XnSkeletonJointOrientation jointInfo[24];
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_WAIST, jointInfo[0]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_TORSO, jointInfo[1]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_NECK, jointInfo[2]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_HEAD, jointInfo[3]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_LEFT_SHOULDER, jointInfo[4]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_LEFT_ELBOW, jointInfo[5]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_LEFT_WRIST, jointInfo[6]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_LEFT_HAND, jointInfo[7]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_RIGHT_SHOULDER, jointInfo[8]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_RIGHT_ELBOW, jointInfo[9]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_RIGHT_WRIST, jointInfo[10]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_RIGHT_HAND, jointInfo[11]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_LEFT_HIP, jointInfo[12]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_LEFT_KNEE, jointInfo[13]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_LEFT_ANKLE, jointInfo[14]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_LEFT_FOOT, jointInfo[15]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_RIGHT_HIP, jointInfo[16]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_RIGHT_KNEE, jointInfo[17]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_RIGHT_ANKLE, jointInfo[18]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_RIGHT_FOOT, jointInfo[19]);
			
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_LEFT_COLLAR, jointInfo[20]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_RIGHT_COLLAR, jointInfo[21]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_LEFT_FINGERTIP, jointInfo[22]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(aUsers[i], XN_SKEL_RIGHT_FINGERTIP, jointInfo[23]);

			

			// need a way to customize skeleton name here
			std::string skeletonName;
			if (i == 0)
				skeletonName = "utah";	

			// switch to quaternion
			float w, x, y, z;
			std::stringstream command;
			
			if (!g_initSkeleton)
			{							
				g_initSkeletonSampleCount++;
				// get global position info
				XnSkeletonJointPosition posInfo[24];
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_WAIST, posInfo[0]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_TORSO, posInfo[1]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_NECK, posInfo[2]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_HEAD, posInfo[3]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_SHOULDER, posInfo[4]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_ELBOW, posInfo[5]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_WRIST, posInfo[6]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_HAND, posInfo[7]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_SHOULDER, posInfo[8]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_ELBOW, posInfo[9]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_WRIST, posInfo[10]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_HAND, posInfo[11]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_HIP, posInfo[12]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_KNEE, posInfo[13]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_ANKLE, posInfo[14]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_FOOT, posInfo[15]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_HIP, posInfo[16]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_KNEE, posInfo[17]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_ANKLE, posInfo[18]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_FOOT, posInfo[19]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_COLLAR, posInfo[20]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_COLLAR, posInfo[21]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_FINGERTIP, posInfo[22]);
				g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_FINGERTIP, posInfo[23]);				
				
				if (g_initSkeletonSampleCount >= 50)
				{
					vhmsg::ttu_notify2("receiver", "echo init Kinect Skeleton...");	
					command << "skeleton " << skeletonName << " kinect initsk ";
					float px,py,pz;
					for (int j = 0; j < 24; j++)
					{
						matrixToQuat(jointInfo[j], w, x, y, z);		
						if (jointInfo[j].fConfidence < 0.f)
						{
							command<< 1.f << " " << 0.f << " " << 0.f << " " << 0.f << " ";
						}
						else
						{
							command << w << " " << x << " " << y << " " << z << " ";
						}						
						if (posInfo[j].fConfidence < 0.5)
						{
							//printf("joint %d, low confidence = %f, position = %f %f %f\n",j,posInfo[j].fConfidence, posInfo[j].position.X,posInfo[j].position.Y,posInfo[j].position.Z);
							command << 0.f << " " << 0.f << " " << 0.f << " ";

						}
						else 
						{
							//printf("joint %d, high confidence = %f, position = %f %f %f\n",j,posInfo[j].fConfidence, posInfo[j].position.X,posInfo[j].position.Y,posInfo[j].position.Z);
							command << posInfo[j].position.X << " " << posInfo[j].position.Y << " " << posInfo[j].position.Z << " ";
						}
					}
				}
				
				if (g_initSkeletonSampleCount >= 80)
				{
					g_initSkeleton = true;
					g_initSkeletonSampleCount = 0;
				}				
			}
			else
			{
				command << "skeleton " << skeletonName << " kinect rotations ";
				for (int j = 0; j < 24; j++)
				{
					matrixToQuat(jointInfo[j], w, x, y, z);
					command << w << " " << x << " " << y << " " << z << " ";
				}
			}

			vhmsg::ttu_notify2("receiver", command.str().c_str());
#if 0
			// translation data
			XnSkeletonJointPosition posInfo[24];
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_WAIST, posInfo[0]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_TORSO, posInfo[1]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_NECK, posInfo[2]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_HEAD, posInfo[3]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_SHOULDER, posInfo[4]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_ELBOW, posInfo[5]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_WRIST, posInfo[6]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_HAND, posInfo[7]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_SHOULDER, posInfo[8]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_ELBOW, posInfo[9]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_WRIST, posInfo[10]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_HAND, posInfo[11]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_HIP, posInfo[12]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_KNEE, posInfo[13]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_ANKLE, posInfo[14]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_FOOT, posInfo[15]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_HIP, posInfo[16]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_KNEE, posInfo[17]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_ANKLE, posInfo[18]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_FOOT, posInfo[19]);
			
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_COLLAR, posInfo[20]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_COLLAR, posInfo[21]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_FINGERTIP, posInfo[22]);
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_FINGERTIP, posInfo[23]);

			std::stringstream command1;
			command1 << "skeleton " << skeletonName << " kinect positions ";
			for (int j = 0; j < 24; j++)
				command1 << posInfo[j].position.X << " " << posInfo[j].position.Y << " " << posInfo[j].position.Z;
			vhmsg::ttu_notify2("receiver", command1.str().c_str());
#else
			XnSkeletonJointPosition base;
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_TORSO, base);
			std::stringstream command1;
			command1 << "skeleton " << skeletonName << " kinect position 0 ";
			command1 << base.position.X << " " << base.position.Y << " " << base.position.Z;
			vhmsg::ttu_notify2("receiver", command1.str().c_str());
#endif
		}
	}

#ifndef USE_GLES
	glutSwapBuffers();
#endif
}

#ifndef USE_GLES
void glutIdle (void)
{
	if (g_bQuit) {
		CleanupExit();
	}

	// Display the frame
	glutPostRedisplay();
}

void glutKeyboard (unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		CleanupExit();
	case 'b':
		// Draw background?
		g_bDrawBackground = !g_bDrawBackground;
		break;
	case 'x':
		// Draw pixels at all?
		g_bDrawPixels = !g_bDrawPixels;
		break;
	case 's':
		// Draw Skeleton?
		g_bDrawSkeleton = !g_bDrawSkeleton;
		break;
	case 'i':
		// Print label?
		g_bPrintID = !g_bPrintID;
		break;
	case 'l':
		// Print ID & state as label, or only ID?
		g_bPrintState = !g_bPrintState;
		break;
	case'p':
		g_bPause = !g_bPause;
		break;
	case 'S':
		SaveCalibration();
		break;
	case 'L':
		LoadCalibration();
		break;
	}
}
void glInit (int * pargc, char ** argv)
{
	glutInit(pargc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutCreateWindow ("Kinect Tracker for SmartBody");
	//glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}
#endif // USE_GLES

#define SAMPLE_XML_PATH "../Data/SamplesConfig.xml"

#define CHECK_RC(nRetVal, what)										\
	if (nRetVal != XN_STATUS_OK)									\
	{																\
		printf("%s failed: %s\n", what, xnGetStatusString(nRetVal));\
		return nRetVal;												\
	}

int main(int argc, char **argv)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (argc > 1)
	{
		nRetVal = g_Context.Init();
		CHECK_RC(nRetVal, "Init");
		nRetVal = g_Context.OpenFileRecording(argv[1]);
		if (nRetVal != XN_STATUS_OK)
		{
			printf("Can't open recording %s: %s\n", argv[1], xnGetStatusString(nRetVal));
			return 1;
		}
	}
	else
	{
		xn::EnumerationErrors errors;
		nRetVal = g_Context.InitFromXmlFile(SAMPLE_XML_PATH, &errors);
		if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
		{
			XnChar strError[1024];
			errors.ToString(strError, 1024);
			printf("%s\n", strError);
			return (nRetVal);
		}
		else if (nRetVal != XN_STATUS_OK)
		{
			printf("Open failed: %s\n", xnGetStatusString(nRetVal));
			return (nRetVal);
		}
	}

	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	CHECK_RC(nRetVal, "Find depth generator");
	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
	if (nRetVal != XN_STATUS_OK)
	{
		nRetVal = g_UserGenerator.Create(g_Context);
		CHECK_RC(nRetVal, "Find user generator");
	}

	XnCallbackHandle hUserCallbacks, hCalibrationCallbacks, hPoseCallbacks;
	if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
	{
		printf("Supplied user generator doesn't support skeleton\n");
		return 1;
	}
	g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
	g_UserGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(UserCalibration_CalibrationStart, UserCalibration_CalibrationEnd, NULL, hCalibrationCallbacks);
	g_UserGenerator.GetSkeletonCap().SetSmoothing(0.8f);
	if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
	{
		g_bNeedPose = TRUE;
		if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
		{
			printf("Pose required, but not supported\n");
			return 1;
		}
		g_UserGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(UserPose_PoseDetected, NULL, NULL, hPoseCallbacks);
		g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
	}

	g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

	nRetVal = g_Context.StartGeneratingAll();
	CHECK_RC(nRetVal, "StartGenerating");

	// activemq connection
	if (vhmsg::ttu_open() == vhmsg::TTU_SUCCESS)
		vhmsg::ttu_notify2("receiver", "echo Kinect Connected...");
#ifndef USE_GLES
	glInit(&argc, argv);
	glutMainLoop();
#else
	if (!opengles_init(GL_WIN_SIZE_X, GL_WIN_SIZE_Y, &display, &surface, &context))
	{
		printf("Error initializing opengles\n");
		CleanupExit();
	}

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	while (!g_bQuit)
	{
		glutDisplay();
		eglSwapBuffers(display, surface);
	}
	opengles_shutdown(display, surface, context);

	CleanupExit();
#endif
}
