#include "geoeditor.h"
#include <QDebug>
#include <QScrollBar>

GraphicsEditor::GraphicsEditor(qreal scal) {
    view = new GraphicsView;
    scene = new QGraphicsScene;
    view->setScene(scene);
    view->setMouseTracking(true);

    connect(view, SIGNAL(viewMouseMoveEvent(QMouseEvent*)), this, SLOT(onMouseMoveEvent(QMouseEvent*)));
    connect(view, SIGNAL(viewMousePressEvent(QMouseEvent*)), this, SLOT(onMousePressEvent(QMouseEvent*)));
    connect(view, SIGNAL(viewMouseReleaseEvent(QMouseEvent*)), this, SLOT(onMouseReleaseEvent(QMouseEvent*)));
    connect(view, SIGNAL(viewWheelEvent(QWheelEvent*)), this, SLOT(onWheelEvent(QWheelEvent*)));

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setMargin(0);
    gridLayout->addWidget(view, 1, 1);

    maxScale = 10;
    minScale = 0.5;
    view->scale(scal, scal);
}

GraphicsEditor::~GraphicsEditor() {

}

void GraphicsEditor::onMousePressEvent(QMouseEvent *event) {
     //start scrolling
    if (event->button() == Qt::MidButton) {
        lastPoint = event->pos();
        view->setDragMode(QGraphicsView::ScrollHandDrag);
    }
}

void GraphicsEditor::onMouseReleaseEvent(QMouseEvent *event) {
    //stop scrolling
    if (event->button() == Qt::MidButton) {
        view->setDragMode(QGraphicsView::NoDrag);
    }
}

void GraphicsEditor::onMouseMoveEvent(QMouseEvent *event) {
    //scrolling
    if (view->dragMode() == QGraphicsView::ScrollHandDrag) {
        QPoint p = -(event->pos() - lastPoint);
        lastPoint = event->pos();
        view->horizontalScrollBar()->setValue( view->horizontalScrollBar()->value() + p.x() );
        view->verticalScrollBar()->setValue( view->verticalScrollBar()->value() + p.y() );
    }
}

void GraphicsEditor::onWheelEvent(QWheelEvent *event) {
    double scaleFactor = 1.15;
    if(event->delta() > 0) {
        if (view->transform().m11() < maxScale)
            view->scale(scaleFactor, scaleFactor);
    } else {
        if (view->transform().m11() > minScale)
            view->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }    
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
    emit viewMousePressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    emit viewMouseReleaseEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event) {
    emit viewMouseMoveEvent(event);
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
    emit viewWheelEvent(event);
}

void GraphicsView::keyPressEvent(QKeyEvent *event) {
    emit viewKeyPressEvent(event);
}

Ruler::Ruler(Orientation orientation, qreal scale) {
    this->scale = scale;
    this->offset = QPoint(0,0);
    this->orientation = orientation;
    setMinimumSize(30, 30);
}

void Ruler::onMousePressEvent(QMouseEvent *event) {
}

void Ruler::onMouseReleaseEvent(QMouseEvent *event) {
}

void Ruler::onMouseMoveEvent(QMouseEvent *event) {
    GraphicsView* view = dynamic_cast<GraphicsView*>(sender());
    if (view==NULL) {
        qDebug() << "Ruler error: sender view is null";
        return;
    }
    offset = view->mapFromScene(0, 0);
    mousePos = event->pos();
    update();
}

void Ruler::onWheelEvent(QWheelEvent *event) {
    GraphicsView* view = dynamic_cast<GraphicsView*>(sender());
    if (view==NULL) {
        qDebug() << "Ruler error: sender view is null";
        return;
    }
    this->scale = view->transform().m11();
    offset = view->mapFromScene(0, 0);
    update();
}

void Ruler::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setPen(Qt::black);
    float j = 0;
    const float minj = 20;
    int dm = 10;
    if (scale >= 8) {
        dm = 1;
    } else if (scale >= 4) {
        dm = 2;
    } else if (scale >= 2) {
        dm = 5;
    }
    painter.setFont(QFont("Arial", 8));
    if (orientation == Horizontal) {
        for(int i=0; i*scale+offset.x()<this->width(); i+=dm) {
            if (j > minj || i == 0){
                painter.drawText(QRectF(i*scale + offset.x()-minj, 0, minj*2, 20), Qt::AlignCenter, QString::number(i));
                j = 0;
                painter.drawLine(i*scale + offset.x(), 21, i*scale + offset.x(), 30);
            } else {
                painter.drawLine(i*scale + offset.x(), 21, i*scale + offset.x(), 25);
                j += dm*scale;
            }
        }
        j = 0;
        for(int i=-dm; i*scale+offset.x()>=0; i-=dm) {
            if (j > minj){
                painter.drawText(QRectF(i*scale + offset.x()-minj, 0, minj*2, 20), Qt::AlignCenter, QString::number(i));
                j = 0;
                painter.drawLine(i*scale + offset.x(), 21, i*scale + offset.x(), 30);
            } else {
                painter.drawLine(i*scale + offset.x(), 21, i*scale + offset.x(), 25);
                j += dm*scale;
            }
        }
        painter.setPen(Qt::red);
        painter.drawLine(mousePos.x(), 0, mousePos.x(), 30);
    } else {
        for(int i=0; i*scale + offset.y()<this->height(); i+=dm) {
            if (j > minj || i == 0){
                painter.drawText(QRectF(0, i*scale + offset.y()-minj, 20, 2*minj), Qt::AlignCenter, QString::number(-i));
                j = 0;
                painter.drawLine(21, i*scale + offset.y(), 30, i*scale + offset.y());
            } else {
                painter.drawLine(21, i*scale + offset.y(), 25, i*scale + offset.y());
                j += dm*scale;
            }
        }
        j = 0;
        for(int i=-dm; i*scale+offset.y()>=0; i-=dm) {
            if (j > minj){
                painter.drawText(QRectF(0, i*scale + offset.y()-minj, 20, 2*minj), Qt::AlignCenter, QString::number(-i));
                j = 0;
                painter.drawLine(21, i*scale + offset.y(), 30, i*scale + offset.y());
            } else {
                painter.drawLine(21, i*scale + offset.y(), 25, i*scale + offset.y());
                j += dm*scale;
            }
        }
        painter.setPen(Qt::red);
        painter.drawLine(0, mousePos.y(), 30, mousePos.y());
    }
}
