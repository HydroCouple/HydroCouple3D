#ifndef HYDROCOUPLEVIS_CPP
#define HYDROCOUPLEVIS_CPP

#include "stdafx.h"
#include "hydrocouplevis.h"
#include "ui_hydrocouplevis.h"

#include <QMessageBox>
#include <QProgressBar>
#include <QStandardItemModel>
#include <iostream>

using namespace HydroCouple;
using namespace HydroCouple::Spatial;

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
      else if(dynamic_cast<IPolyhedralSurfaceComponentDataItem*>(output))
      {
        m_modelGraphicsData[modelComponent][output] = QList<QGraphicsItem*>();
        QList<QGraphicsItem*> items = addGeometryDataItem(dynamic_cast<IPolyhedralSurfaceComponentDataItem*>(output));
        QList<QGraphicsItem*> & t = m_modelGraphicsData[modelComponent][output];

        if(items.length())
          t.append(items);
      }
    }


    for(IArgument* output : modelComponent->arguments())
    {

      if(dynamic_cast<IGeometryComponentDataItem*>(output))
      {
        m_modelGraphicsData[modelComponent][output] = QList<QGraphicsItem*>();
        QList<QGraphicsItem*> & t = m_modelGraphicsData[modelComponent][output];
        QList<QGraphicsItem*> items = addGeometryDataItem(dynamic_cast<IGeometryComponentDataItem*>(output));

        if(items.length())
          t.append(items);
      }
      else if(dynamic_cast<IPolyhedralSurfaceComponentDataItem*>(output))
      {
        m_modelGraphicsData[modelComponent][output] = QList<QGraphicsItem*>();
        QList<QGraphicsItem*> & t = m_modelGraphicsData[modelComponent][output];
        QList<QGraphicsItem*> items = addGeometryDataItem(dynamic_cast<IPolyhedralSurfaceComponentDataItem*>(output));
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
    QMap<IComponentDataItem*,QList<QGraphicsItem*>> citems = m_modelGraphicsData[modelComponent];

    for( QMap<IComponentDataItem*,QList<QGraphicsItem*> >::iterator it = citems.begin();
         it != citems.end() ; it++)
    {
      for(QGraphicsItem* item : it.value())
      {
        ui->graphicsViewHydroCoupleVis->scene()->removeItem(item);
        delete item;
      }
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


  connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(onNewProject()));
  connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenFiles()));
  connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(onSave()));
  connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(onSaveAs()));
  connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(onPrint()));

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

  actionPreferences->setText("Options");
  menuTools->addAction(actionPreferences);
  menuHelp->addAction(actionAbout);

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
    QStandardItem *modelItem = new QStandardItem(QIcon(), modelComponent->caption());
    modelItem->setCheckable(true);
    modelItem->setUserTristate(false);
    QHash<QString,QVariant> data;
    data["ModelId"] = modelComponent->id();
    data["NodeType"] = "Model";
    modelItem->setData(data,Qt::UserRole);

    m_tableOfContentTreeViewModel->invisibleRootItem()->appendRow(modelItem);

    QStandardItem *argumentHeaders = new QStandardItem(QIcon(), "Arguments");
    argumentHeaders->setCheckable(true);
    argumentHeaders->setUserTristate(false);
    QHash<QString,QVariant> dataArgs;
    dataArgs["ModelId"] = modelComponent->id();
    dataArgs["NodeType"] = "ArgumentsHeader";
    argumentHeaders->setData(dataArgs,Qt::UserRole);
    modelItem->appendRow(argumentHeaders);


    QStandardItem *argumentOutputs = new QStandardItem(QIcon(), "Outputs");
    argumentOutputs->setCheckable(true);
    argumentOutputs->setUserTristate(false);
    QHash<QString,QVariant> outputArgs;
    outputArgs["ModelId"] = modelComponent->id();
    outputArgs["NodeType"] = "OutputsHeader";
    argumentOutputs->setData(outputArgs,Qt::UserRole);
    modelItem->appendRow(argumentOutputs);

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

        argumentOutputs->appendRow(outputItem);
      }
      else
      {
        QStandardItem *argumentItem = new QStandardItem(QIcon(), cdataItem->caption());
        argumentItem->setCheckable(true);
        argumentItem->setUserTristate(false);
        QHash<QString,QVariant> dataArgs;
        dataArgs["ModelId"] = modelComponent->id();
        dataArgs["NodeType"] = "Argument";
        dataArgs["ComponentDataItemId"] = cdataItem->id();
        argumentItem->setData(dataArgs,Qt::UserRole);

        argumentHeaders->appendRow(argumentItem);
      }
    }

    ui->treeViewModelComponents->expandToDepth(1);
    modelItem->setCheckState(Qt::Checked);
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

  if(geometryData->geometryType() == HydroCouple::Spatial::LineString ||
     geometryData->geometryType() == HydroCouple::Spatial::LineStringZ ||
     geometryData->geometryType() == HydroCouple::Spatial::LineStringZM ||
     geometryData->geometryType() == HydroCouple::Spatial::LineStringM
     )
  {
    brush.setColor(Qt::transparent);
  }
  else if (geometryData->geometryType() == HydroCouple::Spatial::Polygon ||
           geometryData->geometryType() == HydroCouple::Spatial::PolygonZ ||
           geometryData->geometryType() == HydroCouple::Spatial::PolygonZM ||
           geometryData->geometryType() == HydroCouple::Spatial::PolygonM
           )
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
    case GeometryType::Point:
    case GeometryType::PointZ:
    case GeometryType::PointZM:
    case GeometryType::PointM:
      {
        IPoint *point = dynamic_cast<IPoint*>(geometry);
        QGraphicsItem* item = toPoint(point,brush,pen);
        items.append(item);
        ui->graphicsViewHydroCoupleVis->scene()->addItem(item);
      }
      break;
    case GeometryType::LineString:
    case GeometryType::LineStringZ:
    case GeometryType::LineStringM:
    case GeometryType::LineStringZM:
      {
        ILineString *lineString = dynamic_cast<ILineString*>(geometry);
        QGraphicsItem* item = toPath(lineString,brush,pen);
        items.append(item);
        ui->graphicsViewHydroCoupleVis->scene()->addItem(item);
      }
      break;
    case GeometryType::Polygon:
    case GeometryType::PolygonZ:
    case GeometryType::PolygonM:
    case GeometryType::PolygonZM:
      {
        IPolygon *polygon = dynamic_cast<IPolygon*>(geometry);
        QGraphicsItem* item = toPolygon(polygon,brush,pen);
        items.append(item);
        ui->graphicsViewHydroCoupleVis->scene()->addItem(item);
      }
      break;
    case GeometryType::GeometryCollection:
    case GeometryType::GeometryCollectionZ:
    case GeometryType::GeometryCollectionM:
    case GeometryType::GeometryCollectionZM:
      {

      }
      break;
    case GeometryType::PolyhedralSurface:
    case GeometryType::PolyhedralSurfaceZ:
    case GeometryType::PolyhedralSurfaceZM:
    case GeometryType::PolyhedralSurfaceM:
    case GeometryType::TIN:
    case GeometryType::TINZ:
    case GeometryType::TINZM:
    case GeometryType::TINM:
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
  QPainterPath painterPath;

  double minX = std::numeric_limits<double>::max();
  double minY = minX;

  for(int i = 0 ; i < lineString->pointCount() ; i++)
  {
    IPoint *p = lineString->point(i);
    minX = p->x() < minX ? p->x() : minX;
    minY = -p->y() < minY ? -p->y() : minY;
  }


  for(int i = 0 ; i < lineString->pointCount() ; i++)
  {
    IPoint *p = lineString->point(i);
    painterPath.lineTo(p->x() - minX, -p->y() - minY);
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

  for(int i = 0 ; i <exteriorRing->pointCount() ; i++)
  {
    IPoint *p = exteriorRing->point(i);
    minX = p->x() < minX ? p->x() : minX;
    minY = -p->y() < minY ? -p->y() : minY;
  }


  for(int i = 0 ; i < exteriorRing->pointCount() ; i++)
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
  QPen p = QPen(randomBrush(),1.5,Qt::SolidLine , Qt::RoundCap , Qt::RoundJoin);
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
  if(statusArgs.data()->status() == HydroCouple::Finished)
  {
    removeModelComponent(statusArgs.data()->component());
    addModelComponent(statusArgs.data()->component());
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
