/* \author Aaron Brown */
// Create simple 3d highway enviroment using PCL
// for exploring self-driving car sensors

#include "sensors/lidar.h"
#include "render/render.h"
#include "processPointClouds.h"
// using templates for processPointClouds so also include .cpp to help linker
#include "processPointClouds.cpp"

std::vector<Car> initHighway(bool renderScene, pcl::visualization::PCLVisualizer::Ptr& viewer)
{

    Car egoCar( Vect3(0,0,0), Vect3(4,2,2), Color(0,1,0), "egoCar");
    Car car1( Vect3(15,0,0), Vect3(4,2,2), Color(0,0,1), "car1");
    Car car2( Vect3(8,-4,0), Vect3(4,2,2), Color(0,0,1), "car2");	
    Car car3( Vect3(-12,4,0), Vect3(4,2,2), Color(0,0,1), "car3");
  
    std::vector<Car> cars;
    cars.push_back(egoCar);
    cars.push_back(car1);
    cars.push_back(car2);
    cars.push_back(car3);

    if(renderScene)
    {
        renderHighway(viewer);
        egoCar.render(viewer);
        car1.render(viewer);
        car2.render(viewer);
        car3.render(viewer);
    }

    return cars;
}


void simpleHighway(pcl::visualization::PCLVisualizer::Ptr& viewer)
{
    // ----------------------------------------------------
    // -----Open 3D viewer and display simple highway -----
    // ----------------------------------------------------
    
    // RENDER OPTIONS
    bool renderScene = false;
    std::vector<Car> cars = initHighway(renderScene, viewer);

    // TODO:: Create lidar sensor 
    Lidar* lidar = new  Lidar(cars , 0);
    // TODO:: Create point processor
    pcl::PointCloud<pcl::PointXYZ>::Ptr scanned_points =lidar->scan();
    //#Creating Rays
    //renderRays(viewer , lidar->position, scanned_points);
    //#Creating Point Cloud
    //renderPointCloud(viewer , scanned_points, "Cloud Representation");
    //#Creating Process Point
    ProcessPointClouds<pcl::PointXYZ> processor_points ;
    std::pair<pcl::PointCloud<pcl::PointXYZ>::Ptr, pcl::PointCloud<pcl::PointXYZ>::Ptr> segmentedcloud = processor_points.SegmentPlane(scanned_points,100,0.2);
    //renderPointCloud(viewer , segmentedcloud.first , "obst Representation" ,Color(1,0,0));
    //renderPointCloud(viewer , segmentedcloud.second, "plane Representation",Color(0,1,0) );

    std::vector<pcl::PointCloud<pcl::PointXYZ>::Ptr> CloudClusters = processor_points.Clustering(segmentedcloud.first, 1.0, 3, 30); //i'll change to heeaaaaaaaaaap
    int clusterId = 0;
    std::vector<Color> colors={Color(1,0,0) , Color(0,1,0) , Color(0,0,1)};
    for(pcl::PointCloud<pcl::PointXYZ>::Ptr cluster : CloudClusters)
        {
            std::cout<<"Cluster Size ";
            processor_points.numPoints(cluster);
            renderPointCloud(viewer , cluster , "#ObstCloud"+std::to_string(clusterId) , colors[clusterId]);
            Box box = processor_points.BoundingBox(cluster);
            renderBox(viewer , box , clusterId);
            clusterId++;
        }

}


//setAngle: SWITCH CAMERA ANGLE {XY, TopDown, Side, FPS}
void initCamera(CameraAngle setAngle, pcl::visualization::PCLVisualizer::Ptr& viewer)
{

    viewer->setBackgroundColor (0, 0, 0);
    
    // set camera position and angle
    viewer->initCameraParameters();
    // distance away in meters
    int distance = 16;
    
    switch(setAngle)
    {
        case XY : viewer->setCameraPosition(-distance, -distance, distance, 1, 1, 0); break;
        case TopDown : viewer->setCameraPosition(0, 0, distance, 1, 0, 1); break;
        case Side : viewer->setCameraPosition(0, -distance, 0, 0, 0, 1); break;
        case FPS : viewer->setCameraPosition(-10, 0, 0, 0, 0, 1);
    }

    if(setAngle!=FPS)
        viewer->addCoordinateSystem (1.0);
}

void cityBlock(pcl::visualization::PCLVisualizer::Ptr &viewer){

    ProcessPointClouds<pcl::PointXYZI>* pointProcessorI (new ProcessPointClouds<pcl::PointXYZI>());
    pcl::PointCloud<pcl::PointXYZI>::Ptr input_cloud = pointProcessorI->loadPcd("../src/sensors/data/pcd/data_1/0000000000.pcd");
    //renderPointCloud(viewer , input_cloud , "InputCLoud");
    pcl::PointCloud<pcl::PointXYZI>::Ptr FilterCloud = pointProcessorI->FilterCloud(input_cloud , 0.5f , Eigen::Vector4f  (-10,-5,-2,1) , Eigen::Vector4f (30,8,1,1));
    //renderPointCloud(viewer , FilterCloud , "FilterCloud");
    std::pair<pcl::PointCloud<pcl::PointXYZI>::Ptr, pcl::PointCloud<pcl::PointXYZI>::Ptr> Segmented = pointProcessorI->SegmentPlane(FilterCloud , 25 , 0.3);
    renderPointCloud(viewer , Segmented.first , "obstacles Representation" ,Color(1,0,0));
    renderPointCloud(viewer , Segmented.second, "plane Representation",Color(0,1,0) );

    /////////////Clustering///////////////
    std::vector<pcl::PointCloud<pcl::PointXYZI>::Ptr> CloudClusters = pointProcessorI->Clustering(Segmented.first , .53, 10, 500); //i'll change to heeaaaaaaaaaap
    int clusterId = 0;
    std::vector<Color> colors={Color(1,0,0) , Color(0,1,0) , Color(0,0,1) };
    for(pcl::PointCloud<pcl::PointXYZI>::Ptr cluster : CloudClusters)
        {
            std::cout<<"Cluster Size ";
            pointProcessorI->numPoints(cluster);
            renderPointCloud(viewer , cluster , "#ObstCloud"+std::to_string(clusterId) , colors[clusterId]);
            Box box = pointProcessorI->BoundingBox(cluster);
            renderBox(viewer , box , clusterId);
            clusterId++;
        }

}
/*
void cityBlock(pcl::visualization::PCLVisualizer::Ptr& viewer, ProcessPointClouds<pcl::PointXYZI>* pointProcessorI, const pcl::PointCloud<pcl::PointXYZI>::Ptr& inputCloud){
   
    pcl::PointCloud<pcl::PointXYZI>::Ptr FilterCloud = pointProcessorI->FilterCloud(inputCloud , 0.5f , Eigen::Vector4f  (-10,-5,-2,1) , Eigen::Vector4f (30,8,1,1));
    std::pair<pcl::PointCloud<pcl::PointXYZI>::Ptr, pcl::PointCloud<pcl::PointXYZI>::Ptr> Segmented = pointProcessorI->SegmentPlane(FilterCloud , 25 , 0.3);
    //renderPointCloud(viewer , Segmented.first , "obstacles Representation" ,Color(1,0,0));
    renderPointCloud(viewer , Segmented.second, "plane Representation",Color(0,1,0) );

    /////////////Clustering///////////////
    std::vector<pcl::PointCloud<pcl::PointXYZI>::Ptr> CloudClusters = pointProcessorI->Clustering(Segmented.first , .53, 10, 500); //i'll change to heeaaaaaaaaaap
    int clusterId = 0;
    std::vector<Color> colors={Color(1,0,0) , Color(0,1,0) , Color(0,0,1) };
    for(pcl::PointCloud<pcl::PointXYZI>::Ptr cluster : CloudClusters)
        {
            std::cout<<"Cluster Size ";
            pointProcessorI->numPoints(cluster);
            renderPointCloud(viewer , cluster , "#ObstCloud"+std::to_string(clusterId) , colors[clusterId]);
            Box box = pointProcessorI->BoundingBox(cluster);
            renderBox(viewer , box , clusterId);
            clusterId++;
        }
}
*/
void cityBlock(pcl::visualization::PCLVisualizer::Ptr& viewer, ProcessPointClouds<pcl::PointXYZI>* pointProcessorI, const pcl::PointCloud<pcl::PointXYZI>::Ptr& inputCloud){
    pcl::PointCloud<pcl::PointXYZI>::Ptr FilterCloud = pointProcessorI->FilterCloud(inputCloud , 0.5f , Eigen::Vector4f  (-10,-5,-2,1) , Eigen::Vector4f (30,8,1,1));
    std::pair<pcl::PointCloud<pcl::PointXYZI>::Ptr, pcl::PointCloud<pcl::PointXYZI>::Ptr> segmentCloud = pointProcessorI->RANSAC3D(FilterCloud, 100, 0.2);
  KdTree* tree = new KdTree;
  
  for (int i=0; i<segmentCloud.first->points.size(); i++) 
    tree->insert(segmentCloud.first->points[i],i);
  std::vector<pcl::PointCloud<pcl::PointXYZI>::Ptr> cloudClusters = pointProcessorI->euclideanCluster(segmentCloud.first, tree, 0.5, 30, 250);

  renderPointCloud(viewer,segmentCloud.second, "planefield", Color(1,1,1));
  renderPointCloud(viewer,segmentCloud.first, "obsfield", Color(1,1,0));
  int clusterId = 0;
  std::vector<Color> colors = {Color(1,0,0), Color(0,1,0), Color(0,0,1)};



  for(pcl::PointCloud<pcl::PointXYZI>::Ptr cluster : cloudClusters)
  {
    std::cout << "cluster size ";
    pointProcessorI->numPoints(cluster);
    renderPointCloud(viewer,cluster,"obstCloud"+std::to_string(clusterId),colors[clusterId]);
    
    Box box = pointProcessorI->BoundingBox(cluster);
    renderBox(viewer,box,clusterId , Color(0,1,1));
    ++clusterId;
  }
}

int main (int argc, char** argv)
{
    std::cout << "starting enviroment" << std::endl;

    pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
    CameraAngle setAngle = XY;
    initCamera(setAngle, viewer);
    //simpleHighway(viewer);
    //cityBlock(viewer);

    ProcessPointClouds<pcl::PointXYZI>* pointProcessorI = new ProcessPointClouds<pcl::PointXYZI>();
    std::vector<boost::filesystem::path> stream = pointProcessorI->streamPcd("../src/sensors/data/pcd/data_1");
    auto streamIterator = stream.begin();
    pcl::PointCloud<pcl::PointXYZI>::Ptr inputCloudI;
    


    /*while (!viewer->wasStopped ())
    {
        viewer->spinOnce ();

    } */

    while (!viewer->wasStopped ())
    {

    // Clear viewer
    viewer->removeAllPointClouds();
    viewer->removeAllShapes();

    // Load pcd and run obstacle detection process
    inputCloudI = pointProcessorI->loadPcd((*streamIterator).string());
    cityBlock(viewer, pointProcessorI, inputCloudI);

    streamIterator++;
    if(streamIterator == stream.end())
        streamIterator = stream.begin();

    viewer->spinOnce ();
    }

}