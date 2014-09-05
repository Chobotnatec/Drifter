#include <iostream>
#include <fstream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "../library/objectTracking.h"
#include "../library/configParser.h"
#include <iosfwd>

#include <chrono>
#include <string>

using namespace std;
using namespace cv;
using namespace Eigen;
using namespace tracking;
namespace po=boost::program_options;

Scalar scalarHSV2BGR( Scalar  colorInHSV ){
  cv::Mat M(1,1,CV_8UC3, colorInHSV);
  cvtColor( M, M, CV_HSV2BGR );
  return Scalar(M.at<Vec3b>(0,0));
}

string helpMsg  = "Show Ground Markers\n"
  "This program displays ground markers with their IDs which are used during tracking. We assume that markers form something like a circle and IDs are assigned in clocwise manner.\n\n"
  "Example of use:\n"
  "./showGroundMarkers --markerName orangeMarker\n\n"
  "command line arguments";
  
class ShowGroundMarkerParser : public ConfigParser{
public:
  ShowGroundMarkerParser( string helpMsg ) : ConfigParser(helpMsg){

    mainDesc.add_options()
      ("markerName,m",po::value<string>()->default_value("groundMarker"),"Names of the ground markers e.g. \"orangeMarker\" ");
  }
};

int main(int arg, char* argc[]) {

  ShowGroundMarkerParser parser(helpMsg);

  try{
    parser.parse( arg, argc);

    MarkerType markerType;
    vector<Point2d> markers;
    string dataFolder = parser.get<string>("dataFolder");
    int camID = parser.get<int>("camID");
    int camW = parser.get<int>("camWidth");
    int camH = parser.get<int>("camHeight");

    string markerName = dataFolder + "/markers/" + parser.get<string>("markerName") + ".txt";
    markerType =  MarkerType(markerName);

    Mat src;

    VideoCapture cap(camID);
    cap.set(CV_CAP_PROP_FRAME_WIDTH,camW);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,camH);

    if (!cap.isOpened())
      return false;

    Matrix33 planeR = Matrix33::Identity();
    Vector3 planeT;

    while (1) {
      cap >> src;

      //flip(src,src,1);

      // Track orange markers
      trackMarker(src, markerType, markers);

      // sort markers in circular patern
      reorganizePoints(markers);

      // print markers in picture
      for (size_t i = 0; i < markers.size(); i++) {
	circle(src, markers[i], 1, Scalar(255, 0, 0), 2);
	stringstream ss;
	ss << i;
	putText(src, ss.str(), markers[i], FONT_HERSHEY_PLAIN, 1.5,
		Scalar(255, 0, 0));
      }


      imshow("preprocess test", src);

      if (waitKey(1) >= 0)
	break;
    }
  }catch(ShowGroundMarkerParser::Exception const & exc){
    cout << exc.what() << endl;
    return 0;
  }catch(exception const & exc){
    cerr << exc.what() << endl;
    return 0;
  }

  return 0;
}


