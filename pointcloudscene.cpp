//#include <pcl/conversions.h>

//#include <pcl/ModelCoefficients.h>
//#include <pcl/sample_consensus/method_types.h>
//#include <pcl/sample_consensus/model_types.h>
//#include <pcl/segmentation/sac_segmentation.h>
//#include <pcl/filters/voxel_grid.h>
//#include <pcl/filters/extract_indices.h>

//#include <pcl/surface/vtk_smoothing/vtk_mesh_quadric_decimation.h>

#include "pointcloudscene.h"

PointCloudScene::PointCloudScene(){

    cameras_.clear();
    nCameras_ = 0;

}

PointCloudScene::~PointCloudScene(){

}


void PointCloudScene::bundlerReader(std::string _fileName){

    std::cerr << "Reading Bundler file" << std::endl;

    std::ifstream inputFile(_fileName);
    std::string line;

    int nPoints = 0;
    PointCloud<PointXYZRGBNormalCam>::Ptr cloud (new PointCloud<PointXYZRGBNormalCam>);

    if (inputFile.is_open()){

        // We avoid all possible comments in the firsts lines
        do {
            std::getline(inputFile, line);
        } while (line.at(0) == '#');

        // First, number of cameras and number of points in the input point cloud
        boost::tokenizer<> tokens(line);
        boost::tokenizer<>::iterator tit = tokens.begin();
        std::stringstream ss;
        ss << *tit;
        ss >> nCameras_;
        ++tit;

        // stringstream is cleared. It could also be ss.str("")
        ss.str(std::string());
        ss.clear();

        ss << *tit;
        ss >> nPoints;

        ss.str(std::string());
        ss.clear();

        // Now we read the camera information
        for (unsigned int i = 0; i < nCameras_; i++){
            Camera camera;
            camera.readCamera(inputFile);

            cameras_.push_back(camera);
        }

        // Now we read geometry information
        for (unsigned int i = 0; i < nPoints; i++){

            PointXYZRGBNormalCam point;
            bundlerPointReader(point, inputFile);

            cloud->push_back(point);

        }

        pointCloud_ = cloud; //---------------------------------------------------------------------------

        inputFile.close();

    } else {
        std::cerr << "Unable to open Bundle file" << std::endl;
    }

}

void PointCloudScene::bundlerPointReader(PointXYZRGBNormalCam& _point, std::ifstream& _stream){

    std::stringstream ss;
    std::string line;

    for (unsigned int point_line = 0; point_line < 3; point_line++){

        std::getline(_stream, line);
        while (line.empty()) std::getline(_stream, line);

        boost::tokenizer<boost::char_separator<char> > point_tokens(line, boost::char_separator<char>(" "));
        boost::tokenizer<boost::char_separator<char> >::iterator ptit = point_tokens.begin();

        // This line has the position of the point
        if (point_line == 0){
            unsigned int index = 0;
            float xyz[3];
            for (; ptit  != point_tokens.end(); ++ptit, index++){
                float value;
                ss << *ptit;
                ss >> value;
                ss.str(std::string());
                ss.clear();
                xyz[index] = value;
            }
            _point.x = xyz[0];
            _point.y = xyz[1];
            _point.z = xyz[2];
        }
        // This line has the color of the point
        else if (point_line == 1){
            unsigned int index = 0;
            unsigned char rgb[3];
            for (; ptit != point_tokens.end(); ++ptit, index++){
                float value;
                ss << *ptit;
                ss >> value;
                ss.str(std::string());
                ss.clear();
                rgb[index] = value;
            }
            _point.r = rgb[0];
            _point.g = rgb[1];
            _point.b = rgb[2];
        }

        // This line sets the camera correspondances with each point in the cloud
        else {
            unsigned int nCam;
            ss << *ptit;
            ss >> nCam;
            ss.str(std::string());
            ss.clear();
            ++ptit;

            if (nCam > 0){
                int cam_value;
                ss << *ptit;
                ss >> cam_value;
                ss.str(std::string());
                ss.clear();
                _point.camera = cam_value;
            } else {
                _point.camera = -1;
            }
        }
    }
}

void PointCloudScene::writeMesh(std::string _fileName){

    io::savePLYFile(_fileName, *pointCloud_);

}
