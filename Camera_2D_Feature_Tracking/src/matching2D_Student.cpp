#include <numeric>
#include "matching2D.hpp"

using namespace std;

// Find best matches for keypoints in two camera images based on several matching methods
void matchDescriptors(std::vector<cv::KeyPoint> &kPtsSource, std::vector<cv::KeyPoint> &kPtsRef, cv::Mat &descSource, cv::Mat &descRef,
                      std::vector<cv::DMatch> &matches, std::string descriptorType, std::string matcherType, std::string selectorType)
{
    // configure matcher
    bool crossCheck = false;
    cv::Ptr<cv::DescriptorMatcher> matcher;

    if (matcherType.compare("MAT_BF") == 0)
    {
        int normType = cv::NORM_HAMMING;
        matcher = cv::BFMatcher::create(normType, crossCheck);
    }
    else if (matcherType.compare("MAT_FLANN") == 0)
    {   
        if (descSource.type() != CV_32F)
        {   // OpenCV bug workaround : convert binary descriptors to floating point due to a bug in current OpenCV implementation
            descSource.convertTo(descSource, CV_32F);
            descRef.convertTo(descRef, CV_32F);
        }
        matcher = cv::FlannBasedMatcher::create();
    }

    // perform matching task
    if (selectorType.compare("SEL_NN") == 0)
    { // nearest neighbor (best match)

        matcher->match(descSource, descRef, matches); // Finds the best match for each descriptor in desc1

    }
    else if (selectorType.compare("SEL_KNN") == 0)
    { // k nearest neighbors (k=2)

        // ...
        vector<vector<cv::DMatch>> knn_matches;
        matcher->knnMatch(descSource,descRef,knn_matches,2);
        const float ratio_thresh = 0.8f;
        for (size_t i =0 ; i<knn_matches.size() ; i++)
        {
            if(knn_matches[i][0].distance < knn_matches[i][1].distance * ratio_thresh)
            {
                matches.push_back(knn_matches[i][0]);
            }
        }
        cout << "# keypoints removed = " << knn_matches.size() - matches.size() << endl;
    }
}

// Use one of several types of state-of-art descriptors to uniquely identify keypoints
void descKeypoints(vector<cv::KeyPoint> &keypoints, cv::Mat &img, cv::Mat &descriptors, string descriptorType)
{
    // select appropriate descriptor
    cv::Ptr<cv::DescriptorExtractor> extractor;
    double t ;
    if (descriptorType.compare("BRISK") == 0)
    {
        int threshold = 30;        // FAST/AGAST detection threshold score.
        int octaves = 3;           // detection octaves (use 0 to do single scale)
        float patternScale = 1.0f; // apply this scale to the pattern used for sampling the neighbourhood of a keypoint.
        t = (double)cv::getTickCount();
        extractor = cv::BRISK::create(threshold, octaves, patternScale);
        extractor->compute(img ,keypoints ,descriptors );
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

    }
    else if(descriptorType.compare("SIFT") == 0 )
    {  
        t = (double)cv::getTickCount();
        extractor = cv::xfeatures2d::SIFT::create();
        extractor->compute(img ,keypoints ,descriptors );
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    }
    else if(descriptorType.compare("ORB") == 0 )
    { 
        t = (double)cv::getTickCount();
        extractor = cv::ORB::create();
        extractor->compute(img ,keypoints,descriptors);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    }
    else if(descriptorType.compare("AKAZE") == 0 )
    { 
        t = (double)cv::getTickCount();
        extractor = cv::AKAZE::create();
        extractor->compute(img ,keypoints,descriptors);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    }
    else if(descriptorType.compare("BRIEF") == 0 )
    { 
        t = (double)cv::getTickCount();
        extractor = cv::xfeatures2d::BriefDescriptorExtractor::create();
        extractor->compute(img ,keypoints,descriptors);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    }
    cout << descriptorType << " descriptor extraction in " << 1000 * t / 1.0 << " ms" << endl;

}

// Detect keypoints in image using the traditional Shi-Thomasi detector
void detKeypointsShiTomasi(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis)
{
    // compute detector parameters based on image size
    int blockSize = 4;       //  size of an average block for computing a derivative covariation matrix over each pixel neighborhood
    double maxOverlap = 0.0; // max. permissible overlap between two features in %
    double minDistance = (1.0 - maxOverlap) * blockSize;
    int maxCorners = img.rows * img.cols / max(1.0, minDistance); // max. num. of keypoints

    double qualityLevel = 0.01; // minimal accepted quality of image corners
    double k = 0.04;

    // Apply corner detection
    double t = (double)cv::getTickCount();
    vector<cv::Point2f> corners;
    cv::goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance, cv::Mat(), blockSize, false, k);

    // add corners to result vector
    for (auto it = corners.begin(); it != corners.end(); ++it)
    {

        cv::KeyPoint newKeyPoint;
        newKeyPoint.pt = cv::Point2f((*it).x, (*it).y);
        newKeyPoint.size = blockSize;
        keypoints.push_back(newKeyPoint);
    }
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "Shi-Tomasi detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;
    // visualize results
    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "Shi-Tomasi Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsModern(vector<cv::KeyPoint> &keypoints, cv::Mat &img,string detectorType, bool bVis)
{   
    cv::Ptr<cv::FeatureDetector> Detector;
    double t=0 ;
    if(detectorType.compare("BRISK") == 0)   
    {   
        t = (double)cv::getTickCount();
        Detector = cv::BRISK::create();
        Detector->detect(img ,keypoints);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    }
    else if(detectorType.compare("SIFT") == 0 )
    {  
        t = (double)cv::getTickCount();
        Detector = cv::xfeatures2d::SIFT::create();
        Detector->detect(img ,keypoints);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    }
    else if(detectorType.compare("ORB") == 0 )
    { 
        t = (double)cv::getTickCount();
        Detector = cv::ORB::create();
        Detector->detect(img ,keypoints);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    }
    else if(detectorType.compare("AKAZE") == 0 )
    { 
        t = (double)cv::getTickCount();
        Detector = cv::AKAZE::create();
        Detector->detect(img ,keypoints);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    }
    else if(detectorType.compare("HARRIS") == 0 )
    { 
        t = (double)cv::getTickCount();
        detKeypointsHarris(Detector ,keypoints,img , bVis);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    }
    else if(detectorType.compare("FAST") == 0 )
    {   
        int threshold = 30;
        bool bNMS = true;
        cv::FastFeatureDetector::DetectorType type = cv::FastFeatureDetector::TYPE_9_16;
        t = (double)cv::getTickCount();
        Detector = cv::FastFeatureDetector::create(threshold,bNMS,type);
        Detector->detect(img ,keypoints);        
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    }
    cout << detectorType <<" detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;
    if(bVis && detectorType.compare("HARRIS") !=0)
    {
    cv::Mat visimg = img.clone();
    string windowname =  detectorType;
    cv::namedWindow(windowname , 1);
    cv::drawKeypoints(img ,keypoints,visimg);
    cv::imshow(windowname,visimg);
    cv::waitKey(0);
    }

}
void detKeypointsHarris(cv::Ptr<cv::FeatureDetector> &Detector,std::vector<cv::KeyPoint> &Keys, cv::Mat &img, bool bVis)
{
    int blockSize = 2;     // for every pixel, a blockSize ?? blockSize neighborhood is considered
    int apertureSize = 3;  // aperture parameter for Sobel operator (must be odd)
    int minResponse = 100; // minimum value for a corner in the 8bit scaled response matrix
    double k = 0.04;       // Harris parameter (see equation for details)

    cv::Mat dst, dst_norm, dst_norm_scaled;
    dst = cv::Mat::zeros(img.size(), CV_32FC1);
    cv::cornerHarris(img, dst, blockSize, apertureSize, k, cv::BORDER_DEFAULT);
    cv::normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
    cv::convertScaleAbs(dst_norm, dst_norm_scaled);

    double maxOverlap = 0.0;
    for (size_t j = 0; j < dst_norm.rows; j++)
    {
        for (size_t i = 0; i < dst_norm.cols; i++)
        {
            int response = dst_norm.at<float>(j,i);
            if(response > minResponse)
            {
                cv::KeyPoint NewKeyPoint;
                NewKeyPoint.pt = cv::Point2d(i,j);
                NewKeyPoint.response = response;
                NewKeyPoint.size = 2*apertureSize;

                bool bOverlap = false;
                
                for(auto it = Keys.begin() ; it != Keys.end() ; it++)
                {
                    double KptOverlap = cv::KeyPoint::overlap(NewKeyPoint,*it);
                    if(KptOverlap > maxOverlap)
                    {
                        bOverlap = true;
                        if (NewKeyPoint.response > (*it).response)
                        {                      // if overlap is >t AND response is higher for new kpt
                            *it = NewKeyPoint; // replace old key point with new one
                            break;             // quit loop over keypoints
                        }
                    }

                }
                if (!bOverlap)
                {                                     // only add new key point if no overlap has been found in previous NMS
                    Keys.push_back(NewKeyPoint); // store new keypoint in dynamic list
                }

            }
        }
    }

    if(bVis)
    {
    std::string windowName = "Harris Corner Detection Results";
    cv::namedWindow(windowName, 5);
    cv::Mat visImage = dst_norm_scaled.clone();
    cv::drawKeypoints(dst_norm_scaled, Keys, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cv::imshow(windowName, visImage);
    cv::waitKey(0);
    }
}
