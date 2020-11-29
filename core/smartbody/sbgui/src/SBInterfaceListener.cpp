#include "SBInterfaceListener.h"
#include <sb/SBNavigationMesh.h>
#include <sb/SBScene.h>
#include "SBSelectionManager.h"
#include "Session.h"
#include <sr/sr_plane.h>
#include <sr/sr_camera.h>

SBInterfaceListener::SBInterfaceListener()
{
}

SBInterfaceListener::~SBInterfaceListener()
{
}

void SBInterfaceListener::onStart()
{
}

bool SBInterfaceListener::onMouseClick(int x, int y, int button)
{
	return false;
}

bool SBInterfaceListener::onMouseMove(int x, int y)
{
	return false;
}

bool SBInterfaceListener::onMouseRelease(int x, int y, int button)
{
	return false;
}

bool SBInterfaceListener::onMouseDrag(int x, int y)
{
	return false;
}

bool SBInterfaceListener::onKeyboardPress(char c)
{
	return false;
}

bool SBInterfaceListener::onKeyboardRelease(char c)
{
	return false;
}

void SBInterfaceListener::onEnd()
{
}





SBInterfaceManager* SBInterfaceManager::_interfaceManager = nullptr;

SBInterfaceManager::SBInterfaceManager()
{
	screenWidth = screenHeight = -1;
}


void SBInterfaceManager::resize( int w, int h )
{
	screenWidth = w;
	screenHeight = h;
}




SBInterfaceManager::~SBInterfaceManager()
{
	std::vector<SBInterfaceListener*> listeners = getInterfaceListeners();
	for (auto & listener : listeners)
	{
		this->removeInterfaceListener(listener);
	}
}

void SBInterfaceManager::addInterfaceListener(SBInterfaceListener* listener)
{
	for (auto & _interfaceListener : _interfaceListeners)
	{
		if (_interfaceListener == listener)
			return;
	}
	_interfaceListeners.emplace_back(listener);
	listener->onStart();
}

void SBInterfaceManager::removeInterfaceListener(SBInterfaceListener* listener)
{
	for (auto iter = _interfaceListeners.begin();
		 iter != _interfaceListeners.end(); 
		 iter++)
	{
		if ((*iter) == listener)
		{
			(*iter)->onEnd();
			_interfaceListeners.erase(iter);
			return;
		}
	}
}

std::vector<SBInterfaceListener*> SBInterfaceManager::getInterfaceListeners()
{
	std::vector<SBInterfaceListener*> listeners;
	for (auto & _interfaceListener : _interfaceListeners)
	{
		listeners.emplace_back(_interfaceListener);
	}

	return listeners;
}

SrVec SBInterfaceManager::convertScreenSpaceTo3D(int x, int y, SrVec ground, SrVec upVector)
{
	SrVec p1;
	SrVec p2;
	float screenX, screenY;
	screenX = (float)(x*2.f)/screenWidth - 1.f;
	screenY = (float)(y*2.f)/screenHeight - 1.f;
	screenY *= -1.0;

	Session::current->renderScene.getActiveCamera()->get_ray(screenX, screenY, p1, p2);
	//SmartBody::util::log("mouse click = %d, %d,   p1 = %f %f %f, p2 = %f %f %f", x,y, p1[0], p1[1], p1[2],  p2[0], p2[1], p2[2]);
	bool intersectGround = true;
	SrVec dest, src;				
	SrPlane plane(ground, upVector);
	dest = plane.intersect(p1, p2);
	return dest;
}

std::string SBInterfaceManager::getSelectedObject()
{
	return SBSelectionManager::getSelectionManager()->getCurrentSelection();
}

SBInterfaceManager* SBInterfaceManager::getInterfaceManager()
{
	if (SBInterfaceManager::_interfaceManager == nullptr)
	{
		SBInterfaceManager::_interfaceManager = new SBInterfaceManager();
	}

	return SBInterfaceManager::_interfaceManager;
}
