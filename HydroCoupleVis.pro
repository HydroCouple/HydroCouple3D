#Author Caleb Amoa Buahin
#Email caleb.buahin@gmail.com
#Date 2016
#License GNU General Public License (see <http: //www.gnu.org/licenses/> for details).

VERSION = 1.0.0
TARGET = HydroCoupleVis

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


QT += core widgets gui opengl concurrent

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

linux{

    INCLUDEPATH += /usr/include

    contains(DEFINES,UTAH_CHPC){
         message("Compiling on CHPC")
     }
}

CONFIG(debug,debug|release) {

   macx{
   LIBS += -L$$PWD/../QPropertyModel/build/debug -lQPropertyModel.1.0.0
     }

   linux{
   LIBS += -L$$PWD/../QPropertyModel/build/debug -lQPropertyModel.so.1.0.0
     }

   win32{
   LIBS += -L$$PWD/../QPropertyModel/build/debug -lQPropertyModel1
     }

   DESTDIR = $$PWD/build/debug
   OBJECTS_DIR = $$DESTDIR/.obj
   MOC_DIR = $$DESTDIR/.moc
   RCC_DIR = $$DESTDIR/.qrc
   UI_DIR = $$DESTDIR/.ui
}

CONFIG(release,debug|release){

   macx{
      LIBS += -L$$PWD/../QPropertyModel/lib/macx -lQPropertyModel.1.0.0
     }

  linux{
      LIBS += -L$$PWD/../QPropertyModel/lib/linux -lQPropertyModel.so.1.0.0
     }

  win32{
      LIBS += -L$$PWD/../QPropertyModel/lib/win32 -lQPropertyModel1
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
