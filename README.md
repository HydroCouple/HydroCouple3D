# HydroCoupleVis

HydroCoupleVis is a visualization and editing application for HydroCouple. 

## Dependencies

HydroCoupleVis can be compiled using the QtCreator project file included with this project or in visual studio using the Qt Visual Studio AddIn. Work is ongoing to add a CMake make file. Modify the project file to make sure that it points to the appropriate libraries on the target machine. Compilation of HydroCoupleVis requires the following frameworks and libraries:

* Qt 5.7.0
* GDAL 2.2.2
* netcdf-cxx4 4.3.0
* hdf5 1.8.17
* [QPropertyModel](https://github.com/HydroCouple/QPropertyModel)
* [HydroCoupleSDK](https://github.com/HydroCouple/HydroCoupleSDK)