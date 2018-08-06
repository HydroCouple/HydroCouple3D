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

#ifndef HYDROCOUPLEVIS_CPP
#define HYDROCOUPLEVIS_CPP

#include "stdafx.h"
#include "hydrocouplevis.h"
#include "ui_hydrocouplevis.h"

#include <QMessageBox>
#include <QProgressBar>
#include <QStandardItemModel>
#include <iostream>
#include <QtConcurrent>

using namespace HydroCouple;
using namespace HydroCouple::Spatial;
using namespace HydroCouple::SpatioTemporal;

HydroCoupleVis::HydroCoupleVis(QWidget *parent):
  QMainWindow(parent),
  ui(new Ui::HydroCoupleVisClass)
{
  ui->setupUi(this);
  initializeSignalSlotConnections();
  initializeGUIElements();
  initializeTreeViewModel();
}

HydroCoupleVis::~HydroCoupleVis()
{

}

HydroCoupleVis *HydroCoupleVis::getInstance()
{
  static HydroCoupleVis* workingInstance = new HydroCoupleVis();
  return workingInstance;
}

void HydroCoupleVis::addModelComponent(IModelComponent *modelComponent)
{
  if(!m_modelGraphicsData.contains(modelComponent))
  {
    for(IOutput* output : modelComponent->outputs())
    {
      if(dynamic_cast<IGeometryComponentDataItem*>(output))
      {
        m_modelGraphicsData[modelComponent][output] = QList<QGraphicsItem*>();
        QList<QGraphicsItem*> & t = m_modelGraphicsData[modelComponent][output];
        QList<QGraphicsItem*> items = addGeometryDataItem(dynamic_cast<IGeometryComponentDataItem*>(output));

        if(items.length())
          t.append(items);
      }
      else if(dynamic_cast<ITimeGeometryComponentDataItem*>(output))
      {
        m_modelGraphicsData[modelComponent][output] = QList<QGraphicsItem*>();
        QList<QGraphicsItem*> & t = m_modelGraphicsData[modelComponent][output];
        QList<QGraphicsItem*> items = addGeometryDataItem(dynamic_cast<ITimeGeometryComponentDataItem*>(output));

        if(items.length())
          t.append(items);
      }
      else if(dynamic_cast<IPolyhedralSurfaceComponentDataItem*>(output))
      {
        m_modelGraphicsData[modelComponent][output] = QList<QGraphicsItem*>();
        QList<QGraphicsItem*> items = addGeometryDataItem(dynamic_cast<IPolyhedralSurfaceComponentDataItem*>(output));
        QList<QGraphicsItem*> & t = m_modelGraphicsData[modelComponent][output];

        if(items.length())
          t.append(items);
      }
    }

    for(IInput* input : modelComponent->inputs())
    {
      if(dynamic_cast<IGeometryComponentDataItem*>(input))
      {
        m_modelGraphicsData[modelComponent][input] = QList<QGraphicsItem*>();
        QList<QGraphicsItem*> & t = m_modelGraphicsData[modelComponent][input];
        QList<QGraphicsItem*> items = addGeometryDataItem(dynamic_cast<IGeometryComponentDataItem*>(input));

        if(items.length())
          t.append(items);
      }
      else if(dynamic_cast<ITimeGeometryComponentDataItem*>(input))
      {
        m_modelGraphicsData[modelComponent][input] = QList<QGraphicsItem*>();
        QList<QGraphicsItem*> & t = m_modelGraphicsData[modelComponent][input];
        QList<QGraphicsItem*> items = addGeometryDataItem(dynamic_cast<ITimeGeometryComponentDataItem*>(input));

        if(items.length())
          t.append(items);
      }
      else if(dynamic_cast<IPolyhedralSurfaceComponentDataItem*>(input))
      {
        m_modelGraphicsData[modelComponent][input] = QList<QGraphicsItem*>();
        QList<QGraphicsItem*> items = addGeometryDataItem(dynamic_cast<IPolyhedralSurfaceComponentDataItem*>(input));
        QList<QGraphicsItem*> & t = m_modelGraphicsData[modelComponent][input];

        if(items.length())
          t.append(items);
      }
    }

    for(IArgument* argument : modelComponent->arguments())
    {

      if(dynamic_cast<IGeometryComponentDataItem*>(argument))
      {
        m_modelGraphicsData[modelComponent][argument] = QList<QGraphicsItem*>();
        QList<QGraphicsItem*> & t = m_modelGraphicsData[modelComponent][argument];
        QList<QGraphicsItem*> items = addGeometryDataItem(dynamic_cast<IGeometryComponentDataItem*>(argument));

        if(items.length())
          t.append(items);
      }
      else if(dynamic_cast<ITimeGeometryComponentDataItem*>(argument))
      {
        m_modelGraphicsData[modelComponent][argument] = QList<QGraphicsItem*>();
        QList<QGraphicsItem*> & t = m_modelGraphicsData[modelComponent][argument];
        QList<QGraphicsItem*> items = addGeometryDataItem(dynamic_cast<ITimeGeometryComponentDataItem*>(argument));

        if(items.length())
          t.append(items);
      }
      else if(dynamic_cast<IPolyhedralSurfaceComponentDataItem*>(argument))
      {
        m_modelGraphicsData[modelComponent][argument] = QList<QGraphicsItem*>();
        QList<QGraphicsItem*> & t = m_modelGraphicsData[modelComponent][argument];
        QList<QGraphicsItem*> items = addGeometryDataItem(dynamic_cast<IPolyhedralSurfaceComponentDataItem*>(argument));
        if(items.length())
          t.append(items);
      }
    }


    connect(dynamic_cast<QObject*>(modelComponent), SIGNAL(componentStatusChanged(const QSharedPointer<HydroCouple::IComponentStatusChangeEventArgs> &)),
            this, SLOT(onComponentStatusChanged(const QSharedPointer<HydroCouple::IComponentStatusChangeEventArgs> &)));

    connect(dynamic_cast<QObject*>(modelComponent), SIGNAL(propertyChanged(const QString &)), this, SLOT(onPropertyChanged(const QString&)));

    addToTreeView(modelComponent);

  }
}

bool HydroCoupleVis::removeModelComponent(IModelComponent *modelComponent)
{
  if(m_modelGraphicsData.contains(modelComponent))
  {
    QGraphicsScene *scene = ui->graphicsViewHydroCoupleVis->scene();

    QList<QGraphicsView*> views = scene->views();

    for(QGraphicsView *view : views)
    {
      view->setScene(nullptr);
    }

    QMap<IComponentDataItem*,QList<QGraphicsItem*>> citems = m_modelGraphicsData[modelComponent];

    for( QMap<IComponentDataItem*,QList<QGraphicsItem*> >::iterator it = citems.begin();
         it != citems.end() ; it++)
    {
      for(QGraphicsItem* item : it.value())
      {
        if(item->parentItem() == nullptr)
          delete item;
      }
    }

    for(QGraphicsView *view : views)
    {
      view->setScene(scene);
    }

    removeFromTreeView(modelComponent);
    bool removed = m_modelGraphicsData.remove(modelComponent);
    return removed;
  }

  return false;
}

void HydroCoupleVis::closeEvent(QCloseEvent *event)
{
  writeSettings();
}

void HydroCoupleVis::initializeSignalSlotConnections()
{
  //Toolbar actions
  connect(ui->actionDefaultSelect, SIGNAL(toggled(bool)), this, SLOT(onSetCurrentTool(bool)));
  connect(ui->actionZoomIn, SIGNAL(toggled(bool)), this, SLOT(onSetCurrentTool(bool)));
  connect(ui->actionZoomOut, SIGNAL(toggled(bool)), this, SLOT(onSetCurrentTool(bool)));
  connect(ui->actionPan, SIGNAL(toggled(bool)), this, SLOT(onSetCurrentTool(bool)));
  connect(ui->actionZoomExtent, SIGNAL(triggered()), this, SLOT(onZoomExtent()));


  //  connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(onNewProject()));
  connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
  //  connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenFiles()));
  //  connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(onSave()));
  //  connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(onSaveAs()));
  //  connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(onPrint()));

  connect(ui->graphicsViewHydroCoupleVis , &GraphicsView::mouseMapCoordinatesChanged,
          this, &HydroCoupleVis::onMouseMapCoordinatesChanged);

  //about
  connect(ui->actionAbout , SIGNAL(triggered()) , this,  SLOT(onAbout()));
  //preferences
  connect(ui->actionPreferences, SIGNAL(triggered()) , this,  SLOT(onPreferences()));

}

void HydroCoupleVis::initializeGUIElements()
{

  QActionGroup* actionGroupSelect = new QActionGroup(this);
  actionGroupSelect->addAction(ui->actionDefaultSelect);
  actionGroupSelect->addAction(ui->actionZoomIn);
  actionGroupSelect->addAction(ui->actionZoomOut);
  actionGroupSelect->addAction(ui->actionPan);
  ui->actionDefaultSelect->setChecked(true);

  //Progressbar
  m_progressBar = new QProgressBar(this);
  m_progressBar->setMinimum(0);
  m_progressBar->setMaximum(100);
  m_progressBar->setVisible(false);
  m_progressBar->setToolTip("Progress bar");
  m_progressBar->setWhatsThis("Sets the progress of a process");
  m_progressBar->setStatusTip("Progress bar");

  m_progressBar->setMaximumWidth(300);
  m_progressBar->setMaximumHeight(18);

  m_progressBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
  ui->statusbarMain->setContentsMargins(0, 0, 0, 0);

  ui->statusbarMain->addPermanentWidget(m_progressBar);

#ifdef _WIN32 // note the underscore: without it, it's not msdn official!

  //  actionPreferences->setText("Options");
  //  menuTools->addAction(actionPreferences);
  //  menuHelp->addAction(actionAbout);

#elif __unix__ // all unices, not all compilers
#elif __linux__
#elif __APPLE__

  for (QObject* child : ui->menuBarMain->children())
  {
    QMenu* macMenu;

    if((macMenu = dynamic_cast<QMenu*>(child)))
    {
      if(!macMenu->title().compare("File" , Qt::CaseInsensitive))
      {
        macMenu->addAction(ui->actionAbout);
        macMenu->addSeparator();
        macMenu->addAction(ui->actionPreferences);
        macMenu->addSeparator();
      }
      else if(!macMenu->title().compare("Help" , Qt::CaseInsensitive))
      {
        macMenu->addAction("");
      }
    }
  }

#endif
}

void HydroCoupleVis::initializeTreeViewModel()
{
  m_tableOfContentTreeViewModel = new QStandardItemModel(this);
  QStringList temp; temp << "";
  m_tableOfContentTreeViewModel->setHorizontalHeaderLabels(temp);
  m_tableOfContentTreeViewModel->setSortRole(Qt::DisplayRole);
  ui->treeViewModelComponents->setModel(m_tableOfContentTreeViewModel);
  connect(m_tableOfContentTreeViewModel, &QStandardItemModel::itemChanged , this , &HydroCoupleVis::onItemChanged);
}

void HydroCoupleVis::resetTreeView()
{
  m_modelGraphicsData.clear();

  for(IModelComponent *modelComponent : m_modelGraphicsData.keys())
  {
    addToTreeView(modelComponent);
  }
}

void HydroCoupleVis::addToTreeView(IModelComponent *modelComponent)
{
  if(m_modelGraphicsData.contains(modelComponent))
  {
    QStandardItem *modelComponentItems = new QStandardItem(QIcon(), modelComponent->caption());
    modelComponentItems->setCheckable(true);
    modelComponentItems->setUserTristate(false);
    QHash<QString,QVariant> modelComponentData;
    modelComponentData["ModelId"] = modelComponent->id();
    modelComponentData["NodeType"] = "Model";
    modelComponentItems->setData(modelComponentData,Qt::UserRole);

    m_tableOfContentTreeViewModel->invisibleRootItem()->appendRow(modelComponentItems);

    QStandardItem *argumentDataItems = new QStandardItem(QIcon(), "Arguments");
    argumentDataItems->setCheckable(true);
    argumentDataItems->setUserTristate(false);
    QHash<QString,QVariant> argumentsData;
    argumentsData["ModelId"] = modelComponent->id();
    argumentsData["NodeType"] = "ArgumentsHeader";
    argumentDataItems->setData(argumentsData,Qt::UserRole);
    modelComponentItems->appendRow(argumentDataItems);

    QStandardItem *inputDataItems = new QStandardItem(QIcon(), "Inputs");
    inputDataItems->setCheckable(true);
    inputDataItems->setUserTristate(false);
    QHash<QString,QVariant> inputData;
    inputData["ModelId"] = modelComponent->id();
    inputData["NodeType"] = "InputsHeader";
    inputDataItems->setData(inputData,Qt::UserRole);
    modelComponentItems->appendRow(inputDataItems);

    QStandardItem *outputDataItems = new QStandardItem(QIcon(), "Outputs");
    outputDataItems->setCheckable(true);
    outputDataItems->setUserTristate(false);
    QHash<QString,QVariant> outputData;
    outputData["ModelId"] = modelComponent->id();
    outputData["NodeType"] = "OutputsHeader";
    outputDataItems->setData(outputData,Qt::UserRole);
    modelComponentItems->appendRow(outputDataItems);

    for(IComponentDataItem *cdataItem : m_modelGraphicsData[modelComponent].keys())
    {
      if(dynamic_cast<IOutput*>(cdataItem))
      {
        QStandardItem *outputItem = new QStandardItem(QIcon(), cdataItem->caption());
        outputItem->setCheckable(true);
        outputItem->setUserTristate(false);
        QHash<QString,QVariant> outputArgs;
        outputArgs["ModelId"] = modelComponent->id();
        outputArgs["NodeType"] = "Output";
        outputArgs["ComponentDataItemId"] = cdataItem->id();
        outputItem->setData(outputArgs,Qt::UserRole);
        outputDataItems->appendRow(outputItem);
      }
      else if(dynamic_cast<IInput*>(cdataItem))
      {
        QStandardItem *inputItem = new QStandardItem(QIcon(), cdataItem->caption());
        inputItem->setCheckable(true);
        inputItem->setUserTristate(false);
        QHash<QString,QVariant> inputArgs;
        inputArgs["ModelId"] = modelComponent->id();
        inputArgs["NodeType"] = "Input";
        inputArgs["ComponentDataItemId"] = cdataItem->id();
        inputItem->setData(inputArgs,Qt::UserRole);
        inputDataItems->appendRow(inputItem);
      }
      else if(dynamic_cast<IArgument*>(cdataItem))
      {
        QStandardItem *argumentItem = new QStandardItem(QIcon(), cdataItem->caption());
        argumentItem->setCheckable(true);
        argumentItem->setUserTristate(false);
        QHash<QString,QVariant> dataArgs;
        dataArgs["ModelId"] = modelComponent->id();
        dataArgs["NodeType"] = "Argument";
        dataArgs["ComponentDataItemId"] = cdataItem->id();
        argumentItem->setData(dataArgs,Qt::UserRole);

        argumentDataItems->appendRow(argumentItem);
      }
    }

    ui->treeViewModelComponents->expandToDepth(1);
    modelComponentItems->setCheckState(Qt::Checked);
  }
}

void HydroCoupleVis::removeFromTreeView(IModelComponent *modelComponent)
{
  QList<QStandardItem*> items = m_tableOfContentTreeViewModel->findItems(modelComponent->caption());

  for(QStandardItem *item : items)
  {
    QHash<QString,QVariant> userData = item->data(Qt::UserRole).toHash();

    if(userData.contains("NodeType") && !userData["NodeType"].toString().compare("Model"))
    {
      m_tableOfContentTreeViewModel->removeRow(item->row());
    }
  }
}

void HydroCoupleVis::readSettings()
{
  m_settings.beginGroup("MainWindow");
  {
    //!HydroCoupleComposer GUI
    this->restoreState(m_settings.value("WindowState", saveState()).toByteArray());
    this->setWindowState((Qt::WindowState)m_settings.value("WindowStateEnum", (int)this->windowState()).toInt());
    this->setGeometry(m_settings.value("Geometry", geometry()).toRect());

    //!recent files
    //    m_recentFiles = m_settings.value("Recent Files", m_recentFiles).toStringList();
    //    while (m_recentFiles.size() > 10)
    //    {
    //      m_recentFiles.removeLast();
    //    }
    //    onUpdateRecentFiles();

  }
  m_settings.endGroup();
}

void HydroCoupleVis::writeSettings()
{
  m_settings.beginGroup("MainWindow");
  {
    m_settings.setValue("WindowState", saveState());
    m_settings.setValue("WindowStateEnum", (int)this->windowState());
    m_settings.setValue("Geometry", geometry());
    m_settings.setValue("Recent Files", m_recentFiles);
  }
  m_settings.endGroup();
}

IModelComponent *HydroCoupleVis::findModelById(const QString &id)
{
  for(IModelComponent *modelComponent : m_modelGraphicsData.keys())
  {
    if(!modelComponent->id().compare(id))
    {
      return modelComponent;
    }
  }

  return nullptr;
}

IComponentDataItem *HydroCoupleVis::findComponentDataItemById(const QString &id, IModelComponent *model)
{
  if(m_modelGraphicsData.contains(model))
  {
    QMap<IComponentDataItem*, QList<QGraphicsItem*>> citems = m_modelGraphicsData[model];

    for(IComponentDataItem * citem : citems.keys())
    {
      if(!citem->id().compare(id))
      {
        return citem;
      }
    }
  }

  return nullptr;
}

void HydroCoupleVis::setChildCheckStates(QStandardItem *parent)
{
  if(parent->rowCount())
  {
    for(int i = 0 ; i < parent->rowCount() ; i++)
    {
      QStandardItem *item = parent->child(i);

      if(item->checkState() != parent->checkState())
      {
        item->setCheckState(parent->checkState());
        setChildCheckStates(item);
      }
    }
  }
}

QList<QGraphicsItem*> HydroCoupleVis::addGeometryDataItem(IGeometryComponentDataItem *geometryData )
{
  QList<QGraphicsItem*> items;

  QBrush brush = randomBrush();
  QPen pen = randomPen();

  if(geometryData->geometryType() == IGeometry::LineString ||
     geometryData->geometryType() == IGeometry::LineStringZ ||
     geometryData->geometryType() == IGeometry::LineStringZM ||
     geometryData->geometryType() == IGeometry::LineStringM
     )
  {
    brush.setColor(Qt::transparent);
  }
  else if (geometryData->geometryType() == IGeometry::Polygon ||
           geometryData->geometryType() == IGeometry::PolygonZ ||
           geometryData->geometryType() == IGeometry::PolygonZM ||
           geometryData->geometryType() == IGeometry::PolygonM ||
           geometryData->geometryType() == IGeometry::Triangle ||
           geometryData->geometryType() == IGeometry::TriangleZ ||
           geometryData->geometryType() == IGeometry::TriangleZM ||
           geometryData->geometryType() == IGeometry::TriangleM)
  {
    brush.setColor(Qt::transparent);
  }


  for(int i = 0 ; i < geometryData->geometryCount() ; i++)
  {

    IGeometry *geom = geometryData->geometry(i);
    QList<QGraphicsItem*> titems = addGeometry(geom,brush,pen);

    if(titems.length())
      items.append(titems);
  }

  onZoomExtent();
  return items;
}

QList<QGraphicsItem*> HydroCoupleVis::addGeometryDataItem(ITimeGeometryComponentDataItem *geometryData )
{
  QList<QGraphicsItem*> items;

  QBrush brush = randomBrush();
  QPen pen = randomPen();

  if(geometryData->geometryType() == IGeometry::LineString ||
     geometryData->geometryType() == IGeometry::LineStringZ ||
     geometryData->geometryType() == IGeometry::LineStringZM ||
     geometryData->geometryType() == IGeometry::LineStringM
     )
  {
    brush.setColor(Qt::transparent);
  }
  else if (geometryData->geometryType() == IGeometry::Polygon ||
           geometryData->geometryType() == IGeometry::PolygonZ ||
           geometryData->geometryType() == IGeometry::PolygonZM ||
           geometryData->geometryType() == IGeometry::PolygonM ||
           geometryData->geometryType() == IGeometry::Triangle ||
           geometryData->geometryType() == IGeometry::TriangleZ ||
           geometryData->geometryType() == IGeometry::TriangleZM ||
           geometryData->geometryType() == IGeometry::TriangleM)
  {
    brush.setColor(Qt::transparent);
  }


  for(int i = 0 ; i < geometryData->geometryCount() ; i++)
  {

    IGeometry *geom = geometryData->geometry(i);
    QList<QGraphicsItem*> titems = addGeometry(geom,brush,pen);

    if(titems.length())
      items.append(titems);
  }

  onZoomExtent();
  return items;
}

QList<QGraphicsItem*> HydroCoupleVis::addGeometryDataItem(IPolyhedralSurfaceComponentDataItem *geometryData)
{

  QBrush brush = QBrush(Qt::transparent);
  //  QBrush brush = randomBrush();
  QPen pen = randomPen();
  pen.setWidthF(1.0);
  QList<QGraphicsItem*> items = addGeometry(geometryData->polyhedralSurface(),brush,pen);
  onZoomExtent();
  return items;
}

QList<QGraphicsItem*> HydroCoupleVis::addGeometry(IGeometry *geometry, const QBrush &brush, const QPen &pen)
{
  QList<QGraphicsItem*> items;

  switch (geometry->geometryType())
  {
    case IGeometry::Point:
    case IGeometry::PointZ:
    case IGeometry::PointZM:
    case IGeometry::PointM:
      {
        IPoint *point = dynamic_cast<IPoint*>(geometry);
        QGraphicsItem* item = toPoint(point,brush,pen);
        items.append(item);
        ui->graphicsViewHydroCoupleVis->scene()->addItem(item);
      }
      break;
    case IGeometry::LineString:
    case IGeometry::LineStringZ:
    case IGeometry::LineStringM:
    case IGeometry::LineStringZM:
      {
        ILineString *lineString = dynamic_cast<ILineString*>(geometry);
        QGraphicsItem* item = toPath(lineString,brush,pen);
        items.append(item);
        ui->graphicsViewHydroCoupleVis->scene()->addItem(item);
      }
      break;
    case IGeometry::Polygon:
    case IGeometry::PolygonZ:
    case IGeometry::PolygonM:
    case IGeometry::PolygonZM:
    case IGeometry::Triangle:
    case IGeometry::TriangleZ:
    case IGeometry::TriangleM:
    case IGeometry::TriangleZM:
      {
        IPolygon *polygon = dynamic_cast<IPolygon*>(geometry);
        QGraphicsItem* item = toPolygon(polygon,brush,pen);
        items.append(item);
        ui->graphicsViewHydroCoupleVis->scene()->addItem(item);
      }
      break;
    case IGeometry::GeometryCollection:
    case IGeometry::GeometryCollectionZ:
    case IGeometry::GeometryCollectionM:
    case IGeometry::GeometryCollectionZM:
      {

      }
      break;
    case IGeometry::PolyhedralSurface:
    case IGeometry::PolyhedralSurfaceZ:
    case IGeometry::PolyhedralSurfaceZM:
    case IGeometry::PolyhedralSurfaceM:
    case IGeometry::TIN:
    case IGeometry::TINZ:
    case IGeometry::TINZM:
    case IGeometry::TINM:
      {
        IPolyhedralSurface *polySurface = dynamic_cast<IPolyhedralSurface*>(geometry);


        if(polySurface->patchCount())
        {
          //          QGraphicsItemGroup *itemGroup = new QGraphicsItemGroup();

          for(int i = 0; i < polySurface->patchCount() ; i++)
          {
            QGraphicsPolygonItem *polygon = toPolygon(polySurface->patch(i),brush,pen);
            items.append(polygon);
            ui->graphicsViewHydroCoupleVis->scene()->addItem(polygon);
          }

        }
      }
      break;
    default:
      break;
  }

  return items;
}

QGraphicsEllipseItem *HydroCoupleVis::toPoint(IPoint *point, const QBrush &brush, const QPen &pen, QGraphicsItem *parent)
{
  QGraphicsEllipseItem *pointItem = new QGraphicsEllipseItem(parent);
  pointItem->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
  pointItem->setRect(-2,-2,4.0,4.0);
  pointItem->setPen(pen);
  pointItem->setBrush(brush);
  pointItem->setPos(point->x(), - point->y());
  return pointItem;
}

QGraphicsPathItem *HydroCoupleVis::toPath(ILineString *lineString, const QBrush &brush, const QPen &pen, QGraphicsItem *parent)
{

  double minX = std::numeric_limits<double>::max();
  double minY = minX;

  for(int i = 0 ; i < lineString->pointCount() ; i++)
  {
    IPoint *p = lineString->point(i);
    minX = p->x() < minX ? p->x() : minX;
    minY = -p->y() < minY ? -p->y() : minY;
  }

  QPainterPath painterPath;

  for(int i = 0 ; i < lineString->pointCount() ; i++)
  {
    IPoint *p = lineString->point(i);
    if(i == 0)
    {
      painterPath = QPainterPath(QPointF(p->x() - minX, -p->y() - minY));
    }
    else
    {
      painterPath.lineTo(p->x() - minX, -p->y() - minY);
    }
  }

  QGraphicsPathItem *pathItem = new QGraphicsPathItem(painterPath, parent);
  pathItem->setPen(pen);
  pathItem->setBrush(brush);
  pathItem->setPos(minX,minY);

  return pathItem;
}

QGraphicsPolygonItem *HydroCoupleVis::toPolygon(IPolygon *polygon, const QBrush &brush, const QPen &pen, QGraphicsItem *parent)
{
  QPolygonF polyF;

  double minX = std::numeric_limits<double>::max();
  double minY = minX;

  ILineString *exteriorRing = polygon->exteriorRing();

  for(int i = 0 ; i <exteriorRing->pointCount() - 1; i++)
  {
    IPoint *p = exteriorRing->point(i);
    minX = p->x() < minX ? p->x() : minX;
    minY = -p->y() < minY ? -p->y() : minY;
  }


  for(int i = 0 ; i < exteriorRing->pointCount() - 1 ; i++)
  {
    IPoint *p = exteriorRing->point(i);
    polyF << QPointF(p->x() - minX , -p->y() - minY);
  }

  QGraphicsPolygonItem *polyItem = new QGraphicsPolygonItem(polyF, parent);
  polyItem->setPen(pen);
  polyItem->setBrush(brush);
  polyItem->setPos(minX,minY);

  return polyItem;
}

QBrush HydroCoupleVis::randomBrush()
{
  return QBrush (randomColor(),Qt::SolidPattern);
}

QPen HydroCoupleVis::randomPen()
{
  QPen p = QPen(randomBrush(),2.0,Qt::SolidLine , Qt::RoundCap , Qt::RoundJoin);
  p.setCosmetic(true);
  return p;
}

QColor HydroCoupleVis::randomColor()
{
  float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

  return QColor(r * 255,
                g * 255,
                b * 255);
}

void HydroCoupleVis::clearGraphics()
{

}

void HydroCoupleVis::removeOldAddNew(IModelComponent *component)
{
  ui->statusbarMain->showMessage("Adding results from component " + component->id() , 10000);
  removeModelComponent(component);
  addModelComponent(component);
}

void HydroCoupleVis::onSetCurrentTool(bool toggled)
{
  if (ui->actionDefaultSelect->isChecked())
  {
    m_currentTool = GraphicsView::Tool::Default;
  }
  else if (ui->actionZoomIn->isChecked())
  {
    m_currentTool = GraphicsView::Tool::ZoomIn;
  }
  else if (ui->actionZoomOut->isChecked())
  {
    m_currentTool = GraphicsView::Tool::ZoomOut;
  }
  else if (ui->actionPan->isChecked())
  {
    m_currentTool = GraphicsView::Tool::Pan;
  }

  ui->graphicsViewHydroCoupleVis->onCurrentToolChanged(m_currentTool);

  emit currentToolChanged(m_currentTool);
}

void HydroCoupleVis::onComponentStatusChanged(const QSharedPointer<IComponentStatusChangeEventArgs> &statusArgs)
{
  if(statusArgs->status() == IModelComponent::Finished ||
     statusArgs->status() == IModelComponent::Initialized)
  {
    removeOldAddNew(statusArgs.data()->component());
    //     QtConcurrent::run(this, &HydroCoupleVis::removeOldAddNew , statusArgs.data()->component());
  }
}

void HydroCoupleVis::onItemChanged(QStandardItem *item)
{
  QHash<QString,QVariant> udata = item->data(Qt::UserRole).toHash();

  if(udata.contains("ModelId") && udata.contains("NodeType") && udata.contains("ComponentDataItemId"))
  {
    IModelComponent *modelComponent = findModelById(udata["ModelId"].toString());

    if(modelComponent)
    {
      IComponentDataItem *cDataItem = findComponentDataItemById(udata["ComponentDataItemId"].toString() , modelComponent);

      if(cDataItem)
      {
        QList<QGraphicsItem*> gitems = m_modelGraphicsData[modelComponent][cDataItem];

        for(QGraphicsItem* gitem : gitems)
        {
          gitem->setVisible(item->checkState() == Qt::Checked);
        }
      }
    }
  }

  setChildCheckStates(item);
}

void HydroCoupleVis::onZoomExtent()
{
  if(ui->graphicsViewHydroCoupleVis->items().length())
  {
    QRectF itemsBoundingRect = ui->graphicsViewHydroCoupleVis->scene()->itemsBoundingRect();
    ui->graphicsViewHydroCoupleVis->fitInView(itemsBoundingRect, Qt::AspectRatioMode::KeepAspectRatio);
  }
}

void HydroCoupleVis::onMouseMapCoordinatesChanged(double mapX, double mapY, const QString &wellFormatedText)
{
  ui->statusbarMain->showMessage(wellFormatedText,10000);
}

void HydroCoupleVis::onPropertyChanged(const QString &propertyName)
{

}

void HydroCoupleVis::onPreferences()
{

}

void HydroCoupleVis::onAbout()
{
  QMessageBox::about(this, "HydroCoupleVis",
                     "<html>"
                     "<head>"
                     "<title>Component Information</title>"
                     "</head>"
                     "<body>"
                     "<img alt=\"icon\" src=':/HydroCoupleComposer/hydrocouplecomposer' width=\"100\" align=\"left\" /><h3 align=\"center\">HydroCoupleVis 1.0.0</h3>"
                     "<hr>"
                     "<p>Built on</p>"
                     "<p align=\"left\">Copyright 2014-2017. The HydroCouple Organization. All rights reserved.</p>"
                     "<p align=\"left\">This program and its associated libraries is provided AS IS with NO WARRANTY OF ANY KIND, "
                     "INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.</p>"
                     "<p align=\"center\"><a href=\"mailto:caleb.buahin@gmail.com?Subject=HydroCouple Composer\">caleb.buahin@gmail.com</a></p>"
                     "<p align=\"center\"><a href=\"www.hydrocouple.org\">www.hydrocouple.org</a></p>"
                     "</body>"
                     "</html>");
}

#endif // HYDROCOUPLEVIS_CPP
