#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QDomDocument>
#include <QColor>

class Group {
public:
    int index;
    QString name;
    QColor color;

    QList<int> indexes;

    QString K1;
    QString K2;
    QString C1;
    QString C2;
    QString Alpha;
};


class Project
{
public:
    Project(QString path);

    QString path;
    QString name;
    QString comment;

    int lobuleCount;
    double hexSize;
    int areaRadius;

    QList<Group*> groups;

    int femOrder;

    void save();
    void read();

private:
    QDomDocument *xml;

    Group* readGroup(QDomElement g);
    void saveGroup(QDomElement &g, int i);    
};

#endif // PROJECT_H
