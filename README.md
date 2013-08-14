KinectC3D
=========
This application is proof of concept for using Kinect for performing a gait analysis.

Some modifications made to the Kinect SDK source to capture the skeleton depth data to C3D file using the btk library.
To compile this application link it with BTK 0.1 and the first beta of the Kinect SDK. The application
is based on an included SDK example and adds just one extra button.

Rendering should be redone with Direct3D or OpenGL if one wants to make this work with the 1.7 SDK.

Capturing
---------

With this app you can capture both C3D skeleton tracking data and RGB video from Kinect. Both can be played back simultaneously
using Mokka. C3D capture files also contain scalar values that show the angles between different interconnected skeleton joints.

Screenshot
----------

![Screencap of a captured C3D file](http://www.runwalk.be/uploads/Image/kinect.png)

