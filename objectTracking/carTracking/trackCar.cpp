#include "../library/carTracker.h"

#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
using namespace drift;
using namespace cv;
using namespace std::chrono;

int main(){

  ofstream outFile;
  outFile.open((string)DRIFTER_DATA_DIR+"/data/trackingPositions.txt");

  if (!outFile.is_open())
    return false;

  Camera cam = loadCameraFromFile((string)DRIFTER_DATA_DIR+"/cameraData/camera.txt");

  MarkerType grndMkr((string)DRIFTER_DATA_DIR+"/markers/pinkMarker.txt");
  MarkerType frontMkr((string)DRIFTER_DATA_DIR+"/markers/blueMarker.txt");
  MarkerType backMkr((string)DRIFTER_DATA_DIR+"/markers/orangeMarker.txt");

  MatrixDD d = loadDistanceMatrix((string)DRIFTER_DATA_DIR+"/cameraData/distances.txt");

  CarTracker tracker( 1, "test1.avi", cam, grndMkr, d, frontMkr, 82, backMkr, 117 );

  tracker.initialize(10);

  Point2d fsp,bsp;
  Vector3 fp,bp;

  int n=0;

  time_point<system_clock> ts,te,timeStart;

  timeStart = system_clock::now();

  outFile << "# Format:\n# Frame number\n# FM x-coord in PS\n# FM y-coord in PS\n# BM x-coord in PS\n# BM y-coord in PS\n# FM x-coord in SS\n# FM y-coord in SS\n# BM x-coord in SS\n# BM y-coord in SS\n# start time of taking frame in ms\n# end time of taking frame in ms\n\n# FM - front marker\n# BM - back marker\n# PS - plane space\n# SS - screen space\n\n";

  while(tracker.trackCar(fp,fsp,bp,bsp,ts,te)){
  
    outFile << n++ << ";\t"<<  fp[0] << ";\t" << fp[1] << ";\t" << bp[0] << ";\t" << bp[1] << ";\t" <<
      fsp.x << ";\t" << fsp.y << ";\t" << bsp.x << ";\t" << bsp.y << ";\t" <<
      duration_cast<milliseconds>(ts-timeStart).count() << ";\t" <<
      duration_cast<milliseconds>(te-timeStart).count() << ";\t" << endl;

    if (cv::waitKey(1) == 27){
      outFile.close();
      break;
    }
  }
  outFile.close();

  return 0;
  
}
