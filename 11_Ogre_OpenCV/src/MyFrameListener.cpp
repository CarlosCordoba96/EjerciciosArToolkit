/*********************************************************************
 * Módulo 4. Curso de Experto en Desarrollo de Videojuegos
 * Autor: Carlos González Morcillo     Carlos.Gonzalez@uclm.es
 *
 * You can redistribute and/or modify this file under the terms of the
 * GNU General Public License ad published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * and later version. See <http://www.gnu.org/licenses/>.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  
 *********************************************************************/

#include "MyFrameListener.h"
#define WIDTHRES 640
#define HEIGHTRES 480
#define VIDEODEVICE 1

MyFrameListener::MyFrameListener(Ogre::RenderWindow* win, 
				 Ogre::Camera* cam, 
				 Ogre::SceneNode *node, 
				 //				 Ogre::OverlayManager *om,
				 Ogre::SceneManager *sm) {
  OIS::ParamList param;
  size_t windowHandle;  std::ostringstream wHandleStr;

  _camera = cam;  _node = node; //_overlayManager = om;
  _sceneManager = sm;
  
  win->getCustomAttribute("WINDOW", &windowHandle);
  wHandleStr << windowHandle;
  param.insert(std::make_pair("WINDOW", wHandleStr.str()));
  
  _inputManager = OIS::InputManager::createInputSystem(param);
  _keyboard = static_cast<OIS::Keyboard*>
    (_inputManager->createInputObject(OIS::OISKeyboard, false));
  _mouse = static_cast<OIS::Mouse*>
    (_inputManager->createInputObject(OIS::OISMouse, false));
  _videoManager = new VideoManager(VIDEODEVICE, WIDTHRES, HEIGHTRES, _sceneManager);
  _arDetector = new ARTKDetector(WIDTHRES, HEIGHTRES, 100);   

  Ogre::Entity* ent1;

  ent1 = _sceneManager->getEntity("Sinbad");
  ent1->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);
 
  //  Animation States 
  //  "IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
  //  "SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"

  baseAnim = ent1->getAnimationState("Dance");
  //  topAnim = ent1->getAnimationState("RunTop");
 
  baseAnim->setLoop(true);     //topAnim->setLoop(true);
  baseAnim->setEnabled(true);  //topAnim->setEnabled(true);
}

MyFrameListener::~MyFrameListener() {
  _inputManager->destroyInputObject(_keyboard);
  _inputManager->destroyInputObject(_mouse);
  OIS::InputManager::destroyInputSystem(_inputManager);
  delete _arDetector;
  delete _videoManager;
}

bool MyFrameListener::frameStarted(const Ogre::FrameEvent& evt) {
  Ogre::Real r = 0;
  Ogre::Real deltaT = evt.timeSinceLastFrame;
  int fps = 1.0 / deltaT;

  Ogre::Vector3 pos; 
  Ogre::Vector3 look; 
  Ogre::Vector3 up; 

  baseAnim->addTime(deltaT);
  //  topAnim->addTime(deltaT);

  _videoManager->UpdateFrame();
  _videoManager->DrawCurrentFrame();
  if (_arDetector->detectMark(_videoManager->getCurrentFrameMat())) {
    _arDetector->getPosRot(pos, look, up);
    _camera->setPosition(pos);
    _camera->lookAt(look);
    _camera->setFixedYawAxis(true, up);
  }
  
  _keyboard->capture();
  if(_keyboard->isKeyDown(OIS::KC_ESCAPE)) return false;
  if(_keyboard->isKeyDown(OIS::KC_R)) r+=180;
  _node->yaw(Ogre::Degree(r * deltaT));
  
  _mouse->capture();

  /*  
  Ogre::OverlayElement *oe;
  oe = _overlayManager->getOverlayElement("fpsInfo");
  oe->setCaption(Ogre::StringConverter::toString(fps));
  oe = _overlayManager->getOverlayElement("camPosInfo");
  oe->setCaption(Ogre::StringConverter::toString(_camera->getPosition()));
  oe = _overlayManager->getOverlayElement("camRotInfo");
  oe->setCaption(Ogre::StringConverter::toString(_camera->getDirection()));
  oe = _overlayManager->getOverlayElement("modRotInfo");
  Ogre::Quaternion q2 = _node->getOrientation();
  oe->setCaption(Ogre::String("RotZ: ") + Ogre::StringConverter::toString(q2.getYaw()));
  */
  return true;
}
