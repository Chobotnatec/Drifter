
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

string helpMsg  = "Program to check marker detection.\n"
"You specify what markers do you want to track and the program will show you those markers. If markers are not shown correctly than consider running `setUpMarker` to alter marker setting.\n"
"Example of use:\n"
"./checkMarkers --markerNames orangeMarker blueMarker\n\n"
"command line arguments";

class CheckMarkerParser : public ConfigParser{
public:
  CheckMarkerParser( string helpMsg ) : ConfigParser(helpMsg){

    mainDesc.add_options()
      ("markerNames,m",po::value<vector<string> >()->multitoken(),"Names of the markers to show e.g. \"orangeMarker blueMarker\" ");
  }
};

int main(int arg, char* argc[]){

  CheckMarkerParser parser(helpMsg);

  try{
    parser.parse( arg, argc);

    vector<MarkerType> markerTypes;
    vector<vector<Point2d> > markers;
    string dataFolder = parser.get<string>("dataFolder");
    int camID = parser.get<int>("camID");
    int camW = parser.get<int>("camWidth");
    int camH = parser.get<int>("camHeight");

    int mCount = parser.count<string>("markerNames");
    if( mCount == 0 ){
      cerr << "You need to specify at least one marker! See option 'markerNames'." << endl;
      return 0;
    }else{

      for(int i=0;i<mCount;++i){

	string markerName = dataFolder + "/markers/" + parser.get<string>("markerNames",i) + ".txt";

	markerTypes.push_back( MarkerType( markerName ) );
	cout << markerName << endl;
      }
    }

    VideoCapture cap(camID);
    cap.set(CV_CAP_PROP_FRAME_WIDTH,camW);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,camH);

    if (!cap.isOpened())
      return false;

    int MN = markerTypes.size();

    Mat src;

    cap >> src;

    while (1) {
      cap >> src;

      trackMarkers(src, markerTypes, markers);

      for(int j=0;j<markers.size();j++){
	auto& marks = markers[j];
	Scalar HSVcolor = 0.5*(markerTypes[j].minHSV + markerTypes[j].maxHSV);
	HSVcolor[0] = (int)(HSVcolor[0] + 128) % 256;
	HSVcolor[1] = 255;
	HSVcolor[2] = 255;
	Scalar color = scalarHSV2BGR( HSVcolor );
      
	for (size_t i = 0; i < marks.size(); i++) {
	  //	HSVcolor[1] = 0; HSVcolor[2] = 255;
	  circle(src, marks[i], 1, color, 2);
	  stringstream ss;
	  ss << i;
	  putText(src, ss.str(), marks[i], FONT_HERSHEY_PLAIN, 1.5, color);
	}
      }

      imshow("check Markers", src);

      if (waitKey(1) >= 0)
	break;
    }
  }catch(CheckMarkerParser::Exception const & exc){
    cout << exc.what() << endl;
    return 0;
  }catch(exception const & exc){
    cerr << exc.what() << endl;
    return 0;
  }
  return 0;
}
