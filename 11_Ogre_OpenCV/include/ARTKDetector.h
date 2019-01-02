#include <AR/ar.h>
#include <AR/gsub.h>
#include <AR/param.h>
#include <Ogre.h>
#include <iostream>

#include "cv.h"

class ARTKDetector {
 private:
  ARMarkerInfo *_markerInfo;
  int _markerNum;
  int _thres;
  int _pattId;
  int _width; int _height;
  double _pattTrans[3][4];   
  bool _detected;

  int readConfiguration();
  int readPatterns();
  void Gl2Mat(double *gl, Ogre::Matrix4 &mat);

 public:
  ARTKDetector(int w, int h, int thres);
  ~ARTKDetector();
  bool detectMark(cv::Mat* frame);
  void getPosRot(Ogre::Vector3 &pos, Ogre::Vector3 &look, Ogre::Vector3 &up);
};
