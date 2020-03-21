# ComputerVision_Carspeed
Given a video of a cockpit showing the car speed, the program plots the speed over time curve during the acceleration from 0 to the maximum car speed (C++ and OpenCV).

Third assignment for the *Computer Vision* [course](https://www.dsi.unive.it/~bergamasco/courses/computer_vision_2017_2018.html) of *Università Ca' Foscari Venezia*, academic year 2017/2018


Install OpenCV:

```console
$ git clone --depth 1 https://github.com/opencv/opencv.git
$ cd opencv
$ mkdir build
$ cd build

$ cmake ../ -DCMAKE_BUILD_TYPE="Release"
$ make -j 2

$ make install
```


How to build 'opencv_carspeed':

```console
$ mkdir build
$ cd build
$ cmake ../ -DOpenCV_DIR="<insert the path of your opencv/build directory>"
$ make
```


How to run 'opencv_carspeed':

```console
$ cd build
$ make run
```

or, alternatively:

```console
$ make install
$ cd dist/bin
```
and run the generated executable


Federico Marcuzzi, 2020


