
#include <iostream>
#include <fstream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "../library/objectTracking.h"
#include "../library/configParser.h"
#include <iosfwd>

#include <chrono>
#include <string>
#include <string>

using namespace std;
using namespace cv;
using namespace Eigen;
using namespace tracking;
namespace po=boost::program_options;

const string helpMsg = 
  "Program for setting up markers.\n"
"Example of use:\n"
"./setUpMarker --markerName orangeMarker.\n"
"Result is saved is ${dataFolder}/markers/${markerName}.txt.\n\n"
"command line arguments";

class MarkerParser : public ConfigParser{
public:
  MarkerParser( string helpMsg ) : ConfigParser(helpMsg){

    mainDesc.add_options()
      ("markerName,m",po::value<string>(),"Name of the marker e.g. \"orangeMarker\" ");
  }
};
int main(int argc, char *argv[]){

  MarkerParser parser( helpMsg );

  try{
    if(!parser.parse( argc, argv ))
      return 0;

    const string filename = parser.get<string>("dataFolder")+"/markers/"+parser.get<string>("markerName")+".txt";
    const int camID = parser.get<int>("camID");
    const int resW = parser.get<int>("camWidth");
    const int resH = parser.get<int>("camHeight");

    int MIN_H(0), MAX_H(255), MIN_S(0), MAX_S(255), MIN_V(0), MAX_V(255),
      erodeAmount(0), dilateAmount(0);

    // Load marker data if it exists
    try{
      MarkerType mt( filename );
    
      MIN_H = mt.minHSV[0];
      MAX_H = mt.maxHSV[0];
      MIN_S = mt.minHSV[1];
      MAX_S = mt.maxHSV[1];
      MIN_V = mt.minHSV[2];
      MAX_V = mt.maxHSV[2];
    
      erodeAmount = mt.erode;
      dilateAmount = mt.dilate;
    }catch( std::ios_base::failure ){
    }

    // create trackbars
    namedWindow("trackbars", 0);
    createTrackbar("MIN_H", "trackbars", &MIN_H, 255);
    createTrackbar("MAX_H", "trackbars", &MAX_H, 255);
    createTrackbar("MIN_S", "trackbars", &MIN_S, 255);
    createTrackbar("MAX_S", "trackbars", &MAX_S, 255);
    createTrackbar("MIN_V", "trackbars", &MIN_V, 255);
    createTrackbar("MAX_V", "trackbars", &MAX_V, 255);
    createTrackbar("erodeAmount", "trackbars", &erodeAmount, 10);
    createTrackbar("dilateAmount", "trackbars", &dilateAmount, 10);


    ofstream outFile(filename);

    if(!outFile.is_open()){
      cout << "Could not open marker file: " << filename << endl;
      return 0;
    }

    Mat src, src_processed;

    VideoCapture cap(camID);
    cap.set(CV_CAP_PROP_FRAME_WIDTH,resW);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,resH);
    if (!cap.isOpened())
      return false;

    cap >> src;

    while (1) {

      cap >> src;

      tracking::preprocessImage(src, src_processed,
				cv::Scalar(MIN_H, MIN_S, MIN_V),
				cv::Scalar(MAX_H, MAX_S, MAX_V), erodeAmount, dilateAmount);
      cv::Mat src2;
      src.copyTo(src2,src_processed);
      //    multiply( src, src, src );
      imshow("preprocess test", src2);

      if (waitKey(1) >= 0){
	outFile << MIN_H << ' ' << MAX_H << endl;
	outFile << MIN_S << ' ' << MAX_S << endl;
	outFile << MIN_V << ' ' << MAX_V << endl;
	outFile << erodeAmount << ' ' << dilateAmount << endl;
	outFile << 10 << ' ' << 1000000;
	outFile.close();
	break;
      }
    }


  }catch( MarkerParser::Exception const & exc ){
    cout << exc.what() << endl;
    return 0;
  }


  return 0;
}

