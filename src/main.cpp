/*!
 * \author Caleb Amoa Buahin <caleb.buahin@gmail.com>
 * \version 1.0.0
 * \description
 * \license
 * This file and its associated files, and libraries are free software.
 * You can redistribute it and/or modify it under the terms of the
 * Lesser GNU Lesser General Public License as published by the Free Software Foundation;
 * either version 3 of the License, or (at your option) any later version.
 * This file and its associated files is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.(see <http://www.gnu.org/licenses/> for details)
 * \copyright Copyright 2014-2018, Caleb Buahin, All rights reserved.
 * \date 2014-2018
 * \pre
 * \bug
 * \warning
 * \todo
 */

#include "stdafx.h"
#include <QtWidgets/QApplication>
#include "hydrocouplevis.h"
#include <QFile>
#include <random>

//#include "spatial/linestring.h"
//#include "spatial/point.h"
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

  //  for(int i = 0 ; i < 1000 ; i++)
  //  {
  //    visApp->addGeometry(new HCPoint(uniRandNum(gen) , uniRandNum(gen) , uniRandNum(gen)));
  //  }


  return a.exec();
}
