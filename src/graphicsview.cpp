#include "stdafx.h"
#include "graphicsview.h"
#include <QWheelEvent>
#include <QGLWidget>

GraphicsView::GraphicsView(QWidget *parent)
  : QGraphicsView(parent), m_mouseIsPressed(false)
{

  QGLFormat glformat;
  glformat.setDoubleBuffer(true);
  glformat.setSampleBuffers(true);
  glformat.setDepth(true);

  QGLWidget *glWidget = new QGLWidget(glformat,this);
  glWidget->makeCurrent();
  setViewport(glWidget);

  QGraphicsScene* cgs = new QGraphicsScene(-10e9, -10e9, 2*10e9, 2*10e9, this);

  setScene(cgs);
  cgs->setBspTreeDepth(8);
  setAcceptDrops(true);
  setRenderHint(QPainter::SmoothPixmapTransform, true);
  setRenderHint(QPainter::Antialiasing, true);
//  setRenderHint(QPainter::HighQualityAntialiasing, true);
  setRenderHint(QPainter::TextAntialiasing, true);
  setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::FullViewportUpdate);
  setDragMode(QGraphicsView::DragMode::RubberBandDrag);
  setRubberBandSelectionMode(Qt::ItemSelectionMode::IntersectsItemShape);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  QPen selectionPen = QPen();
  selectionPen.setCapStyle(Qt::PenCapStyle::RoundCap);
  selectionPen.setWidthF(0.0f);
  selectionPen.setColor(QColor(0, 150, 255, 150));

  QBrush brush = QBrush(QColor(0, 150, 255, 80));
  m_zoomItem.setPen(selectionPen);
  m_zoomItem.setBrush(brush);
  m_zoomItem.setVisible(true);
  m_currentTool = Tool::Default;
  totalScaleFactor = 1.0;
  setFocusPolicy(Qt::StrongFocus);
}

GraphicsView::~GraphicsView()
{

}

void GraphicsView::onCurrentToolChanged(int currentTool)
{
  m_currentTool = (Tool)currentTool;
}

void GraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
  //  if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist") && canAcceptDrop(event->mimeData()))
  //  {
  //    event->accept();
  //  }
  //  else
  //  {
  //    event->ignore();
  //  }
  QGraphicsView::dragEnterEvent(event);
}

void GraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
  //  if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
  //  {
  //    event->accept();
  //  }
  //  else
  //  {
  //    event->ignore();
  //  }
  QGraphicsView::dragMoveEvent(event);
}

void GraphicsView::dropEvent(QDropEvent  *event)
{
  //  QMap<QString,QVariant> dropData;

  //  if (canAcceptDrop(event->mimeData(), dropData))
  //  {
  //    emit itemDropped(mapToScene(event->pos()) , dropData);
  //    event->accept();
  //  }
  //  else
  //  {
  //    event->ignore();
  //  }
  QGraphicsView::dropEvent(event);

}

void GraphicsView::wheelEvent(QWheelEvent * event)
{
  if (event->delta())
  {
    double value = event->delta()*0.05 / 120.0;

    setTransformationAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);

    scale(1 + value, 1 + value);

    setTransformationAnchor(QGraphicsView::ViewportAnchor::NoAnchor);

    event->accept();
  }
}

void GraphicsView::mousePressEvent(QMouseEvent * event)
{
  m_mousePressPosition = mapToScene(event->pos());
  m_mouseIsPressed = true;

  switch (m_currentTool)
  {
    case GraphicsView::ZoomIn:
    case GraphicsView::ZoomOut:
      {
        m_zoomItem.setRect(QRectF(0, 0, 0, 0));
        m_zoomItem.setPos(m_mousePressPosition);
        scene()->addItem(&m_zoomItem);
      }
      break;
    case GraphicsView::Pan:
      break;
    default:
      {
        if (event->button() != Qt::LeftButton)
        {
          event->accept();
        }
        else
        {
          QGraphicsView::mousePressEvent(event);
        }
      }
  }
}

void GraphicsView::mouseReleaseEvent(QMouseEvent * event)
{
  QPointF currentPos = mapToScene(event->pos());

  if (event->buttons().testFlag(Qt::MouseButton::MiddleButton))
  {
    QGraphicsView::mouseReleaseEvent(event);
  }
  else
  {
    switch (m_currentTool)
    {
      case GraphicsView::ZoomIn:
        {
          QRectF p = getRectFrom(m_mousePressPosition, currentPos);
          m_zoomItem.setRect(QRectF(0, 0, p.width(), p.height()));
          m_zoomItem.setPos(p.topLeft());

          if (m_zoomItem.rect().width() > 0 || m_zoomItem.rect().height() > 0)
            fitInView(&m_zoomItem, Qt::AspectRatioMode::KeepAspectRatio);
          scene()->removeItem(&m_zoomItem);
        }
        break;
      case GraphicsView::ZoomOut:
        {
          QRectF p = getRectFrom(m_mousePressPosition, currentPos);
          m_zoomItem.setRect(QRectF(0, 0, p.width(), p.height()));
          m_zoomItem.setPos(p.topLeft());
          if (m_zoomItem.rect().width() > 0 || m_zoomItem.rect().height() > 0)
          {
            centerOn(&m_zoomItem);
            scale(0.8, 0.8);
            setTransformationAnchor(QGraphicsView::ViewportAnchor::NoAnchor);
          }
          scene()->removeItem(&m_zoomItem);
        }
        break;
      default:
        QGraphicsView::mouseReleaseEvent(event);
    }
  }

  m_mouseIsPressed = false;
}

void GraphicsView::mouseMoveEvent(QMouseEvent * event)
{
  QPoint ePos = event->pos();
  QPointF currentPos = mapToScene(ePos);

  if (m_mouseIsPressed)
  {
    if (event->buttons().testFlag(Qt::MouseButton::MiddleButton))
    {
      QPointF t = currentPos - m_previousMousePosition;
      translate(t.x(), t.y());
    }
    else
    {
      switch (m_currentTool)
      {
        case GraphicsView::ZoomIn:
        case GraphicsView::ZoomOut:
          {
            QRectF p = getRectFrom(m_mousePressPosition, currentPos);
            m_zoomItem.setPos(p.topLeft());
            m_zoomItem.setRect(QRectF(0, 0, p.width(), p.height()));
          }
          break;
        case GraphicsView::Pan:
          {
            QPointF t = currentPos - m_previousMousePosition;
            translate(t.x(), t.y());
          }
          break;
        default:
          {
            QGraphicsView::mouseMoveEvent(event);
          }
      }
    }
  }
  else
  {
    QGraphicsView::mouseMoveEvent(event);
  }


  m_previousMousePosition = mapToScene(ePos);

  QString status = "Screen Coordinates : " + QString::number(ePos.x()) + " ," + QString::number(ePos.y()) +
                   " | Map Coordinates : " + QString::number(currentPos.x(),'g',10) + " ," + QString::number(-currentPos.y(), 'g',10);

  emit mouseMapCoordinatesChanged(currentPos.x() , -currentPos.y(), status);
}

bool GraphicsView::viewportEvent(QEvent *event)
{
  switch (event->type())
  {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
      {
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();

        if (touchPoints.count() == 2)
        {
          //          // determine scale factor
          //          const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
          //          const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();

          //          qreal currentScaleFactor = QLineF(touchPoint0.pos(), touchPoint1.pos()).length()
          //                                     / QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();

          //          if (touchEvent->touchPointStates() & Qt::TouchPointReleased)
          //          {
          //            // if one of the fingers is released, remember the current scale
          //            // factor so that adding another finger later will continue zooming
          //            // by adding new scale factor to the existing remembered value.
          //            totalScaleFactor *= currentScaleFactor;
          //            currentScaleFactor = 1.0;
          //          }

          //          setTransformationAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);
          //          setTransform(QTransform().scale(totalScaleFactor*currentScaleFactor, totalScaleFactor*currentScaleFactor));
          //          setTransformationAnchor(QGraphicsView::ViewportAnchor::NoAnchor);
        }
        return true;
      }
    default:
      break;
  }
  return QGraphicsView::viewportEvent(event);
}

QRectF GraphicsView::getRectFrom(const QPointF& p1, const QPointF p2)
{
  double x1 = 0, x2 = 0, y1 = 0, y2=0;

  if (p1.x() < p2.x())
  {
    x1 = p1.x(); x2 = p2.x();
  }
  else
  {
    x1 = p2.x(); x2 = p1.x();
  }

  if (p1.y() < p2.y())
  {
    y1 = p1.y(); y2 = p2.y();
  }
  else
  {
    y1 = p2.y(); x2 = p1.y();
  }

  QRectF t = QRectF(x1, y1, x2 - x1, y2 - y1);
  return t;
}
