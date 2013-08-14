KinectC3D
=========
This application is served as part of a proof of concept for using Kinect in a gait analysis application.

Some modifications were done to the Kinect SDK source to capture the skeleton depth data to C3D file using the [btk library](https://code.google.com/p/b-tk/).
To compile this application link it with BTK 0.1 and the first beta of the Kinect SDK.
Rendering should be redone with Direct3D or OpenGL if one wants to make this work with the 1.7 SDK.

Capturing
---------

With this app you can capture both C3D skeleton tracking data and RGB video from Kinect simultaneously using [Mokka](http://b-tk.googlecode.com/svn/web/mokka/index.html). 
The captured C3D files also contain angular values of the interconnected skeleton joints.

Screenshot
----------

![Screencap of a captured C3D file](http://www.runwalk.be/uploads/Image/kinect.png)

