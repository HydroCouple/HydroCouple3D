/*!
 * \file hydrocouplevis.h
 * \author Caleb Amoa Buahin <caleb.buahin@gmail.com>
 * \version 1.0.0
 * \description
 * \license
 * This file and its associated files, and libraries are free software.
 * You can redistribute it and/or modify it under the terms of the
 * Lesser GNU General Public License as published by the Free Software Foundation;
 * either version 3 of the License, or (at your option) any later version.
 * This file and its associated files is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.(see <http://www.gnu.org/licenses/> for details)
 * \copyright Copyright 2014-2018, Caleb Buahin, All rights reserved.
 * \date 2014-2018
 * \pre
 * \bug
 * \warning
 * \todo properly abstract models and the visualization items.
 * \todo create complex graphics argument items like meshes polygons etc.
 */

#ifndef HYDROCOUPLEVIS_H
#define HYDROCOUPLEVIS_H

#include "stdafx.h"
#include <QtWidgets/QMainWindow>
#include <QSettings>
#include "hydrocouplespatiotemporal.h"
#include "graphicsview.h"

#include <QHash>
#include <QSharedPointer>

#ifdef HYDROCOUPLEVIS_LIBRARY
# define HYDROCOUPLEVIS_EXPORT Q_DECL_EXPORT
#else
# define HYDROCOUPLEVIS_EXPORT Q_DECL_IMPORT
#endif

namespace Ui{
  class HydroCoupleVisClass;
}

class QProgressBar;
class QStandardItemModel;
class QStandardItem;

class HYDROCOUPLEVIS_EXPORT HydroCoupleVis : public QMainWindow
{
    Q_OBJECT

  public:

    HydroCoupleVis(QWidget* parent = 0);

    ~HydroCoupleVis();

    /*!
     * \brief getInstance used to generate GUI fot testing purposes
     * \return
     */
    static HydroCoupleVis * getInstance() ;

    //visualize geometric arguments and outputs
    void addModelComponent(HydroCouple::IModelComponent *modelComponent);

    bool removeModelComponent(HydroCouple::IModelComponent *modelComponent);

  protected:

    void closeEvent(QCloseEvent * event) override;

//  private:
    //temp public must be private;
  public:

    void initializeSignalSlotConnections();

    void initializeGUIElements();

    void initializeTreeViewModel();

    void resetTreeView();

    void addToTreeView(HydroCouple::IModelComponent *modelComponent);

    void removeFromTreeView(HydroCouple::IModelComponent *modelComponent);

    void readSettings();

    void writeSettings();

    HydroCouple::IModelComponent *findModelById(const QString &id);

    HydroCouple::IComponentDataItem *findComponentDataItemById(const QString &id, HydroCouple::IModelComponent *model);

    void setChildCheckStates(QStandardItem *parent);


    QList<QGraphicsItem*> addGeometryDataItem(HydroCouple::Spatial::IGeometryComponentDataItem *geometryData);

    QList<QGraphicsItem*> addGeometryDataItem(HydroCouple::SpatioTemporal::ITimeGeometryComponentDataItem *geometryData);

    QList<QGraphicsItem*> addGeometryDataItem(HydroCouple::Spatial::IPolyhedralSurfaceComponentDataItem *geometryData);

    QList<QGraphicsItem*> addGeometry(HydroCouple::Spatial::IGeometry *geometry,
                                      const QBrush &brush = randomBrush(),
                                      const QPen &pen = randomPen());

    QGraphicsEllipseItem *toPoint(HydroCouple::Spatial::IPoint *point,
                                  const QBrush &brush,
                                  const QPen &pen, QGraphicsItem *parent =nullptr);

    QGraphicsPathItem* toPath(HydroCouple::Spatial::ILineString *lineString,
                              const QBrush &brush,
                              const QPen &pen, QGraphicsItem *parent = nullptr);

    QGraphicsPolygonItem *toPolygon(HydroCouple::Spatial::IPolygon *polygon,
                                    const QBrush &brush,
                                    const QPen &pen, QGraphicsItem *parent = nullptr);

    static QBrush randomBrush();

    static QPen randomPen();

    static QColor randomColor();

    void clearGraphics();

    void removeOldAddNew(HydroCouple::IModelComponent *component);

  signals:

    void currentToolChanged(int currentTool);

  private slots:

    void onSetCurrentTool(bool toggled);

    void onZoomExtent();

    void onMouseMapCoordinatesChanged(double mapX, double mapY, const QString &wellFormatedText);

    void onItemChanged(QStandardItem *item);

    void onComponentStatusChanged(const QSharedPointer<HydroCouple::IComponentStatusChangeEventArgs> &status);

    void onPropertyChanged(const QString& propertyName);

    void onPreferences();

    void onAbout();

  private:

    QProgressBar *m_progressBar;
    QStandardItemModel *m_tableOfContentTreeViewModel;
    GraphicsView::Tool m_currentTool;
    QStringList m_recentFiles;
    QSettings m_settings;
    QMap<HydroCouple::IModelComponent*, QMap<HydroCouple::IComponentDataItem*,QList<QGraphicsItem*>>> m_modelGraphicsData;

  public:
    Ui::HydroCoupleVisClass *ui;
};


#endif // HYDROCOUPLEVIS_H
