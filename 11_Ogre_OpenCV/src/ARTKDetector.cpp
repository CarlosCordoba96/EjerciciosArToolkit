#include "ARTKDetector.h"

ARTKDetector::ARTKDetector(int w, int h, int thres){
  _markerNum=0;
  _markerInfo=NULL;
  _thres = thres;
  _width = w; _height = h;
  _detected = false;

  if (readConfiguration()==0) 
    std::cout << "Config ARToolKit [OK]" << std::endl;
  else std::cout << "Config ARToolKit [ERROR]" << std::endl;

  if (readPatterns()==0) 
    std::cout << "Read ARPatterns [OK]" << std::endl;
  else std::cout << "Read ARPatterns [ERROR]" << std::endl;
}

ARTKDetector::~ARTKDetector(){
  argCleanup();
}

int ARTKDetector::readPatterns(){
  if((_pattId=arLoadPatt("data/simple.patt")) < 0) return -1;
  return 0;
}


int ARTKDetector::readConfiguration(){
  ARParam  wparam, cparam; 
  // Cargamos los parametros intrinsecos de la camara
  if(arParamLoad("data/camera_para.dat", 1, &wparam) < 0) return -1;
  
  arParamChangeSize(&wparam, _width, _height, &cparam);
  arInitCparam(&cparam);   // Inicializamos la camara con "cparam"
  return 0;
}

bool ARTKDetector::detectMark(cv::Mat* frame) {
  int j, k;
  double p_width     = 120.0;        // Ancho del patron (marca)
  double p_center[2] = {0.0, 0.0};   // Centro del patron (marca)

  _detected = false;
  if(frame->rows==0) return _detected;

  // Detectamos la marca en el frame capturado 
  if(arDetectMarker(frame->data, _thres, &_markerInfo, &_markerNum) < 0) {
    std::cout << "ERROR ARDETECT" << std::endl;
    return _detected; 
  }
  // Vemos donde detecta el patron con mayor fiabilidad
  for(j=0, k=-1; j < _markerNum; j++) {
    if(_pattId == _markerInfo[j].id) {
      if (k == -1) k = j;
      else if(_markerInfo[k].cf < _markerInfo[j].cf) k = j;
    }
  }

  if(k != -1) {   // Si ha detectado el patron en algun sitio...
    // Obtener transformacion relativa entre la marca y la camara real
    arGetTransMat(&_markerInfo[k], p_center, p_width, _pattTrans);
    _detected = true;
  }

  return _detected;
}

void ARTKDetector::Gl2Mat(double *gl, Ogre::Matrix4 &mat){
  for(int i=0;i<4;i++) for(int j=0;j<4;j++) mat[i][j]=gl[i*4+j];
}

void ARTKDetector::getPosRot(Ogre::Vector3 &pos, Ogre::Vector3 &look, Ogre::Vector3 &up){
  if (!_detected) return;

  double glAuxd[16]; Ogre::Matrix4 m;
  argConvGlpara(_pattTrans,glAuxd);
  Gl2Mat(glAuxd, m);   // Convertir a Matrix4 de Ogre
  
  m[0][1]*=-1; m[1][1]*=-1;  m[2][1]*=-1; m[3][1]*=-1;
  m = m.inverse();
  m = m.concatenate(Ogre::Matrix4(Ogre::Quaternion(Ogre::Degree(90),Ogre::Vector3::UNIT_X)));
 
  pos  = Ogre::Vector3 (m[3][0],         m[3][1],         m[3][2]);
  look = Ogre::Vector3 (m[2][0]+m[3][0], m[2][1]+m[3][1], m[2][2]+m[3][2]);
  up   = Ogre::Vector3 (m[1][0],         m[1][1],         m[1][2]);
}

