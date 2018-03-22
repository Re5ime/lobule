#ifndef GEOEDITOR_H
#define GEOEDITOR_H

#include <QtGui>
#include <QGraphicsView>
#include <QGridLayout>

class GraphicsView : public QGraphicsView {    
    Q_OBJECT
public:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
signals:
    void viewKeyPressEvent(QKeyEvent *event);
    void viewMousePressEvent(QMouseEvent *event);
    void viewMouseReleaseEvent(QMouseEvent *event);
    void viewMouseMoveEvent(QMouseEvent *event);
    void viewWheelEvent(QWheelEvent *event);
};

class Ruler : public QWidget {
    Q_OBJECT
public:
    enum Orientation {
        Horizontal,
        Vertical
    };
    Ruler(Ruler::Orientation orientation, qreal scale = 1);
public slots:
    void onMousePressEvent(QMouseEvent *event);
    void onMouseReleaseEvent(QMouseEvent *event);
    void onMouseMoveEvent(QMouseEvent *event);
    void onWheelEvent(QWheelEvent* event);
protected:
    void paintEvent(QPaintEvent *event);
private:
    qreal scale;
    QPoint offset;
    QPoint mousePos;
    Orientation orientation;
};

class GraphicsEditor : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicsEditor(qreal scal = 1);
    virtual ~GraphicsEditor();
protected slots:
    virtual void onMousePressEvent(QMouseEvent *event);
    virtual void onMouseReleaseEvent(QMouseEvent *event);
    virtual void onMouseMoveEvent(QMouseEvent *event);
    virtual void onWheelEvent(QWheelEvent *event);
protected:
    GraphicsView *view;
    QGraphicsScene *scene;
    QGridLayout *gridLayout;    
    float maxScale;
    float minScale;
private:
    QPoint lastPoint; //for scrolling
};

#endif // GEOEDITOR_H
