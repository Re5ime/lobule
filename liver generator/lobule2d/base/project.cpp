#include <math.h>

#include <QXmlStreamReader>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QDir>

#include "project.h"

Project::Project(QString path):
    path(path) {
    for (int i = 0; i < 5; i++) {
        meshList[i] = 0;
    }

    QFileInfo info(path);
    name = info.baseName();

    QFile file(path + QDir::separator() + "info.xml");
    if ( ! file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open" << file.fileName();
        return;
    }
    xml = new QDomDocument();
    xml->setContent(&file);
    file.close();

    read();
}

void Project::read() {
    QDomNodeList children = xml->documentElement().childNodes();
    for (int i = 0; i < children.size(); i++) {
        QDomElement e = children.at(i).toElement();
        if (e.tagName() == "comment") {
            comment = e.text().trimmed();
        }
        if (e.tagName() == "lobule") {
            lobuleCount = e.attribute("count").toInt();
            hexSize = e.attribute("size").toDouble();
            areaRadius = e.attribute("radius").toInt();
        }
        if (e.tagName() == "mesh") {
            count = e.attribute("count").toInt();            
            list = e.attribute("list");
        }
        if (e.tagName() == "parameters") {
            femOrder = e.attribute("femOrder").toInt();
        }
        if (e.tagName() == "groups") {
            qDeleteAll(groups);
            groups.clear();
            QDomNodeList groupsNodes = e.childNodes();
            for (int j = 0; j < groupsNodes.size(); j++) {
                QDomElement g = groupsNodes.at(j).toElement();
                groups << readGroup(g);
            }
        }
    }
    if (areaRadius > 100)
        areaRadius = 100;
}
void Project::fillMeshlist() {
    int meshPower;
    QString mesh1, mesh2;

    for (int i = 0; i < 5; i++){
        meshPower = 6 * pow(4, i);
        mesh1 = path + QDir::separator() + "domain" + QString::number(meshPower) + "_physical_region.xml";
        mesh2 = path + QDir::separator() + "mesh" + QString::number(meshPower) + "_physical_region.xml.pvd";
        if (QFile::exists(mesh1) && QFile::exists(mesh2)) {
            meshList[i] = meshPower;
        } else {
            meshList[i] = 0;
        }
    }
}

Group *Project::readGroup(QDomElement g) {
    Group* group = new Group;
    group->index = g.attribute("index").toInt();
    group->name = g.attribute("name");
    group->color = QColor(g.attribute("color"));

    QDomNodeList coeffs = g.firstChildElement("coefficients").childNodes();
    for (int i = 0; i < coeffs.size(); i++){
        QDomElement c = coeffs.at(i).toElement();
        if (c.attribute("name") == "K1")
            group->K1 = c.attribute("value");
        if (c.attribute("name") == "K2")
            group->K2 = c.attribute("value");
        if (c.attribute("name") == "C1")
            group->C1 = c.attribute("value");
        if (c.attribute("name") == "C2")
            group->C2 = c.attribute("value");
        if (c.attribute("name") == "Alpha")
            group->Alpha = c.attribute("value");
    }

    QDomNodeList hexes = g.firstChildElement("hexes").childNodes();
    for (int i = 0; i < hexes.size(); i++){
        QDomElement h = hexes.at(i).toElement();
        group->indexes << h.attribute("index").toInt();
    }

    return group;
}

void Project::save() {    
    count = 0;
    list = "";
    for (int i = 0; i < 5; i++) {
        if (meshList[i] > 0) {
            list += QString::number(meshList[i]) + ",";
            count++;
        }
    }
    list.remove(list.size() - 1, 1);

    QDomElement root = xml->documentElement();
    xml->removeChild(root);
    root = xml->createElement("project");
    xml->appendChild(root);

    QDomElement e = xml->createElement("comment");
    QDomText text = xml->createTextNode(comment);
    e.appendChild(text);
    root.appendChild(e);

    e = xml->createElement("lobule");
    e.setAttribute("count", lobuleCount);
    e.setAttribute("size", hexSize);
    e.setAttribute("radius", areaRadius);
    root.appendChild(e);

    e = xml->createElement("mesh");
    e.setAttribute("count", count);
    e.setAttribute("list", list);
    root.appendChild(e);

    e = xml->createElement("parameters");
    e.setAttribute("femOrder", femOrder);    
    root.appendChild(e);

    e = xml->createElement("groups");
    root.appendChild(e);

    for (int i = 0; i < groups.size(); i++) {
        QDomElement groupElement = xml->createElement("group");
        saveGroup(groupElement, i);
        e.appendChild(groupElement);
    }

    QFile file(path + QDir::separator() + "info.xml");
    if ( ! file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Can't write" << file.fileName();
        return;
    }
    QByteArray data = xml->toByteArray();
    file.write(data);
    file.close();
}

void Project::saveGroup(QDomElement &g, int i) {
    Group* group = groups[i];
    g.setAttribute("index", group->index);
    g.setAttribute("name", group->name);
    g.setAttribute("color", group->color.name());

    QDomElement coeffs = xml->createElement("coefficients");
    g.appendChild(coeffs);

    QDomElement c = xml->createElement("coefficient");
    c.setAttribute("name", "K1");
    c.setAttribute("value", group->K1);
    coeffs.appendChild(c);
    c = xml->createElement("coefficient");
    c.setAttribute("name", "K2");
    c.setAttribute("value", group->K2);
    coeffs.appendChild(c);
    c = xml->createElement("coefficient");
    c.setAttribute("name", "C1");
    c.setAttribute("value", group->C1);
    coeffs.appendChild(c);
    c = xml->createElement("coefficient");
    c.setAttribute("name", "C2");
    c.setAttribute("value", group->C2);
    coeffs.appendChild(c);
    c = xml->createElement("coefficient");
    c.setAttribute("name", "Alpha");
    c.setAttribute("value", group->Alpha);
    coeffs.appendChild(c);

    QDomElement hexes = xml->createElement("hexes");
    g.appendChild(hexes);
    foreach (int i, group->indexes) {
        QDomElement h = xml->createElement("hex");
        h.setAttribute("index", i);        
        hexes.appendChild(h);
    }
}

bool Project::isGeoExist() {
    QString geo = path + QDir::separator() + "domain.geo";
    return QFile::exists(geo);
}

bool Project::isMeshExist() {    
    if (count > 0) {
        return true;
    } else {
        return false;
    }
}
