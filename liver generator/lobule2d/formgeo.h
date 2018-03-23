#ifndef FORMGEO_H
#define FORMGEO_H

#include "geoeditor.h"
#include "baseform.h"
#include "project.h"

#include <QTableWidget>

namespace Ui {
    class FormGeo;
}

class Group;

class Hex {
public:
    Hex(int index, double x, double y) {
        this->index = index;
        this->x = x;
        this->y = y;
        this->isSelected = false;
    }

    void draw(QGraphicsScene *scene, QBrush brush = QBrush()) const;
    void drawCircle(QGraphicsScene *scene, QBrush brush = QBrush()) const;

    int index;
    double x;
    double y;
    bool isSelected;
    bool isOnesixth;
    Group* group;
};

class FormGeo : public BaseForm {
    Q_OBJECT
public:
    explicit FormGeo(QWidget *parent = 0);
    ~FormGeo();

    QWidget* tools();
    void setProject(Project* current);

protected slots:
    void onMousePressEvent(QMouseEvent *event);
    void onMouseWheelEvent(QWheelEvent *event);
    void onMouseMoveEvent(QMouseEvent *event);
    void onMouseReleaseEvent(QMouseEvent *event);

private slots:
    void on_saveButton_clicked();
    void on_loadButton_clicked();

    void on_areaRadiusEdit_valueChanged(int radius);
    void on_hexSizeEdit_textChanged(const QString &hexSizeText);

    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_viewButton_clicked();

    void on_groupsTable_itemSelectionChanged();
    void on_groupsTable_itemChanged(QTableWidgetItem *item);
    void changeColor();

    void on_K1Edit_textChanged(const QString &K1);
    void on_K2Edit_textChanged(const QString &K2);
    void on_C1Edit_textChanged(const QString &C1);
    void on_C2Edit_textChanged(const QString &C2);
    void on_AlphaEdit_textChanged(const QString &Alpha);

    void on_partViewRadio_clicked();
    void on_fullViewRadio_clicked();

private:
    QString createGmshHex(double x, double y, int i);
    QString createGmshHexCircle(double x, double y, int i);
    void exportGeometry();
    void exportCoefficients();
    void deleteFiles();

    void draw();
    void symmetrize();
    void changeArea(int previousRadius, int radius, Group* group);

    void saveGroup();
    void addGroup(Group* group);

    Project* p;
    Ui::FormGeo *ui;

    Hex* addHex(double x, double y, Group *group, bool onesixth = false);
    QList<Hex*> hexes;
    Group *current();

    GraphicsView *view;
    QGraphicsScene *scene;
    QGridLayout *gridLayout;        
    QPoint lastPoint; //for scrolling

    float maxScale;
    float minScale;
    float R1, R2;
    bool isPart = false;

};

#endif // FORMGEO_H
