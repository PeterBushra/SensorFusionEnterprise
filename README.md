# Sensor Fusion Enterprise

* Lidar
* Radar
* Cameras
* Klaman Filters

# Lidar
Process raw lidar data with filtering, segmentation, and clustering to detect other vehicles on the road. implemented in C++.

![Alt Text](https://media.giphy.com/media/FYRguAD2LMbEzLYFKe/giphy.gif)

# Radar
Analyze radar signatures to detect and track objects. Calculate velocity and orientation by correcting for radial velocity distortions, noise, and occlusions.

# Cameras
Fuse camera images together with lidar point cloud data. will extract object features, classify objects, and project the camera image into three dimensions to fuse with lidar data.

# Kalman Filters
Fuse data from multiple sources using Kalman filters, and build extended and unscented Kalman filters for tracking nonlinear movement.

![Alt Text](https://media.giphy.com/media/HO3qcs3F56wmmndjn2/giphy.gif)
