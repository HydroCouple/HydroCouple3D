#include "stdafx.h"
#include <QtWidgets/QApplication>
#include "hydrocouplevis.h"
#include <QFile>
#include <random>

#include "spatial/linestring.h"
#include "spatial/point.h"
#include "ui_hydrocouplevis.h"

void setApplicationStyle(QApplication& application)
{
  application.setStyle("Fusion");
  QFile file(":/HydroCoupleVis/Styles");
  file.open(QFile::ReadOnly);
  QString styleSheet = QLatin1String(file.readAll());
  application.setStyleSheet(styleSheet);
}

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setOrganizationName("HydroCouple");
  a.setOrganizationDomain("hydrocouple.org");
  a.setApplicationName("hydrocouplevis");
  setApplicationStyle(a);

  HydroCoupleVis* visApp = HydroCoupleVis::getInstance();

  visApp->show();


  static std::default_random_engine gen;
  std::uniform_real_distribution<float> uniRandNum(0,1000.00);

//  HCLineString* lineString = new HCLineString();

  for(int i = 0 ; i < 1000 ; i++)
  {
    visApp->addGeometry(new HCPoint(uniRandNum(gen) , uniRandNum(gen) , uniRandNum(gen)));
  }


  return a.exec();
}
