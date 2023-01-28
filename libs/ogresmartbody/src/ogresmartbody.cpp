#include "ogresmartbody.h"
#include "ogresmartbodylistener.h" 

#include <sb/SBPython.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include "SBUtilities.h"
#include <sb/SBPawn.h>
#include <sr/sr_camera.h>
#include <sr/sr_light.h>
#include <sr/sr_euler.h>
#include <sb/SBAttribute.h>
#include <sr/sr.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <boost/filesystem/convenience.hpp>

std::string mediaPath = "";
std::string originalScript = "";
std::string originalPath = "";

//-------------------------------------------------------------------------------------
OgreSmartBody::OgreSmartBody()
    : mRoot(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mWindow(0),
    mSceneMgr(0),
    mCamera(0),
	m_pScene(NULL)
{
	firstTime = true;
}
//-------------------------------------------------------------------------------------
OgreSmartBody::~OgreSmartBody()
{
    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	if (mWindow)
		windowClosed(mWindow);
    delete mRoot;
}

Ogre::SceneManager* OgreSmartBody::getSceneManager()
{
	return mSceneMgr;
}
 
 
bool OgreSmartBody::go()
{
#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
	mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif
 
    // construct Ogre::Root
    mRoot = new Ogre::Root(mPluginsCfg, "", "ogre.log");
 
    // setup resources
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);
 
    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
 
    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
    // configure
    // Show the configuration dialog and initialise the system
    if(!(mRoot->restoreConfig() || mRoot->showConfigDialog(NULL)))
    {
        return false;
    }
 
    mWindow = mRoot->initialise(true, "OgreSmartBody Render Window");
 
    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    // initialise all resource groups
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
 
    // Create the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager("DefaultSceneManager");
 
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");
	Ogre::SceneNode* cameraNode = mSceneMgr->createSceneNode();
	cameraNode->attachObject(mCamera);
 
    // Position it at 500 in Z direction
	cameraNode->setPosition(Ogre::Vector3(0,10,60));
    // Look back along -Z
	cameraNode->lookAt(Ogre::Vector3(0,0,-300), Ogre::Node::TS_WORLD);
    mCamera->setNearClipDistance(5);
 
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
 
    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	// create a floor for better visualization
		//adding plane entity to the scene
	Ogre::Plane plane;
	plane.normal = Ogre::Vector3::UNIT_Y;
	plane.d = 0;
	Ogre::MeshManager::getSingleton().createPlane( "Myplane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1500, 1500, 20, 20, true, 1, 60, 60, Ogre::Vector3::UNIT_Z );
	Ogre::Entity * pPlaneEnt = mSceneMgr->createEntity( "plane", "Myplane" );
	//pPlaneEnt->setMaterialName( "Examples/Rockwall" );
	pPlaneEnt->setMaterialName( "Rockwall" );
	pPlaneEnt->setCastShadows( false );
	mSceneMgr->getRootSceneNode()->createChildSceneNode("plane_node", Ogre::Vector3( 0, 0, 0 ) )->attachObject( pPlaneEnt );
	mSceneMgr->getSceneNode("plane_node")->setVisible(true);

	// shadows
	mSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_TEXTURE_MODULATIVE );
	mSceneMgr->setShadowTextureSize( 4048 );
	mSceneMgr->setShadowColour( Ogre::ColourValue( 0.3f, 0.3f, 0.3f ) );
  
    // Set ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
 
    // Create a light
    Ogre::Light* light = mSceneMgr->createLight("MainLight");
	Ogre::SceneNode* lightNode = mSceneMgr->createSceneNode();
	lightNode->attachObject(light);

	lightNode->setPosition(20,80,50);
 
   
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
 
    
    windowResized(mWindow);
 
    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
 
	mWindow->setVSyncEnabled(true);
    mRoot->addFrameListener(this);

	// smartbody
	// the root path to SmartBody: change this to your own path
#ifdef MAINSBBUILD
	std::string smartbodyRoot = "e:/smartbody/trunk/"; // ".. / .. / .. / ..";
#else
	std::string smartbodyRoot = "..";
#endif
	// set the following to the location of the Python libraries. 
	// if you downloaded SmartBody, it will be in libs/smartbody/Python27/Lib
#ifdef WIN32
#ifdef MAINSBBUILD
	initPython(smartbodyRoot + "/libs/smartbody/Python27/lib");
#else
	initPython(smartbodyRoot + "/Python27/lib");
#endif
#else
	initPython("/usr/lib/python2.7");
#endif
	m_pScene = SmartBody::SBScene::getScene();

	m_pScene->startFileLogging("smartbody.log");

	SmartBody::util::log("Script = %s", originalScript.c_str());
	SmartBody::util::log("Path = %s", originalPath.c_str());

	OgreSmartBodyListener* listener = new OgreSmartBodyListener(this);
	m_pScene->addSceneListener(listener);
	m_pScene->start();

	// sets the media path, or root of all the data to be used
	// other paths will be relative to the media path
	m_pScene->setMediaPath(mediaPath);
	m_pScene->addAssetPath("script", originalPath);
	// the file 'OgreSmartBody.py' needs to be placed in the media path directory
	m_pScene->runScript(originalScript);
	
	mStartTime = Ogre::Root::getSingleton().getTimer()->getMilliseconds() / 1000.0f;
	mRoot->startRendering();
 
    return true;
}
 
//Adjust mouse clipping area
void OgreSmartBody::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
 
   }
 
//Unattach OIS before window shutdown (very important under Linux)
void OgreSmartBody::windowClosed(Ogre::RenderWindow* rw)
{
   
}
 
bool OgreSmartBody::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;
 
 
	// smartbody
	if (!m_pScene)
		return true;


	SmartBody::SBSimulationManager* sim = m_pScene->getSimulationManager();
	sim->setTime((Ogre::Root::getSingleton().getTimer()->getMilliseconds() / 1000.0f) - mStartTime);
	m_pScene->update();

	//updateOgreCamera();
	//updateOgreLights();

	int numCharacters = m_pScene->getNumCharacters();
	if (numCharacters == 0)
		return true;

	const std::vector<std::string>& characterNames = m_pScene->getCharacterNames();

	for (size_t n = 0; n < characterNames.size(); n++)
	{
		SmartBody::SBCharacter* character = m_pScene->getCharacter(characterNames[n]);
		if (!this->getSceneManager()->hasEntity(characterNames[n]))
			continue;

		Ogre::Entity* entity = this->getSceneManager()->getEntity(characterNames[n]);
		Ogre::Skeleton* meshSkel = entity->getSkeleton();
		Ogre::Node* node = entity->getParentNode();


		SrVec pos = character->getPosition();
		SrQuat ori = character->getOrientation();
		//std::cout << ori.w << ori.x << " " << ori.y << " " << ori.z << std::endl;
		node->setPosition(pos.x, pos.y, pos.z);
		node->setOrientation(ori.w, ori.x, ori.y, ori.z);

		// update the blend shapes
		ogreBlendShape(entity, character->dMeshInstance_p);
	
		// Update joints
		auto sbSkel = character->getSkeleton();
			
		int numJoints = sbSkel->getNumJoints();
		for (int j = 0; j < numJoints; j++)
		{
			SmartBody::SBJoint* joint = sbSkel->getJoint(j);
	
			try
			{
				SrQuat orientation = joint->quat()->value();
	
				Ogre::Vector3 posDelta(joint->getPosition().x, joint->getPosition().y, joint->getPosition().z);
				Ogre::Quaternion quatDelta(orientation.w, orientation.x, orientation.y, orientation.z);
				Ogre::Bone* bone = meshSkel->getBone(joint->getName());
				if (!bone)
				{
					continue;
				}
				
				if (!bone->isManuallyControlled())
					bone->setManuallyControlled(true);
				bone->setPosition(bone->getInitialPosition() + posDelta);
				bone->setOrientation(quatDelta);
			
			}
			catch (Ogre::ItemIdentityException& ex)
			{
				//SmartBody::util::log("%s", ex.getFullDescription().c_str());
				// Should not happen as we filtered using m_mValidBones
			}
		}
	}
	
	firstTime = false;

 
    return true;
}

void OgreSmartBody::ogreBlendShape(Ogre::Entity* sbEntity, DeformableMeshInstance* meshInstance)
{
	if (!meshInstance)
		return;

	DeformableMesh* defMesh = meshInstance->getDeformableMesh();
	if (!defMesh) return;
	if (defMesh->blendShapeMap.size() == 0) return;  // no blend shape

	Ogre::MeshPtr meshPtr = sbEntity->getMesh();

	std::map<std::string, std::vector<SrSnModel*> >::iterator mIter;
	mIter = defMesh->blendShapeMap.begin();
	SrSnModel* writeToBaseModel = NULL;
	int vtxBaseIdx = 0;
	for (size_t i = 0; i < defMesh->dMeshStatic_p.size(); ++i)
	{
		if (strcmp(defMesh->dMeshStatic_p[i]->shape().name, mIter->first.c_str()) == 0)
		{
			writeToBaseModel = defMesh->dMeshStatic_p[i];
			break;
		}
		else
		{
			// skip vertices for this sub mesh
			vtxBaseIdx += defMesh->dMeshStatic_p[i]->shape().V.size();
		}
	}

	if (!writeToBaseModel) return;

	// perform face blendshape
	meshInstance->blendShapes();

	SrModel& baseModel = writeToBaseModel->shape();
	const Ogre::VertexElement* VertexEle_POS = meshPtr->sharedVertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
	Ogre::HardwareVertexBufferSharedPtr VertexBufPOS = meshPtr->sharedVertexData->vertexBufferBinding->getBuffer(VertexEle_POS->getSource());
	unsigned char* VertexPtrPOS = static_cast<unsigned char*>(VertexBufPOS->lock(Ogre::HardwareBuffer::HBL_NORMAL));
	int VertSizePOS = VertexBufPOS->getVertexSize();
	float * pElementPOS = NULL;
	std::map<int, std::vector<int> >& vtxNewVtxIdxMap = defMesh->vtxNewVtxIdxMap;

	for (int i = 0; i<baseModel.V.size(); i++)
	{
		int iVtx = vtxBaseIdx + i;
		SrVec basePos = baseModel.V[i];
		unsigned char* curVertexPtrPos = VertexPtrPOS + iVtx*VertSizePOS;

		VertexEle_POS->baseVertexPointerToElement(curVertexPtrPos, &pElementPOS);
		pElementPOS[0] = basePos[0];
		pElementPOS[1] = basePos[1];
		pElementPOS[2] = basePos[2];
		if (vtxNewVtxIdxMap.find(iVtx) != vtxNewVtxIdxMap.end())
		{
			std::vector<int>& idxMap = vtxNewVtxIdxMap[iVtx];
			// copy related vtx components 
			for (unsigned int k = 0; k<idxMap.size(); k++)
			{
				curVertexPtrPos = VertexPtrPOS + idxMap[k] * VertSizePOS;
				VertexEle_POS->baseVertexPointerToElement(curVertexPtrPos, &pElementPOS);
				pElementPOS[0] = basePos[0];
				pElementPOS[1] = basePos[1];
				pElementPOS[2] = basePos[2];
				//_deformPosBuf[idxMap[k]] = finalVec;
			}
		}
	}

	//UNLOCK BUFFER
	if (VertexBufPOS->isLocked())
		VertexBufPOS->unlock();
}

void OgreSmartBody::updateOgreCamera()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	
	Ogre::Camera* ogreCam = mCamera;
	//SrCamera& cam = *FltkViewer::_data->camera;
	SrCamera& cam = *scene->getActiveCamera();
	// override the ogre camera with fltk camera
	SrMat viewMat;
	cam.get_view_mat(viewMat);
	SrQuat q = SrQuat(viewMat);
	SrVec  p = cam.getEye();
	Ogre::Node* cameraNode = ogreCam->getParentNode();

	cameraNode->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z).Inverse());
	cameraNode->setPosition(Ogre::Vector3(p.x, p.y, p.z));
	//ogreCam->setOrientation()	
	ogreCam->setFarClipDistance(cam.getFarPlane());
	ogreCam->setNearClipDistance(cam.getNearPlane());
	//cam.zfar = ogreCam->getFarClipDistance();
	//cam.znear = ogreCam->getNearClipDistance();
	ogreCam->setFOVy(Ogre::Radian(cam.getFov()));
}

void OgreSmartBody::updateOgreLights()
{
	Ogre::SceneManager*   ogreSceneMgr = mSceneMgr;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	int numLightsInScene = 0;
	int numShadowLight = 0;
	float inverseScale = float(1.0 / scene->getScale());

	SmartBody::IntAttribute* shadowMapSizeAttr = dynamic_cast<SmartBody::IntAttribute*>(scene->getAttribute("shadowMapSize"));
	SmartBody::IntAttribute* shadowMapCountAttr = dynamic_cast<SmartBody::IntAttribute*>(scene->getAttribute("shadowMapCount"));

	if (shadowMapSizeAttr)
	{
		bool changeShadowMapSize = false;
		int numShadowTextures = scene->getIntAttribute("shadowMapCount");
		for (int s = 0; s < numShadowTextures; s++)
		{
			const Ogre::TexturePtr& texturePtr = ogreSceneMgr->getShadowTexture(s);
			int textureSize = (*texturePtr).getSize();
			if (textureSize != shadowMapSizeAttr->getValue()) // set the nth shadow map to correct size
			{
				//ogreSceneMgr->setShadowTextureSize(shadowMapSize->getValue());
				changeShadowMapSize = true;
			}
		}
		if (changeShadowMapSize)
			ogreSceneMgr->setShadowTextureSize(shadowMapSizeAttr->getValue());
	}

	if (shadowMapCountAttr)
	{
		if (shadowMapCountAttr->getValue() != ogreSceneMgr->getShadowTextureCount())
			ogreSceneMgr->setShadowTextureCount(shadowMapCountAttr->getValue());
	}

	const std::vector<std::string>& pawnNames = SmartBody::SBScene::getScene()->getPawnNames();
	for (std::vector<std::string>::const_iterator iter = pawnNames.begin();
		iter != pawnNames.end();
		iter++)
	{
		SmartBody::SBPawn* sbpawn = SmartBody::SBScene::getScene()->getPawn(*iter);
		const std::string& name = sbpawn->getName();
		if (name.find("light") == 0)
		{
			numLightsInScene++;
			if (firstTime)
			{
				ogreSceneMgr->destroyAllLights();
				firstTime = false;
			}
			Ogre::Light * light;
			SrVec pos = sbpawn->getPosition();
			try {
				light = ogreSceneMgr->getLight(name);
			}
			catch (Ogre::Exception&) {
				light = ogreSceneMgr->createLight(name);
			}
			SmartBody::BoolAttribute* enabledAttr = dynamic_cast<SmartBody::BoolAttribute*>(sbpawn->getAttribute("enabled"));
			if (enabledAttr && !enabledAttr->getValue())
			{
				light->setVisible(false);
			}
			else
			{
				light->setVisible(true);
			}
			SrVec up(0, 1, 0);
			SrQuat orientation = sbpawn->getOrientation();
			SrVec lightDirection = up * orientation;
			SmartBody::BoolAttribute* directionalAttr = dynamic_cast<SmartBody::BoolAttribute*>(sbpawn->getAttribute("lightIsDirectional"));
			if (directionalAttr)
			{
				if (directionalAttr->getValue())
				{
					if (light->getType() != Ogre::Light::LT_DIRECTIONAL)
						light->setType(Ogre::Light::LT_DIRECTIONAL);


					const Ogre::Vector3& direction = light->getParentSceneNode()->getOrientation().zAxis() * -1.0;
					if (direction.x != lightDirection.x ||
						direction.y != lightDirection.y ||
						direction.z != lightDirection.z)
						light->getParentSceneNode()->setDirection(lightDirection.x, lightDirection.y, lightDirection.z, Ogre::Node::TS_LOCAL);
				}
				else
				{
					light->setType(Ogre::Light::LT_POINT);
					light->getParentSceneNode()->setPosition(pos.x, pos.y, pos.z);
					light->getParentSceneNode()->setDirection(lightDirection.x, lightDirection.y, lightDirection.z, Ogre::Node::TS_LOCAL);
				}
			}
			else
			{
				light->setType(Ogre::Light::LT_POINT);
				light->getParentSceneNode()->setPosition(pos.x, pos.y, pos.z);
				light->getParentSceneNode()->setDirection(lightDirection.x, lightDirection.y, lightDirection.z, Ogre::Node::TS_LOCAL);
			}

			SmartBody::BoolAttribute* castShadowAttr = dynamic_cast<SmartBody::BoolAttribute*>(sbpawn->getAttribute("lightCastShadow"));
			bool lightCastShadow = false;
			if (castShadowAttr)
			{
				if (light->getCastShadows() != castShadowAttr->getValue())
					light->setCastShadows(castShadowAttr->getValue());
				if (castShadowAttr->getValue())
				{
					lightCastShadow = true;
					numShadowLight++;
				}
			}
#if 0 // disable this since this is a global parameter, should be set in SBScene instead
			SmartBody::IntAttribute* shadowMapSize = dynamic_cast<SmartBody::IntAttribute*>(sbpawn->getAttribute("lightShadowMapSize"));
			if (shadowMapSize)
			{
				int numShadowTextures = ogreSceneMgr->getShadowTextureCount();
				for (int s = 0; s < numShadowTextures; s++)
				{
					const Ogre::TexturePtr& texturePtr = ogreSceneMgr->getShadowTexture(s);
					int textureSize = (*texturePtr).getSize();
					if (textureSize != shadowMapSize->getValue()) // set the nth shadow map to correct size
					{
						ogreSceneMgr->setShadowTextureSize(shadowMapSize->getValue());
					}
				}
				//ogreSceneMgr->setShadowTextureSize(shadowMapSize->getValue());			
			}
#endif

			SmartBody::Vec3Attribute* diffuseColorAttr = dynamic_cast<SmartBody::Vec3Attribute*>(sbpawn->getAttribute("lightDiffuseColor"));
			if (diffuseColorAttr)
			{
				const SrVec& color = diffuseColorAttr->getValue();
				const Ogre::ColourValue& diffuse = light->getDiffuseColour();
				Ogre::ARGB rgb = diffuse.getAsARGB();
				if (diffuse.r != color[0] ||
					diffuse.g != color[1] ||
					diffuse.b != color[2])
					light->setDiffuseColour(color.x, color.y, color.z);
			}
			else
			{
				const Ogre::ColourValue& diffuse = light->getDiffuseColour();
				Ogre::ARGB rgb = diffuse.getAsARGB();
				if (diffuse.r != 1.0f ||
					diffuse.g != 0.95f ||
					diffuse.b != 0.8f)
					light->setDiffuseColour(1.0f, 0.95f, 0.8f);
			}
			SmartBody::Vec3Attribute* specularColorAttr = dynamic_cast<SmartBody::Vec3Attribute*>(sbpawn->getAttribute("lightSpecularColor"));
			if (specularColorAttr)
			{
				const SrVec& color = diffuseColorAttr->getValue();
				const Ogre::ColourValue& specular = light->getSpecularColour();
				Ogre::ARGB rgb = specular.getAsARGB();
				if (specular.r != color[0] ||
					specular.g != color[1] ||
					specular.b != color[2])
					light->setSpecularColour(color.x, color.y, color.z);
			}
			else
			{
				const SrVec& color = diffuseColorAttr->getValue();
				const Ogre::ColourValue& specular = light->getSpecularColour();
				Ogre::ARGB rgb = specular.getAsARGB();
				if (specular.r != 0.0f ||
					specular.g != 0.0f ||
					specular.b != 0.0f)
					light->setSpecularColour(0.0f, 0.0f, 0.0f);
			}
		}
	}
	if (ogreSceneMgr->getShadowFarDistance() != 30.f*inverseScale)
		ogreSceneMgr->setShadowFarDistance(30.f*inverseScale);
	if (ogreSceneMgr->getShadowDirectionalLightExtrusionDistance() != 30.f*inverseScale)
		ogreSceneMgr->setShadowDirectionalLightExtrusionDistance(30.f*inverseScale);

	if (numLightsInScene == 0)
	{
		Ogre::Light* light;
		// add in default Ogre lighting here
		try {
			light = ogreSceneMgr->getLight("defaultLight0");
		}
		catch (Ogre::Exception&) {
			light = ogreSceneMgr->createLight("defaultLight0");
			light->setType(Ogre::Light::LT_DIRECTIONAL);
		}

		SrMat mat;
		sr_euler_mat_xyz(mat, SR_TORAD(0),SR_TORAD(0), SR_TORAD(135));
		SrQuat orientation(mat);
		SrVec up(0, 1, 0);
		SrVec lightDirection = up * orientation;
		light->getParentSceneNode()->setDirection(lightDirection[0], lightDirection[1], -lightDirection[2], Ogre::Node::TS_LOCAL);
		light->setDiffuseColour(1.0f, 1.0f, 1.0f);
		light->setVisible(true);

		// add the second default light
		try {
			light = ogreSceneMgr->getLight("defaultLight1");
		}
		catch (Ogre::Exception&) {
			light = ogreSceneMgr->createLight("defaultLight1");
			light->setType(Ogre::Light::LT_DIRECTIONAL);
		}

		sr_euler_mat_xyz(mat, SR_TORAD(0), SR_TORAD(0), SR_TORAD(-135));
		SrQuat orientation2(mat);
		SrVec lightDirection2 = up * orientation2;
		light->getParentSceneNode()->setDirection(lightDirection2[0], lightDirection2[1], -lightDirection2[2]);
		light->setDiffuseColour(1.0f, 1.0f, 1.0f);
		light->setVisible(true);

	}
	else
	{
		Ogre::Light* light;
		try {
			light = ogreSceneMgr->getLight("defaultLight0");
			light->setVisible(false);
		}
		catch (Ogre::Exception&) {
		}
		try {
			light = ogreSceneMgr->getLight("defaultLight1");
			light->setVisible(false);
		}
		catch (Ogre::Exception&) {
		}
	}

}


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "shellapi.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		LPWSTR *szArgList;
		int argCount;

		szArgList = CommandLineToArgvW(GetCommandLineW(), &argCount);
		if (szArgList == NULL || argCount < 3)
		{
			MessageBox(NULL, "Usage: ogresmartbody.exe <mediapath> <script>", "Error", MB_OK);
			return 1;
		}

		std::wstring wMediaPath(szArgList[1]);
		mediaPath = std::string(wMediaPath.begin(), wMediaPath.end());

		std::wstring wFilePath = szArgList[2];
		std::string filePath = std::string(wFilePath.begin(), wFilePath.end());
		
		
		LocalFree(szArgList);
#else
		if (argc < 2)
		{ 
			std::cout << "Usage: ogresmartbody.exe <mediapath> <script>" << std::endl;
			exit(1);
		}

		mediaPath = argv[1];
		std::string filePath = argv[2];
#endif


		boost::filesystem::path p(filePath);
		originalScript = p.filename().string();
		originalPath = p.parent_path().string();		

		
		// Create application object
        OgreSmartBody app;
 
        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
 
        return 0;
    }
 
#ifdef __cplusplus
}
#endif
