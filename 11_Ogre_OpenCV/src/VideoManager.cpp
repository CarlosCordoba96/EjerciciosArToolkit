#include "VideoManager.h"

// -----------------------------------------------------------------------------
//  Constructor: VideoManager
// -----------------------------------------------------------------------------
VideoManager::VideoManager(int device, int w, int h, Ogre::SceneManager* sm){
  _sceneManager = sm;
  _capture = cvCreateCameraCapture(device);
  cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_WIDTH, w);
  cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_HEIGHT, h);
  createBackground (w, h);
  _frameIpl = NULL; _frameMat = NULL;
}

// -----------------------------------------------------------------------------
//  Destructor: VideoManager
// -----------------------------------------------------------------------------
VideoManager::~VideoManager(){
  cvReleaseCapture(&_capture);
  if (_frameIpl != NULL) delete _frameIpl; 
  if (_frameMat != NULL) delete _frameMat;
}

// -----------------------------------------------------------------------------
//  createBackground: Crea el plano sobre el que se dibuja el video 
// -----------------------------------------------------------------------------
void VideoManager::createBackground(int cols, int rows){
  Ogre::TexturePtr texture=Ogre::TextureManager::getSingleton().createManual(
            "BackgroundTex", // Nombre de la textura
	    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::TEX_TYPE_2D, // Tipo de la textura
	    cols, rows, 0, // Filas, columas y Numero de Mipmaps
	    Ogre::PF_BYTE_BGRA,
	    Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
            );

  Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("Background", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME); 
  material->getTechnique(0)->getPass(0)->createTextureUnitState(); 
  material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
  material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false); 
  material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false); 
  material->getTechnique(0)->getPass(0)->setLightingEnabled(false);   
  material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName("BackgroundTex");

  // Creamos un rectangulo que cubra toda la pantalla
  Ogre::Rectangle2D* rect = new Ogre::Rectangle2D(true);    
  rect->setCorners(-1.0, 1.0, 1.0, -1.0); 
  rect->setMaterial("Background"); 
  
  // Dibujamos el background antes que nada
  rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);   
  
  // Establecemos la bounding box 
  rect->setBoundingBox(Ogre::AxisAlignedBox(-100000.0*Ogre::Vector3::UNIT_SCALE, 100000.0*Ogre::Vector3::UNIT_SCALE));   
  
  Ogre::SceneNode* node = _sceneManager->getRootSceneNode()->createChildSceneNode("BackgroundNode"); 
  node->attachObject(rect);   

}

// -----------------------------------------------------------------------------
//  UpdateFrame
// -----------------------------------------------------------------------------
void VideoManager::UpdateFrame(){
  _frameIpl = cvQueryFrame(_capture);
  _frameMat = new cv::Mat(_frameIpl);
}

// -----------------------------------------------------------------------------
//  IplImage* getCurrentFrameIpl
// -----------------------------------------------------------------------------
IplImage* VideoManager::getCurrentFrameIpl(){
  return _frameIpl;
}

// -----------------------------------------------------------------------------
//  cv::Mat* getCurrentFrameMat
// -----------------------------------------------------------------------------
cv::Mat* VideoManager::getCurrentFrameMat(){
  return _frameMat;
}

// -----------------------------------------------------------------------------
//  DrawCurrentFrame
// -----------------------------------------------------------------------------
void VideoManager::DrawCurrentFrame(){
  if(_frameMat->rows==0) return;
  Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().getByName("BackgroundTex",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  Ogre::HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
    
  pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
  const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
  
  Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
  
  for(int j=0;j<_frameMat->rows;j++) {
    for(int i=0;i<_frameMat->cols;i++) {
      int idx = ((j) * pixelBox.rowPitch + i )*4; 
      pDest[idx] = _frameMat->data[(j*_frameMat->cols + i)*3];
      pDest[idx + 1] = _frameMat->data[(j*_frameMat->cols + i)*3 + 1];
      pDest[idx + 2] = _frameMat->data[(j*_frameMat->cols + i)*3 + 2];
      pDest[idx + 3] = 255;
    }
  }
  
  pixelBuffer->unlock();
  
  Ogre::Rectangle2D* rect = static_cast<Ogre::Rectangle2D*>(_sceneManager->getSceneNode("BackgroundNode")->getAttachedObject(0));
  Ogre::MaterialPtr material = rect->getMaterial();
}

