#include "../library/carTracker.h"
#include "../library/objectTracking.h"
#include "../library/configParser.h"

#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
using namespace tracking;
using namespace cv;
using namespace std::chrono;
namespace po=boost::program_options;

string helpMsg  = "CHANGE HELP MESSAGE\n"
  "You specify what markers do you want to track and the program will show you those markers. If markers are not shown correctly than consider running `setUpMarker` to alter marker setting.\n"
  "Example of use:\n"
  "./checkMarkers --markerNames orangeMarker blueMarker\n\n"
  "command line arguments";

class TrackPointParser : public ConfigParser{
public:
  TrackPointParser( string helpMsg ) : ConfigParser(helpMsg){

    mainDesc.add_options()
      ("groundMarker,g",po::value<string>()->default_value("groundMarker"),"Name of the ground marker e.g. \"groundMarker\" ")
      ("trackMarker,t",po::value<string>()->default_value("orangeMarker"), "Name of the marker to track e.g. \"orangeMarker\"")
      ("cameraName",po::value<string>()->default_value("default_camera"),"Name of the camera")
      ("outputName,o", po::value<string>()->default_value("trackingMarkerData"),"Name of the output file. It will be saved in tracking data directory")
      ;
  }
};


int main(int argc, char *argv[]){
  
  TrackPointParser parser(helpMsg);

  try{
    parser.parse(argc,argv);

    vector<Point2d> markers;
    string dataFolder = parser.get<string>("dataFolder");
    int camID = parser.get<int>("camID");
    int camW = parser.get<int>("camWidth");
    int camH = parser.get<int>("camHeight");

    string groundMarkerName = dataFolder + "/markers/" + parser.get<string>("groundMarker") + ".txt";
    string trackMarkerName = dataFolder + "/markers/" + parser.get<string>("trackMarker") + ".txt";
    MarkerType groundMarker(groundMarkerName);
    MarkerType trackMarker(trackMarkerName);
  
    string distanceFile = dataFolder + "/distances/" + parser.get<string>("distanceMatrix") + ".txt";
    MatrixDD distances = loadDistanceMatrix(distanceFile);
  
    Mat src;

    string cameraFileName = dataFolder + "/camera/" + parser.get<string>("cameraName") + ".txt";
    Camera cam = loadCameraFromFile( cameraFileName );

    string outFileName = dataFolder + "/data/" + parser.get<string>("outputName") + ".txt";
    ofstream outFile;
    outFile.open(outFileName);

    if (!outFile.is_open()){
      cout << "Could not open: " << outFileName << endl;
      return false;
    }

    CarTracker tracker( camID, "", cam, groundMarkerName, distances, trackMarker, 18.5, trackMarker, 18.5 );

    tracker.initialize(10);

    Point2d fsp;
    Vector3 fp;

    int n=0;

    time_point<system_clock> ts,te,timeStart;

    timeStart = system_clock::now();

    //    outFile << "# Format:\n# Frame number\n# FM x-coord in PS\n# FM y-coord in PS\n# BM x-coord in PS\n# BM y-coord in PS\n# FM x-coord in SS\n# FM y-coord in SS\n# BM x-coord in SS\n# BM y-coord in SS\n# start time of taking frame in ms\n# end time of taking frame in ms\n\n# FM - front marker\n# BM - back marker\n# PS - plane space\n# SS - screen space\n\n";

    while(1){

      if(tracker.trackFrontMarker(fp, fsp, ts,te)){
  
      outFile << n++ << ";\t"<<  fp[0] << ";\t" << fp[1] << ";\t" <<
	fsp.x << ";\t" << fsp.y << ";\t" <<
	duration_cast<milliseconds>(ts-timeStart).count() << ";\t" <<
	duration_cast<milliseconds>(te-timeStart).count() << ";\t" << endl;
      }
      
      cout << fp[0] << ' ' << fp[1] << endl;

      if (cv::waitKey(1) > 0){
	outFile.close();
	break;
      }
    }
    outFile.close();

  }catch( std::exception const & exc ){
    cerr << exc.what() << endl;
    return 0;
  }


  return 0;
  
}
