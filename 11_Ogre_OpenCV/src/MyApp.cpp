/*********************************************************************
 * Módulo 2. Curso de Experto en Desarrollo de Videojuegos
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

#include "MyApp.h" 

MyApp::MyApp() {
  _sceneManager = NULL;
  _framelistener = NULL;
}

MyApp::~MyApp() {
  delete _root;
  delete _framelistener;
}

int MyApp::start() {
  _root = new Ogre::Root();
  
  if(!_root->restoreConfig()) {
    _root->showConfigDialog();
    _root->saveConfig();
  }
  
  Ogre::RenderWindow* window = _root->initialise(true,"MyApp Example");
  _sceneManager = _root->createSceneManager(Ogre::ST_GENERIC);
  _sceneManager->setAmbientLight(Ogre::ColourValue(1, 1, 1));
  
  Ogre::Camera* cam = _sceneManager->createCamera("MainCamera");
  cam->setPosition(Ogre::Vector3(50,250,250));
  cam->lookAt(Ogre::Vector3(0,0,0));
  cam->setNearClipDistance(1);
  cam->setFarClipDistance(1000);
  
  Ogre::Viewport* viewport = window->addViewport(cam);
  viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,0.0));


  double width = viewport->getActualWidth();
  double height = viewport->getActualHeight();
  cam->setAspectRatio(width / height);
  
  loadResources();
  createScene();

  Ogre::SceneNode *node = _sceneManager->getSceneNode("SinbadNode");
  
  _framelistener = new MyFrameListener(window, cam, node, _sceneManager);
  _root->addFrameListener(_framelistener);
  
  _root->startRendering();
  return 0;
}

void MyApp::loadResources() {
  Ogre::ConfigFile cf;
  cf.load("resources.cfg");
  
  Ogre::ConfigFile::SectionIterator sI = cf.getSectionIterator();
  Ogre::String sectionstr, typestr, datastr;
  while (sI.hasMoreElements()) {
    sectionstr = sI.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = sI.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    for (i = settings->begin(); i != settings->end(); ++i) {
      typestr = i->first;    datastr = i->second;
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation
            (datastr, typestr, sectionstr);	
    }
  }
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void MyApp::createScene() {
  Ogre::Entity* ent1 = _sceneManager->createEntity("Sinbad","Sinbad.mesh");
  Ogre::SceneNode* node1 = _sceneManager->createSceneNode("SinbadNode");

  node1->attachObject(ent1);
  node1->setScale(20,20,20);
  node1->translate(Ogre::Vector3(0,100,0), Ogre::Node::TS_LOCAL);
  _sceneManager->getRootSceneNode()->addChild(node1);

  _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);	
  Ogre::Light* light = _sceneManager->createLight("Light1");
  light->setType(Ogre::Light::LT_DIRECTIONAL);
  light->setDirection(Ogre::Vector3(1,-1,0));
  node1->attachObject(light);
}


