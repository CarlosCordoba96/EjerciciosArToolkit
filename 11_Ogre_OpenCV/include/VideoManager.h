#include <Ogre.h>
#include <iostream>
#include "cv.h"
#include "highgui.h"

class VideoManager {
 private:
  void createBackground(int cols, int rows);
  void ReleaseCapture();
  CvCapture* _capture;
  IplImage* _frameIpl;
  cv::Mat* _frameMat;
  Ogre::SceneManager* _sceneManager;

 public:
  VideoManager(int device, int w, int h, Ogre::SceneManager* sm);
  ~VideoManager();
  void UpdateFrame();
  IplImage* getCurrentFrameIpl();
  cv::Mat* getCurrentFrameMat();
  void DrawCurrentFrame();
};
