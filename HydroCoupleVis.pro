 #Author Caleb Amoa Buahin
#Email caleb.buahin@gmail.com
#Date 2016
#License GNU General Public License (see <http://www.gnu.org/licenses/> for details).

DEFINES += HYDROCOUPLEVIS_LIBRARY
DEFINES += UTAH_CHPC

CONFIG += c++11
CONFIG += debug_and_release

contains(DEFINES,HYDROCOUPLEVIS_LIBRARY){
  TEMPLATE = lib
  message("Compiling as library")
} else {
  TEMPLATE = app
  message("Compiling as application")
}

VERSION = 1.0.0.0
TARGET = HydroCoupleVis
QT += core widgets gui opengl concurrent




INCLUDEPATH += ./include \
                /usr/local/include \
               ../HydroCouple/include \
               ../HydroCoupleSDK/include \
               ../../QPropertyModel/include

PRECOMPILED_HEADER += ./include/stdafx.h

HEADERS += ./include/stdafx.h \
           ./include/hydrocouplevis.h \
           ./include/graphicsview.h

SOURCES += ./src/stdafx.cpp \
           ./src/main.cpp \
           ./src/hydrocouplevis.cpp \
           ./src/graphicsview.cpp

linux {

INCLUDEPATH += /usr/include \
               ../gdal/include

LIBS += -L/usr/lib/ogdi -lgdal \
        -L../gdal/lib -lgdal

    contains(DEFINES,UTAH_CHPC){

         INCLUDEPATH += /uufs/chpc.utah.edu/sys/installdir/hdf5/1.8.17-c7/include \
                        /uufs/chpc.utah.edu/sys/installdir/netcdf-c/4.3.3.1/include \
                        /uufs/chpc.utah.edu/sys/installdir/netcdf-cxx/4.3.0-c7/include

         LIBS += -L/uufs/chpc.utah.edu/sys/installdir/hdf5/1.8.17-c7/lib -lhdf5 \
                 -L/uufs/chpc.utah.edu/sys/installdir/netcdf-cxx/4.3.0-c7/lib -lnetcdf_c++4

         message("Compiling on CHPC")
    }
}

CONFIG(debug,debug|release) {

   LIBS += -L./../../QPropertyModel/build/debug -lQPropertyModel \
           -L./../HydroCoupleSDK/build/debug -lHydroCoupleSDK

   DESTDIR = ./build/debug
   OBJECTS_DIR = $$DESTDIR/.obj
   MOC_DIR = $$DESTDIR/.moc
   RCC_DIR = $$DESTDIR/.qrc
   UI_DIR = $$DESTDIR/.ui
}

CONFIG(release,debug|release){

   macx{
      LIBS += -L./../../QPropertyModel/lib/macx -lQPropertyModel \
              -L./../HydroCoupleSDK/lib/macx -lHydroCoupleSDK
   }

   linux{
      LIBS += -L./../../QPropertyModel/lib/linux -lQPropertyModel \
              -L./../HydroCoupleSDK/lib/linux -lHydroCoupleSDK
   }

   win32{
      LIBS += -L./../../QPropertyModel/lib/win32 -lQPropertyModel1 \
              -L./../HydroCoupleSDK/lib/win32 -lHydroCoupleSDK
   }

     contains(DEFINES,HYDROCOUPLEVIS_LIBRARY){
         #MacOS
         macx{
             DESTDIR = lib/macx
         }

         #Linux
         linux{
             DESTDIR = lib/linux
         }

         #Windows
         win32{
             DESTDIR = lib/win32
         }
     } else {
         #MacOS
         macx{
             DESTDIR = bin/macx
         }

         #Linux
         linux{
             DESTDIR = bin/linux
         }

         #Windows
         win32{
             DESTDIR = bin/win32
         }
     }


    RELEASE_EXTRAS = ./build/release
    OBJECTS_DIR = $$RELEASE_EXTRAS/.obj
    MOC_DIR = $$RELEASE_EXTRAS/.moc
    RCC_DIR = $$RELEASE_EXTRAS/.qrc
    UI_DIR = $$RELEASE_EXTRAS/.ui
}


FORMS += forms/hydrocouplevis.ui

RESOURCES += ./resources/hydrocouplevis.qrc
RC_FILE = ./resources/HydroCoupleVis.rc

macx{
ICON = ./resources/HydroCoupleVis.icns
}
