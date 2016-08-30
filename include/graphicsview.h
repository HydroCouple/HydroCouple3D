#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGLWidget>
#include <QGraphicsView>
#include <QGraphicsRectItem>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

  public:

    enum Tool
    {
      Default,
      ZoomIn,
      ZoomOut,
      Pan,
    };

    GraphicsView(QWidget *parent);

    virtual ~GraphicsView();

  public slots:

    void onCurrentToolChanged(int currentTool);

  protected:

    void dragEnterEvent(QDragEnterEvent * event) override;

    void dragMoveEvent(QDragMoveEvent * event) override;

    void dropEvent(QDropEvent * event) override;

    void wheelEvent(QWheelEvent * event) override;

    void mousePressEvent(QMouseEvent * event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;

    void mouseMoveEvent(QMouseEvent * event) override;

    bool viewportEvent(QEvent *event) override;

  private:

    QRectF getRectFrom(const QPointF &p1, const QPointF p2);

  signals:

    void mouseMapCoordinatesChanged(double mapX , double mapY, const QString &status);

  private:
    QPointF m_previousMousePosition;
    QPointF m_mousePressPosition;
    bool m_mouseIsPressed;
    Tool m_currentTool;
    QRectF m_zoomRect;
    QGraphicsRectItem m_zoomItem;
    qreal totalScaleFactor;
};

#endif // GRAPHICSVIEW_H
