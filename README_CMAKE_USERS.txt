-------------------------------------------------
-------------- WINDOWS CMAKE USERS --------------

How to build LBPlibrary with OpenCV 2.4.10 and Visual Studio 2013 from CMAKE.

For Linux users, please see the instruction in README_LINUX_USERS.txt file.

Dependencies:
* GIT (tested with git version 2.7.2.windows.1).
* CMAKE for Windows (tested with cmake version 3.1.1).
* Microsoft Visual Studio (tested with VS2013).

Please follow the instructions below:

1) Go to Windows console.

2) Clone LBPlibrary git repository:
e.g.: git clone https://github.com/carolinepacheco/lbplibrary.git

3) Go to lbplibrary/build folder.
e.g.: C:\lbplibrary\build>_

2) Set your OpenCV PATH:
e.g.:
\> setlocal
\> set OpenCV_DIR=C:\OpenCV2.4.10\build
\> cmake -DOpenCV_DIR=%OpenCV_DIR% -G "Visual Studio 12" ..
or:
\> cmake -DOpenCV_DIR=%OpenCV_DIR% -G "Visual Studio 12 Win64" ..

Now, you will see something like:
-------------------------------------------------
-- The C compiler identification is MSVC 18.0.40629.0
-- The CXX compiler identification is MSVC 18.0.40629.0
-- Check for working C compiler using: Visual Studio 12 2013
-- Check for working C compiler using: Visual Studio 12 2013 -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working CXX compiler using: Visual Studio 12 2013
-- Check for working CXX compiler using: Visual Studio 12 2013 -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- OpenCV ARCH: x86
-- OpenCV RUNTIME: vc12
-- OpenCV STATIC: OFF
-- Found OpenCV 2.4.10 in C:/OpenCV2.4.10/build/x86/vc12/lib
-- You might need to add C:\OpenCV2.4.10\build\x86\vc12\bin to your PATH to be able to run your applications.
-- OpenCV library status:
--     version: 2.4.10
--     libraries: opencv_videostab;opencv_video;opencv_ts;opencv_superres;opencv_stitching;opencv_photo;opencv_ocl;opencv_objdetect;opencv_nonfree;opencv_ml;opencv_legacy;opencv_imgproc;opencv_highgui;opencv_gpu;opencv_flann;opencv_features2d;opencv_core;opencv_contrib;opencv_calib3d
--     include path: C:/OpenCV2.4.10/build/include;C:/OpenCV2.4.10/build/include/opencv
-- Configuring done
-- Generating done
-- Build files have been written to: C:/lbplibrary/build
-------------------------------------------------

3) Include OpenCV binaries in the system path:
\> set PATH=%PATH%;%OpenCV_DIR%\x86\vc12\bin
or:
\> set PATH=%PATH%;%OpenCV_DIR%\x64\vc12\bin

4) Open 'lbp.sln' in Visual Studio and switch to 'RELEASE' mode 
4.1) Note if you are using a Visual Studio version superior than 10, you will need to CANCEL the project wizard update. However, you can go to step (2) and change the Visual Studio version, e.g.: -G "Visual Studio XXX", where XXX is your Visual Studio version.

5) Click on 'lbp' project, and set:
[Configuration Type] Static library (.lib)
[Target Extension] .lib

6) Click on 'ALL_BUILD' project and build!

7) If everything goes well, you can run lbplibrary in the Windows console as follows:

7.1) Running LBPlibrary (compiled with a webcamera):
C:\lbplibrary> build\Release\lbp.exe
