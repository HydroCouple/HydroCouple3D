#Author Caleb Amoa Buahin
#Email caleb.buahin@gmail.com
#Date 2016
#License GNU General Public License (see <http://www.gnu.org/licenses/> for details).

DEFINES += HYDROCOUPLESDK_LIBRARY

contains(DEFINES,HYDROCOUPLESDK_LIBRARY){
  TEMPLATE = lib
  message("Compiling as library")
} else {
  TEMPLATE = app
  message("Compiling as application")
}

VERSION = 1.0.0.0
TARGET = HydroCoupleVis
QT += core widgets gui opengl




INCLUDEPATH += ./include \
                /usr/local/include \
               ../HydroCouple/include \
               ../HydroCoupleSDK/include \
               ../../QPropertyModel/QPropertyModel/include

PRECOMPILED_HEADER += ./include/stdafx.h

HEADERS += ./include/stdafx.h \
           ./include/hydrocouplevis.h \
           ./include/graphicsview.h

SOURCES += ./src/stdafx.cpp \
           ./src/main.cpp \
           ./src/hydrocouplevis.cpp \
           ./src/graphicsview.cpp


CONFIG(debug,debug|release) {

   macx{
   LIBS += -L./../../QPropertyModel/QPropertyModel/build/debug -lQPropertyModel \
           -L./../HydroCoupleSDK/build/debug -lHydroCoupleSDK

   }

   linux{
   LIBS += -L./../../QPropertyModel/QPropertyModel/build/debug -lQPropertyModel \
           -L./../HydroCoupleSDK/build/debug -lHydroCoupleSDK \

   }

   win32{
   LIBS += -L./../../QPropertyModel/QPropertyModel/build/debug -lQPropertyModel1 \
           -L./../HydroCoupleSDK/build/debug -lHydroCoupleSDK1
   }

   DESTDIR = ./build/debug
   OBJECTS_DIR = $$DESTDIR/.obj
   MOC_DIR = $$DESTDIR/.moc
   RCC_DIR = $$DESTDIR/.qrc
   UI_DIR = $$DESTDIR/.ui
}

CONFIG(release,debug|release){

   macx{
      LIBS += -L./../../QPropertyModel/QPropertyModel/lib -lQPropertyModel \
              -L./../HydroCoupleSDK/lib -lHydroCoupleSDK
   }

   linux{
      LIBS += -L./../../QPropertyModel/QPropertyModel/lib -lQPropertyModel \
              -L./../HydroCoupleSDK/lib -lHydroCoupleSDK
   }

   win32{
      LIBS += -L./../../QPropertyModel/QPropertyModel/lib -lQPropertyModel1 \
              -L./../HydroCoupleSDK/lib -lHydroCoupleSDK
   }

contains(DEFINES,HYDROCOUPLESDK_LIBRARY){
  DESTDIR = lib
} else {
      DESTDIR = bin
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
