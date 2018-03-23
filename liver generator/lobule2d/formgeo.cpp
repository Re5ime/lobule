#include <math.h>

#include <QAbstractItemView>
#include <QColorDialog>
#include <QDomDocument>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QScrollBar>

#include "ui_formgeo.h"
#include "formgeo.h"

using namespace std;

FormGeo::FormGeo(QWidget *parent) :
    BaseForm(parent),
    ui(new Ui::FormGeo) {

    view = new GraphicsView;
    scene = new QGraphicsScene;
    view->setScene(scene);
    view->setMouseTracking(true);

    connect(view, SIGNAL(viewMouseMoveEvent(QMouseEvent*)), this, SLOT(onMouseMoveEvent(QMouseEvent*)));
    connect(view, SIGNAL(viewMousePressEvent(QMouseEvent*)), this, SLOT(onMousePressEvent(QMouseEvent*)));
    connect(view, SIGNAL(viewMouseReleaseEvent(QMouseEvent*)), this, SLOT(onMouseReleaseEvent(QMouseEvent*)));
    connect(view, SIGNAL(viewWheelEvent(QWheelEvent*)), this, SLOT(onMouseWheelEvent(QWheelEvent*)));

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setMargin(0);
    gridLayout->addWidget(view, 1, 1);

    maxScale = 10;
    minScale = 0.5;    

    ui->setupUi(this);
    ui->viewLayout->addLayout(gridLayout);

    view->setStyleSheet("QGraphicsView {border-radius: 0px; border: 1px solid #282828; selection-background-color: #5F5F5F; }");
    view->scale(1, 1);

    //maxScale = 100;

    ui->groupsTable->setColumnWidth(1, 40);
    QHeaderView *headerView = ui->groupsTable->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);
    headerView->setSectionResizeMode(1, QHeaderView::Interactive);

    ui->hexSizeEdit->setValidator(new QDoubleValidator(0.01, 9999, 2, this));

    ui->K1Edit->setValidator(new QDoubleValidator(0.0, 9999, 15, this));
    ui->K2Edit->setValidator(new QDoubleValidator(0.0, 9999, 15, this));
    ui->C1Edit->setValidator(new QDoubleValidator(0.0, 9999, 15, this));
    ui->C2Edit->setValidator(new QDoubleValidator(0.0, 9999, 15, this));
    ui->AlphaEdit->setValidator(new QDoubleValidator(0.0, 9999, 15, this));
    ui->coefficientsBox->setEnabled(false);    
}

FormGeo::~FormGeo() {
    delete ui;
}

QWidget *FormGeo::tools() {
    return ui->widget;
}

QString FormGeo::createGmshHex(double x, double y, int i) {
    QString s = "";
    s = s + "Point(" + QString::number(i) + ")={" + QString::number(x) + "," + QString::number(y + p->hexSize / sqrt(3.0)) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 1) + ")={" + QString::number(x + p->hexSize / 2) + "," + QString::number(y + p->hexSize / sqrt(3.0) / 2.0) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 2) + ")={" + QString::number(x + p->hexSize / 2) + "," + QString::number(y - p->hexSize / sqrt(3.0) / 2.0) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 3) + ")={" + QString::number(x) + "," + QString::number(y - p->hexSize / sqrt(3.0)) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 4) + ")={" + QString::number(x - p->hexSize / 2) + "," + QString::number(y - p->hexSize / sqrt(3.0) / 2.0) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 5) + ")={" + QString::number(x - p->hexSize / 2) + "," + QString::number(y + p->hexSize / sqrt(3.0) / 2.0) + ",0,p1};\n";
    s = s + "Line(" + QString::number(i) + ")={" + QString::number(i) + "," + QString::number(i + 1) + "};\n";
    s = s + "Line(" + QString::number(i + 1) + ")={" + QString::number(i + 1) + "," + QString::number(i + 2) + "};\n";
    s = s + "Line(" + QString::number(i + 2) + ")={" + QString::number(i + 2) + "," + QString::number(i + 3) + "};\n";
    s = s + "Line(" + QString::number(i + 3) + ")={" + QString::number(i + 3) + "," + QString::number(i + 4) + "};\n";
    s = s + "Line(" + QString::number(i + 4) + ")={" + QString::number(i + 4) + "," + QString::number(i + 5) + "};\n";
    s = s + "Line(" + QString::number(i + 5) + ")={" + QString::number(i + 5) + "," + QString::number(i) + "};\n";
    s = s + "Line Loop(" + QString::number(i + 6) + ")={" + QString::number(i) + "," + QString::number(i + 1) + "," + QString::number(i + 2) + "," + QString::number(i + 3) + "," + QString::number(i + 4) + "," + QString::number(i + 5) + "};\n";
    s = s + "Plane Surface(" + QString::number(i + 7) + ")={" + QString::number(i + 6) + "};\n";
    return s;
}

QString FormGeo::createGmshHexCircle(double x, double y, int i) {
    QString s = "";
    s = s + "Point(" + QString::number(i) + ")={" + QString::number(x) + "," + QString::number(y + p->hexSize / sqrt(3.0)) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 1) + ")={" + QString::number(x + p->hexSize / 2) + "," + QString::number(y + p->hexSize / sqrt(3.0) / 2.0) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 2) + ")={" + QString::number(x + p->hexSize / 2) + "," + QString::number(y - p->hexSize / sqrt(3.0) / 2.0) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 3) + ")={" + QString::number(x) + "," + QString::number(y - p->hexSize / sqrt(3.0)) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 4) + ")={" + QString::number(x - p->hexSize / 2) + "," + QString::number(y - p->hexSize / sqrt(3.0) / 2.0) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 5) + ")={" + QString::number(x - p->hexSize / 2) + "," + QString::number(y + p->hexSize / sqrt(3.0) / 2.0) + ",0,p1};\n";

    //center circle
    s = s + "Point(" + QString::number(i + 6) + ")={" + QString::number(x) + "," + QString::number(y) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 7) + ")={" + QString::number(x + R1) + "," + QString::number(y) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 8) + ")={" + QString::number(x) + "," + QString::number(y + R1) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 9) + ")={" + QString::number(x - R1) + "," + QString::number(y) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 10) + ")={" + QString::number(x) + "," + QString::number(y - R1) + ",0,p1};\n";

    //other circles
    s = s + "Point(" + QString::number(i + 11) + ")={" + QString::number(x - R2 * sqrt(3.0) / 2.0) + "," + QString::number(y + p->hexSize / sqrt(3.0) - R2 / 2.0) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 12) + ")={" + QString::number(x + R2 * sqrt(3.0) / 2.0) + "," + QString::number(y + p->hexSize / sqrt(3.0) - R2 / 2.0) + ",0,p1};\n";

    s = s + "Point(" + QString::number(i + 13) + ")={" + QString::number(x + p->hexSize / 2 - R2 * sqrt(3.0) / 2.0) + "," + QString::number(y + p->hexSize / sqrt(3.0) / 2.0 + R2 / 2.0) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 14) + ")={" + QString::number(x + p->hexSize / 2) + "," + QString::number(y + p->hexSize / sqrt(3.0) / 2.0 - R2) + ",0,p1};\n";

    s = s + "Point(" + QString::number(i + 15) + ")={" + QString::number(x + p->hexSize / 2) + "," + QString::number(y - p->hexSize / sqrt(3.0) / 2.0 + R2) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 16) + ")={" + QString::number(x + p->hexSize / 2 - R2 * sqrt(3.0) / 2.0) + "," + QString::number(y - p->hexSize / sqrt(3.0) / 2.0 - R2 / 2.0) + ",0,p1};\n";

    s = s + "Point(" + QString::number(i + 17) + ")={" + QString::number(x + R2 * sqrt(3.0) / 2.0) + "," + QString::number(y - p->hexSize / sqrt(3.0) + R2 / 2.0) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 18) + ")={" + QString::number(x - R2 * sqrt(3.0) / 2.0) + "," + QString::number(y - p->hexSize / sqrt(3.0) + R2 / 2.0) + ",0,p1};\n";

    s = s + "Point(" + QString::number(i + 19) + ")={" + QString::number(x - p->hexSize / 2 + R2 * sqrt(3.0) / 2.0) + "," + QString::number(y - p->hexSize / sqrt(3.0) / 2.0 - R2 / 2.0) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 20) + ")={" + QString::number(x - p->hexSize / 2) + "," + QString::number(y - p->hexSize / sqrt(3.0) / 2.0 + R2) + ",0,p1};\n";

    s = s + "Point(" + QString::number(i + 21) + ")={" + QString::number(x - p->hexSize / 2) + "," + QString::number(y + p->hexSize / sqrt(3.0) / 2.0 - R2) + ",0,p1};\n";
    s = s + "Point(" + QString::number(i + 22) + ")={" + QString::number(x - p->hexSize / 2 + R2 * sqrt(3.0) / 2.0) + "," + QString::number(y + p->hexSize / sqrt(3.0) / 2.0 + R2 / 2.0) + ",0,p1};\n";

    //lobule edges
    s = s + "Line(" + QString::number(i + 23) + ")={" + QString::number(i + 12) + "," + QString::number(i + 13) + "};\n";
    s = s + "Line(" + QString::number(i + 24) + ")={" + QString::number(i + 14) + "," + QString::number(i + 15) + "};\n";
    s = s + "Line(" + QString::number(i + 25) + ")={" + QString::number(i + 16) + "," + QString::number(i + 17) + "};\n";
    s = s + "Line(" + QString::number(i + 26) + ")={" + QString::number(i + 18) + "," + QString::number(i + 19) + "};\n";
    s = s + "Line(" + QString::number(i + 27) + ")={" + QString::number(i + 20) + "," + QString::number(i + 21) + "};\n";
    s = s + "Line(" + QString::number(i + 28) + ")={" + QString::number(i + 22) + "," + QString::number(i + 11) + "};\n";

    //circles
    s = s + "Circle(" + QString::number(i + 29) + ")={" + QString::number(i + 7) + "," + QString::number(i + 6) + "," + QString::number(i + 8) + "};\n";
    s = s + "Circle(" + QString::number(i + 30) + ")={" + QString::number(i + 8) + "," + QString::number(i + 6) + "," + QString::number(i + 9) + "};\n";
    s = s + "Circle(" + QString::number(i + 31) + ")={" + QString::number(i + 9) + "," + QString::number(i + 6) + "," + QString::number(i + 10) + "};\n";
    s = s + "Circle(" + QString::number(i + 32) + ")={" + QString::number(i + 10) + "," + QString::number(i + 6) + "," + QString::number(i + 7) + "};\n";

    s = s + "Circle(" + QString::number(i + 33) + ")={" + QString::number(i + 11) + "," + QString::number(i) + "," + QString::number(i + 12) + "};\n";
    s = s + "Circle(" + QString::number(i + 34) + ")={" + QString::number(i + 13) + "," + QString::number(i + 1) + "," + QString::number(i + 14) + "};\n";
    s = s + "Circle(" + QString::number(i + 35) + ")={" + QString::number(i + 15) + "," + QString::number(i + 2) + "," + QString::number(i + 16) + "};\n";
    s = s + "Circle(" + QString::number(i + 36) + ")={" + QString::number(i + 17) + "," + QString::number(i + 3) + "," + QString::number(i + 18) + "};\n";
    s = s + "Circle(" + QString::number(i + 37) + ")={" + QString::number(i + 19) + "," + QString::number(i + 4) + "," + QString::number(i + 20) + "};\n";
    s = s + "Circle(" + QString::number(i + 38) + ")={" + QString::number(i + 21) + "," + QString::number(i + 5) + "," + QString::number(i + 22) + "};\n";

    s = s + "Line Loop(" + QString::number(i + 39) + ")={" + QString::number(i + 29) + "," + QString::number(i + 30) + "," + QString::number(i + 31) + "," + QString::number(i + 32) + "};\n";
    s = s + "Line Loop(" + QString::number(i + 40) + ")={" + QString::number(i + 33) + "," + QString::number(i + 23) + "," + QString::number(i + 34) + "," + QString::number(i + 24) + "," + QString::number(i + 35) + "," + QString::number(i + 25) + "," + QString::number(i + 36) + "," + QString::number(i + 26) + "," + QString::number(i + 37) + "," + QString::number(i + 27) + "," + QString::number(i + 38) + "," + QString::number(i + 28) + "};\n";

    s = s + "Plane Surface(" + QString::number(i + 41) + ")={" + QString::number(i + 40) + "," + QString::number(i + 39) + "};\n";
    return s;
}

void FormGeo::exportGeometry() {
    R1 = p->hexSize * 0.10;
    R2 = 2.0 * R1 / 3.0;

    QString fileName = p->path + QDir::separator() + "domain.geo";
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Can't write file"), tr("File can't be opened for writing") + fileName);
        return;
    } else {
        QTextStream out(&file);
        out << "p1 = " << p->hexSize / 4.0 << ";" << endl;
        int k = 1;
        double ratio = p->hexSize / 50.0;
        for (int i = 0; i < hexes.size(); i++) {
            if (hexes[i]->group->index > 0) {
                QString s = createGmshHexCircle(ratio * hexes[i]->x, -ratio * hexes[i]->y, k);
                s += "\n";
                out << s;
                k += 42;
            }
        }
        out << "Coherence;" << endl;

        for (int i = 1; i < p->groups.size(); i++) {
            QString s1 = "";
            foreach (int hexIndex, p->groups[i]->indexes) {
                s1 += QString::number(42 * hexIndex + 42) + ",";
            }
            s1.resize(s1.size() - 1);
            out << "Physical Surface(" << i << ")={" << s1 << "};\n" << endl;
        }

        qDebug() << "Done importing:" << file.fileName();
        out.flush();
        file.close();

    }
}

void FormGeo::exportCoefficients() {
    QString fileName = p->path + QDir::separator() + "coefficients.xml";
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Can't write file"), tr("File can't be opened for writing") + fileName);
        return;
    } else {
        QTextStream out(&file);

        out << "<?xml version=\"1.0\"?>" << endl;
        out << "<dolfin xmlns:dolfin=\"http://fenicsproject.org\">" << endl;
        out << "  <parameters name=\"coefficients\">" << endl;
        //K1
        out << "    <parameters name=\"K1\">" << endl;
        for (int i = 1; i < p->groups.size(); i++) {
            out << "      <parameter key=\"" << i << "\" type=\"double\" value=\"";
            out << p->groups[i]->K1 << "\" />" << endl;
        }
        out << "    </parameters>" << endl;
        //K2
        out << "    <parameters name=\"K2\">" << endl;
        for (int i = 1; i < p->groups.size(); i++) {
            out << "      <parameter key=\"" << i << "\" type=\"double\" value=\"";
            out << p->groups[i]->K2 << "\" />" << endl;
        }
        out << "    </parameters>" << endl;
        //C1
        out << "    <parameters name=\"C1\">" << endl;
        for (int i = 1; i < p->groups.size(); i++) {
            out << "      <parameter key=\"" << i << "\" type=\"double\" value=\"";
            out << p->groups[i]->C1 << "\" />" << endl;
        }
        out << "    </parameters>" << endl;
        //C2
        out << "    <parameters name=\"C2\">" << endl;
        for (int i = 1; i < p->groups.size(); i++) {
            out << "      <parameter key=\"" << i << "\" type=\"double\" value=\"";
            out << p->groups[i]->C2 << "\" />" << endl;
        }
        out << "    </parameters>" << endl;
        //Alpha
        out << "    <parameters name=\"Alpha\">" << endl;
        for (int i = 1; i < p->groups.size(); i++) {
            out << "      <parameter key=\"" << i << "\" type=\"double\" value=\"";
            out << p->groups[i]->Alpha << "\" />" << endl;
        }
        out << "    </parameters>" << endl;
        out << "  </parameters>" << endl;
        out << "</dolfin>";
        out.flush();
        qDebug() << "Done writing" << file.fileName();
        file.close();
    }
}

void FormGeo::changeArea(int previousRadius, int radius, Group *group) {
    double x = 0, y = 0;    

    if (hexes.size() == 0) {
        addHex(x, y, group, true);
    }

    double msize = 50;
    for (int r = previousRadius + 1; r <= radius; r++) {
        x = msize * r;
        y = 0;
        for (int i = 1; i <= r; i++) {
            x -= msize / 2.0;
            y += msize * sqrt(3.0) / 2.0;
            addHex(x, y, group);
        }
        for (int i = 1; i <= r; i++) {
            x -= msize;
            addHex(x, y, group);
        }
        for (int i = 1; i <= r; i++) {
            x -= msize / 2.0;
            y -= msize * sqrt(3.0) / 2.0;
            addHex(x, y, group);
        }
        for (int i = 1; i <= r; i++) {
            x += msize / 2.0;
            y -= msize * sqrt(3.0) / 2.0;
            addHex(x, y, group);
        }
        for (int i = 1; i <= r; i++) {
            x += msize;
            addHex(x, y, group);
        }
        for (int i = 1; i <= r; i++) {
            x += msize / 2.0;
            y += msize * sqrt(3.0) / 2.0;
            addHex(x, y, group,true);
        }
    }

    for (int r = previousRadius; r > radius; r--) {
        for (int i = 0; i < 6 * r; i++) {
            Hex* hex = hexes.last();
            hexes.removeOne(hex);
            hex->group->indexes.removeOne(hex->index);
            delete hex;
        }
    }
}

Hex *FormGeo::addHex(double x, double y, Group *group, bool onesixth) {
    Hex* hex = new Hex(hexes.size(), x, y);
    hex->isOnesixth = onesixth;
    hexes << hex;    
    if (group != NULL) {
        hex->group = group;
        group->indexes << hex->index;        
    }
    return hex;
}

void FormGeo::draw()
{
    scene->clear();
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    foreach (const Group *group, p->groups) {
        int alpha = 255;
        if (current() != NULL && group != current()) {
            alpha = 157;
        }
        foreach (int i, group->indexes) {
            if (isPart && ! hexes[i]->isOnesixth){
                alpha = 30;
            }
            QColor gcolor = group->color;
            gcolor.setAlpha(alpha);
            brush.setColor(gcolor);
            hexes[i]->draw(scene, brush);
            hexes[i]->drawCircle(scene, brush);
        }
    }
    foreach (const Hex *hex, hexes) {
        if (hex->isSelected){
            QColor gcolor = hex->group->color;
            if (isPart && ! hex->isOnesixth){
                gcolor.setAlpha(30);
            }
            brush.setColor(gcolor);
            hex->draw(scene, brush);
            hex->drawCircle(scene, brush);
        }
    }
}

void FormGeo::onMouseReleaseEvent(QMouseEvent *event) {
    //stop scrolling
    if (event->button() == Qt::MidButton) {
        view->setDragMode(QGraphicsView::NoDrag);
    }
}

void FormGeo::onMouseMoveEvent(QMouseEvent *event) {
    //scrolling
    if (view->dragMode() == QGraphicsView::ScrollHandDrag) {
        QPoint p = -(event->pos() - lastPoint);
        qDebug() << p.x() << p.y();
        lastPoint = event->pos();
        view->horizontalScrollBar()->setValue( view->horizontalScrollBar()->value() + p.x() );
        view->verticalScrollBar()->setValue( view->verticalScrollBar()->value() + p.y() );
    }
}

void FormGeo::onMouseWheelEvent(QWheelEvent *event) {
    double scaleFactor = 1.15;
    if(event->delta() > 0) {
        if (view->transform().m11() < maxScale)
            view->scale(scaleFactor, scaleFactor);
    } else {
        if (view->transform().m11() > minScale)
            view->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
   }
}

void FormGeo::onMousePressEvent(QMouseEvent *event) {
    if (current() != NULL && current()->index == 0)
        return;

    Hex* hex;
    if (QGraphicsItem *item = view->itemAt(event->pos())) {

        QVariant var = item->data(0);
        hex = (Hex*)var.value<void*>();
        if ((!hex->isOnesixth)&&(isPart)){
            return;
        }
    }
    else {
        return;
    }

    if (event->button() == Qt::LeftButton) {
        if (current() == NULL) {
            for (int i = 0; i < hexes.size(); i++) {
                hexes[i]->isSelected = false;
            }
        }
        hex->isSelected = true;

        if (current() != NULL) {
            saveGroup();
            if (isPart) {
                symmetrize();
            }
        }
    }

    if (event->button() == Qt::RightButton) {
        if (hex->group == current() || current() == NULL) {
            hex->isSelected = false;

            hex->group->indexes.removeOne(hex->index);
            p->groups[0]->indexes << hex->index;
            hex->group = p->groups[0];
            if (isPart) {
                symmetrize();
            }
        }
    }

    //start scrolling
    if (event->button() == Qt::MidButton) {
        lastPoint = event->pos();
        view->setDragMode(QGraphicsView::ScrollHandDrag);
    }

    //GraphicsEditor::onMousePressEvent(event);
    draw();
}

void Hex::draw(QGraphicsScene *scene, QBrush brush) const {
    double r = 50;
    double a[6], b[6];
    a[0] = x;
    b[0] = y + r / sqrt(3.0);
    a[1] = x + r / 2;
    b[1] = y + r / sqrt(3.0) / 2.0;
    a[2] = x + r / 2;
    b[2] = y - r / sqrt(3.0) / 2.0;
    a[3] = x;
    b[3] = y - r / sqrt(3.0);
    a[4] = x - r / 2;
    b[4] = y - r / sqrt(3.0) / 2.0;
    a[5] = x - r / 2;
    b[5] = y + r / sqrt(3.0) / 2.0;

    QPolygonF polygon;
    for(int i = 0; i < 6; i++){
        polygon << QPointF(a[i], b[i]);
    }
    QPen pen;
    if (isSelected){
        pen.setWidthF(2);
        pen.setColor(Qt::black);
    }
    else {
        pen.setWidthF(1);
    }

    QGraphicsPolygonItem *item = scene->addPolygon(polygon, pen, brush);
    item->setData(0, qVariantFromValue((void*)this));
}

void Hex::drawCircle(QGraphicsScene *scene, QBrush brush) const {
    double r1 = 20;
    QRectF rect(x - r1 / 2.0, y - r1 / 2.0, r1, r1);

    QPen pen;
    pen.setWidthF(1);

    //gcolor.setAlpha(alpha);
    brush.setColor(QColor(100, 100, 100));

    QGraphicsEllipseItem *item = scene->addEllipse(rect, pen, brush);
    item->setData(0, qVariantFromValue((void*)this));
}

void FormGeo::on_deleteButton_clicked() {
    Group* group = current();

    if (group != NULL && group->index == 0)
        return;

    foreach (int i, group->indexes) {
        group->indexes.removeOne(i);
        p->groups[0]->indexes << i;
        hexes[i]->group = p->groups[0];
    }

    ui->groupsTable->removeRow(group->index);
    ui->groupsTable->clearSelection();

    p->groups.removeOne(group);
    for (int i = group->index; i < p->groups.size(); i++){
        ui->groupsTable->cellWidget(i, 1)->setProperty("index", i);
        p->groups[i]->index = i;
    }

    ui->coefficientsBox->setEnabled(false);
    ui->deleteButton->setEnabled(false);

    draw();
}

void FormGeo::on_loadButton_clicked() {
    p->read();
    setProject(p);
    draw();
}

void FormGeo::addGroup(Group *g) {       
    ui->groupsTable->insertRow(g->index);    
    ui->groupsTable->setItem(g->index, 0, new QTableWidgetItem(g->name));
    QPushButton *groupColor = new QPushButton();
    groupColor->setProperty("index", g->index);
    connect(groupColor, SIGNAL(clicked()), this, SLOT(changeColor()));
    groupColor->setStyleSheet(QString("background-color: %1").arg(g->color.name()));
    ui->groupsTable->setCellWidget(g->index, 1, groupColor);
    if (g->index == 0)
        ui->groupsTable->hideRow(0);
}

void FormGeo::changeColor() {    
    QPushButton *groupColor = (QPushButton*)sender();
    int index = groupColor->property("index").toInt();
    ui->groupsTable->selectRow(index);
    Group* group = p->groups[index];
    QColor color = QColorDialog::getColor(group->color, this, tr("Выберите цвет группы"));
    if (color.isValid()) {
        group->color = color;
        groupColor->setStyleSheet(QString("background-color: %1").arg(color.name()));
        draw();
    }
}

void FormGeo::on_addButton_clicked() {
    Group *group = new Group();
    group->index = p->groups.size();
    group->name = tr("Тип ") + QString::number(group->index);
    group->color = QColor(qrand() % 256, qrand() % 256, qrand() % 256);

    group->K1 = ui->K1Edit->text();
    group->K2 = ui->K2Edit->text();
    group->C1 = ui->C1Edit->text();
    group->C2 = ui->C2Edit->text();
    group->Alpha = ui->AlphaEdit->text();

    p->groups << group;
    addGroup(group);
    ui->groupsTable->selectRow(group->index);
}

void FormGeo::saveGroup() {
    Group *group = current();

    if (group != NULL && group->index != 0) {
        for(int i = 0; i < hexes.size(); i++) {
            if (hexes[i]->isSelected) {
                if (hexes[i]->group != group) {
                    hexes[i]->group->indexes.removeOne(i);
                    hexes[i]->group = group;
                    group->indexes << i;
                }
            }
        }
    }
}

void FormGeo::on_K1Edit_textChanged(const QString &K1) {
    if (current() != NULL)
        current()->K1 = K1;
}

void FormGeo::on_K2Edit_textChanged(const QString &K2) {
    if (current() != NULL)
        current()->K2 = K2;
}

void FormGeo::on_C1Edit_textChanged(const QString &C1) {
    if (current() != NULL)
        current()->C1 = C1;
}

void FormGeo::on_C2Edit_textChanged(const QString &C2) {
    if (current() != NULL)
        current()->C2 = C2;
}

void FormGeo::on_AlphaEdit_textChanged(const QString &Alpha) {
    if (current() != NULL)
        current()->Alpha = Alpha;
}

void FormGeo::on_viewButton_clicked() {
    ui->coefficientsBox->setEnabled(false);
    ui->deleteButton->setEnabled(false);

    for (int i = 0; i < hexes.size(); i++){
        hexes[i]->isSelected = false;
//        hexes[i]->isSelectedCircle = false;
    }

    ui->groupsTable->clearSelection();
    draw();
}

void FormGeo::on_saveButton_clicked() {
    exportGeometry();
    exportCoefficients();
//    deleteFiles();
    p->lobuleCount = hexes.size() - p->groups[0]->indexes.size();
    p->femOrder = 0;
    p->save();

    emit updateMainTools();
}

//void FormGeo::deleteFiles() {
//    QString fileName;
//    if (p->isMeshExist()){
//        for (int i = 0; i < 5; i++) {
//            if (p->meshList[i] > 0) {
//                QString ms = QString::number(p->meshList[i]);
//                fileName = p->path + QDir::separator() + "domain" + ms + ".msh";
//                QFile::remove(fileName);
//                fileName = p->path + QDir::separator() + "domain" + ms + ".xml";
//                QFile::remove(fileName);
//                fileName = p->path + QDir::separator() + "domain" + ms + "_physical_region.xml";
//                QFile::remove(fileName);
//                fileName = p->path + QDir::separator() + "mesh" + ms + ".msh";
//                QFile::remove(fileName);
//                fileName = p->path + QDir::separator() + "mesh" + ms + ".xml";
//                QFile::remove(fileName);
//                fileName = p->path + QDir::separator() + "mesh" + ms + "_physical_region.xml";
//                QFile::remove(fileName);
//                fileName = p->path + QDir::separator() + "mesh" + ms + "_physical_region.xml.pvd";
//                QFile::remove(fileName);
//                fileName = p->path + QDir::separator() + "mesh" + ms + "_physical_region.xml000000.vtu";
//                QFile::remove(fileName);
//            }
//        }
//    }

//    for (int i = 0; i < 5; i++) {
//        p->meshList[i] = 0;
//    }
//    p->save();
//}

Group *FormGeo::current() {
    int row = ui->groupsTable->currentRow();
    QTableWidgetItem *item = ui->groupsTable->item(row, 0);
    if (! ui->groupsTable->isItemSelected(item))
        return NULL;

    return p->groups[row];
}

void FormGeo::on_areaRadiusEdit_valueChanged(int radius) {
    if (current() != NULL) {
        changeArea(p->areaRadius, radius, current());
    } else {
        if (p->groups.size() > 0) {
            changeArea(p->areaRadius, radius, p->groups[0]);
        } else {
            changeArea(p->areaRadius, radius, NULL);
        }
    }
    p->areaRadius = radius;
    on_groupsTable_itemSelectionChanged();
    draw();
}

void FormGeo::on_hexSizeEdit_textChanged(const QString &hexSizeText) {
    p->hexSize = hexSizeText.toDouble();
}

void FormGeo::on_groupsTable_itemSelectionChanged() {
    for (int i = 0; i < hexes.size(); i++){
        hexes[i]->isSelected = false;
    }

    Group *group = current();

    if (group == NULL) {
        ui->coefficientsBox->setEnabled(false);
        ui->deleteButton->setEnabled(false);
        return;
    }

    if (group != NULL && group->index == 0) {
        ui->deleteButton->setEnabled(false);
        return;
    }

    ui->K1Edit->setText(group->K1);
    ui->K2Edit->setText(group->K2);
    ui->C1Edit->setText(group->C1);
    ui->C2Edit->setText(group->C2);
    ui->AlphaEdit->setText(group->Alpha);

    ui->coefficientsBox->setEnabled(true);
    ui->deleteButton->setEnabled(true);

    for(int i = 0; i < group->indexes.size(); i++) {
        if (isPart) {
            if (hexes[group->indexes[i]]->isOnesixth)
                hexes[group->indexes[i]]->isSelected = true;
        } else {
            hexes[group->indexes[i]]->isSelected = true;
        }
    }

    draw();
}

void FormGeo::on_groupsTable_itemChanged(QTableWidgetItem *item) {
    if (current() != NULL && item->column() == 0) {
        current()->name = item->text();
    }
}

void FormGeo::setProject(Project *current) {
    ui->groupsTable->clear();
    ui->groupsTable->setRowCount(0);
    qDeleteAll(hexes);
    hexes.clear();
    p = current;    

    if (p->groups.size() == 2 && p->groups[0]->indexes.size() == 0 && p->groups[1]->indexes.size() == 0) {
        changeArea(0, p->areaRadius-1, p->groups[1]);
        changeArea(p->areaRadius-1, p->areaRadius, p->groups[0]);
    } else {
        changeArea(0, p->areaRadius, NULL);
        foreach (Group *g, p->groups) {            
            foreach (int i, g->indexes) {                
                hexes[i]->group = g;
            }
        }
    }

    foreach (Group* group, p->groups) {        
        addGroup(group);
    }

    ui->areaRadiusEdit->setValue(p->areaRadius);
    ui->hexSizeEdit->setText(QString::number(p->hexSize));
    qreal sc = 5.0 / (p->areaRadius + 0.5);
    view->resetTransform();
    view->scale(sc, sc);
    ui->fullViewRadio->setChecked(true);
    isPart = false;
    draw();
}

void FormGeo::on_partViewRadio_clicked() {
    isPart = true;
    symmetrize();
    on_groupsTable_itemSelectionChanged();
    draw();
}

void FormGeo::symmetrize() {
    for (int r = 1; r <= p->areaRadius; r++) {
        for (int i = 1; i < 6; i++) {
            for (int j = 0; j < r; j++) {
                int s = 3*r*(r+1) - j;
                int ind = 3*r*(r+1) - r*i - j;
                hexes[ind]->group->indexes.removeOne(ind);
                hexes[ind]->group = hexes[s]->group;
                hexes[s]->group->indexes << ind;
            }
        }
    }
}

void FormGeo::on_fullViewRadio_clicked() {
    isPart = false;
    on_groupsTable_itemSelectionChanged();
    draw();
}
