#include <Ogre.h>
#include <OIS/OIS.h>
#include "ARTKDetector.h"
#include "VideoManager.h"

class MyFrameListener : public Ogre::FrameListener {
private:
  OIS::InputManager* _inputManager;
  OIS::Keyboard* _keyboard;
  OIS::Mouse* _mouse;
  Ogre::Camera* _camera;
  Ogre::SceneNode* _node;
  //  Ogre::OverlayManager* _overlayManager;
  Ogre::SceneManager* _sceneManager;
  Ogre::AnimationState *baseAnim, *topAnim;

  ARTKDetector* _arDetector;
  VideoManager* _videoManager;

public:

  MyFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam, 
		  Ogre::SceneNode* node, // Ogre::OverlayManager* om, 
		  Ogre::SceneManager* sm);
  ~MyFrameListener();
  bool frameStarted(const Ogre::FrameEvent& evt);  
};
