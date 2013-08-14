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

Publications
------------
A paper was published some time ago using the Kinect to replace different pressure and angular velocity measuring sensors.
Might be a [good starting point](http://research.microsoft.com/pubs/172555/KinectGaitEMBC2012.pdf) for further developments.
m

Possible Improvements
---------------------
Some new ideas and improvements to the C3D capture files are the following

* Double the frame rate by interpolating each joint's coordinate data
* Adding foot 'strike' events in C3D by calibrating the 3D space to take the ground plane into account
* Caclulation of the elbow's velocities in the capture file by measuring the angular difference over time
* Calculation of the acceleration of the center of mass of the patient by using the method mentioned in the paper above
* Visualisation of these measurements in an XY scantter plot

Screenshot
----------

![Screencap of a captured C3D file](http://www.runwalk.be/uploads/Image/kinect.png)

