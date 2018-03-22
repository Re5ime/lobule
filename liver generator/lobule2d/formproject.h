#ifndef FORMPROJECT_H
#define FORMPROJECT_H

//#include <Qt/QtGui>
#include <QtGui>

#include "baseform.h"
#include "project.h"

namespace Ui {
class FormProject;
}

class FormProject : public BaseForm
{
    Q_OBJECT
public:
    explicit FormProject(QWidget *parent = 0);
    ~FormProject();
    QWidget* tools();
    void setProject(Project* current);
    Project* current();

public slots:
    void saveSettings();
    void update();

private slots:
    void readSettings();
    void createProject(QString name, QString dirPath);
    void copyProject(QString name, QString dirPath);    

    void on_toolCreate_clicked();
    void on_toolAdd_clicked();
    void on_toolCopy_clicked();
    void on_toolDelete_clicked();

    void on_projectsTable_itemSelectionChanged();
    void on_saveComment_clicked();
    void on_settingsButton_clicked();
    void on_exitButton_clicked();

    void on_loadComment_clicked();

private:
    void addProject(QString path);
    bool removeDir(const QString &dirName);
    bool copyRecursively(const QString &srcFilePath, const QString &tgtFilePath);

    Ui::FormProject *ui;
    QStringList projectsPaths;
    QList<Project*> projects;
    int currentRow;
};

#endif // FORMPROJECT_H
