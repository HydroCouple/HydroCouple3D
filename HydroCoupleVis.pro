#Author Caleb Amoa Buahin
#Email caleb.buahin@gmail.com
#Date 2016
#License GNU Lesser General Public License (see <http: //www.gnu.org/licenses/> for details).

TARGET = HydroCoupleVis
VERSION = 1.0.0
QT += core widgets gui opengl concurrent

DEFINES += HYDROCOUPLEVIS_LIBRARY
DEFINES += UTAH_CHPC
DEFINES += QT_NO_VERSION_TAGGING

CONFIG += c++11
CONFIG += debug_and_release
CONFIG += optimize_full

contains(DEFINES,HYDROCOUPLEVIS_LIBRARY){

  TEMPLATE = lib
  message("Compiling as library")

  } else {

  TEMPLATE = app
  message("Compiling as application")

}


INCLUDEPATH += ./include \
                /usr/local/include \
               ../HydroCouple/include \
               ../QPropertyModel/include

PRECOMPILED_HEADER += ./include/stdafx.h

HEADERS += ./include/stdafx.h \
           ./include/hydrocouplevis.h \
           ./include/graphicsview.h

SOURCES += ./src/stdafx.cpp \
           ./src/main.cpp \
           ./src/hydrocouplevis.cpp \
           ./src/graphicsview.cpp

win32{
    QMAKE_CXXFLAGS += /MP
}

CONFIG(debug,debug|release) {

    win32 {
       QMAKE_CXXFLAGS_DEBUG = $$QMAKE_CXXFLAGS /MDd  /O2
    }

    macx {
     QMAKE_CFLAGS_DEBUG = $$QMAKE_CFLAGS -g -O3
     QMAKE_CXXFLAGS_DEBUG = $$QMAKE_CXXFLAGS -g -O3
    }

    linux {
     QMAKE_CFLAGS_DEBUG = $$QMAKE_CFLAGS -g -O3
     QMAKE_CXXFLAGS_DEBUG = $$QMAKE_CXXFLAGS -g -O3
    }

     macx{
        LIBS += -L../QPropertyModel/build/debug -lQPropertyModel
     }

     linux{
        LIBS += -L../QPropertyModel/build/debug -lQPropertyModel
     }

     win32{
        LIBS += -L../QPropertyModel/build/debug -lQPropertyModel1
     }

     DESTDIR = $$PWD/build/debug
     OBJECTS_DIR = $$DESTDIR/.obj
     MOC_DIR = $$DESTDIR/.moc
     RCC_DIR = $$DESTDIR/.qrc
     UI_DIR = $$DESTDIR/.ui
}

CONFIG(release,debug|release){

   win32 {
    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS /MD
   }

    macx {
      LIBS += -L../QPropertyModel/lib/macx -lQPropertyModel
    }

    linux {
      LIBS += -L../QPropertyModel/lib/linux -lQPropertyModel
    }

    win32 {
      LIBS += -L../QPropertyModel/lib/win32 -lQPropertyModel1
    }

     contains(DEFINES,HYDROCOUPLEVIS_LIBRARY){
         #MacOS
         macx{
             DESTDIR = $$PWD/lib/macx
          }

         #Linux
         linux{
             DESTDIR = $$PWD/lib/linux
          }

         #Windows
         win32{
             DESTDIR = $$PWD/lib/win32
          }
     } else {
         #MacOS
         macx{
             DESTDIR = $$PWD/bin/macx
          }

         #Linux
         linux{
             DESTDIR = $$PWD/bin/linux
          }

         #Windows
         win32{
             DESTDIR = $$PWD/bin/win32
          }
     }


    RELEASE_EXTRAS = $$PWD/build/release
    OBJECTS_DIR = $$RELEASE_EXTRAS/.obj
    MOC_DIR = $$RELEASE_EXTRAS/.moc
    RCC_DIR = $$RELEASE_EXTRAS/.qrc
    UI_DIR = $$RELEASE_EXTRAS/.ui
}


FORMS += ./forms/hydrocouplevis.ui

RESOURCES += ./resources/hydrocouplevis.qrc
RC_FILE = ./resources/HydroCoupleVis.rc

macx{
    ICON = ./resources/HydroCoupleVis.icns
}
