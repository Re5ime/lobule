#include <QDebug>
#include <QDir>
//#include <QMessageBox>
#include <QtWidgets>

#include "ui_formproject.h"
#include "dialogproject.h"
#include "dialogsetting.h"
#include "dialogclone.h"
#include "formproject.h"
#include "settings.h"

FormProject::FormProject(QWidget *parent) :
    BaseForm(parent),
    ui(new Ui::FormProject)
{
    ui->setupUi(this);

    QTimer::singleShot(0, this, SLOT(readSettings()));
}

FormProject::~FormProject()
{
    delete ui;
}

void FormProject::readSettings() {
    projectsPaths = Settings::L("projectsPaths");
    for (int i = 0; i < projectsPaths.size(); i++) {
        addProject(projectsPaths[i]);
    }
    ui->projectsTable->horizontalHeader()->restoreGeometry(Settings::A("projectsTable.g"));
    ui->projectsTable->horizontalHeader()->restoreState(Settings::A("projectsTable.s"));
    if (ui->projectsTable->rowCount() > 0) {
        ui->projectsTable->selectRow(0);
    }
}

void FormProject::saveSettings() {
    Settings::Set("projectsTable.g", ui->projectsTable->horizontalHeader()->saveGeometry());
    Settings::Set("projectsTable.s", ui->projectsTable->horizontalHeader()->saveState());
    Settings::Set("projectsPaths", projectsPaths);
}

QWidget *FormProject::tools() {
    return ui->widget;
}

void FormProject::on_toolCreate_clicked() {
    DialogProject* dialog = new DialogProject(this);
    connect(dialog, SIGNAL(create(QString,QString)), this, SLOT(createProject(QString,QString)));
    dialog->show();
}

void FormProject::createProject(QString name, QString dirPath) {
    qDebug() << name << dirPath;
    QDir dir(dirPath);
    if (dir.isReadable() && dir.mkdir(name)) {
        qDebug() << dirPath;
        QString path = dirPath + QDir::separator() + name;
        qDebug() << "dir created: " << path;
        QString infoPath = path + QDir::separator() + "info.xml";
        if (QFile::copy(":/template/info.xml", infoPath)) {
            qDebug() << "project created";
            QFile info(infoPath);
            info.setPermissions(info.permissions() | QFile::WriteOwner);
            addProject(path);
        }
    }
}

void FormProject::addProject(QString path) {
    if ( ! projectsPaths.contains(path)) {
        projectsPaths.append(path);
    }

    if (ui->projectsTable->findItems(path, Qt::MatchExactly).size() > 0)
        return;

    QFile* file = new QFile(path + QDir::separator() + "info.xml");
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this,
                              "FormProject::addProject",
                              "Couldn't open info.xml",
                              QMessageBox::Ok);
        projectsPaths.removeOne(path);
        return;
    }

    ui->projectsTable->setSortingEnabled(false);

    QFileInfo pathInfo(path);

    int row = ui->projectsTable->rowCount();
    ui->projectsTable->insertRow(row);    

    QTableWidgetItem* nameItem = new QTableWidgetItem(pathInfo.baseName());
    ui->projectsTable->setItem(row, 0, nameItem);

    QTableWidgetItem* pathItem = new QTableWidgetItem(path);
    ui->projectsTable->setItem(row, 1, pathItem);

    QTableWidgetItem* createdItem = new QTableWidgetItem(pathInfo.created().toString());
    ui->projectsTable->setItem(row, 2, createdItem);

    QTableWidgetItem* modifiedItem = new QTableWidgetItem(pathInfo.lastModified().toString());
    ui->projectsTable->setItem(row, 3, modifiedItem);

    Project* project = new Project(path);
    projects.append(project);

    QTableWidgetItem* commentItem = new QTableWidgetItem(project->comment);
    ui->projectsTable->setItem(row, 4, commentItem);

    ui->projectsTable->setSortingEnabled(true);
}

void FormProject::on_toolAdd_clicked() {
    QString lastAddDir = Settings::S("lastAddDir");

    QString dir = QFileDialog::getExistingDirectory(
                this, tr("Добавление проекта"), lastAddDir,
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        return;
    }
    Settings::Set("lastAddDir", dir);
    addProject(dir);
}

void FormProject::on_toolDelete_clicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Удаление проекта"), tr("Удалить проект ") + current()->name + "?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QString path = current()->path;
        projectsPaths.removeAll(path);
        removeDir(path);
        ui->projectsTable->removeRow(currentRow);
    }
}


bool FormProject::removeDir(const QString & dirName) {
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        foreach (QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            } else {
                result = QFile::remove(info.absoluteFilePath());
            }
            if (!result) {
                return result;
            }
            qDebug() << info.absoluteFilePath() << "was removed";
        }
        result = dir.rmdir(dirName);
        qDebug() << dirName << "dir removed";
    }
    return result;
}

bool FormProject::copyRecursively(const QString &srcFilePath, const QString &tgtFilePath) {
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        foreach (const QString &fileName, fileNames) {
            const QString newSrcFilePath
                    = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath
                    = tgtFilePath + QLatin1Char('/') + fileName;
            if (!copyRecursively(newSrcFilePath, newTgtFilePath))
                return false;
        }
    } else {
        if (!QFile::copy(srcFilePath, tgtFilePath))
            return false;
    }
    return true;
}

void FormProject::update() {
    on_projectsTable_itemSelectionChanged();
}

void FormProject::on_projectsTable_itemSelectionChanged() {
    QList<QTableWidgetItem*> selection = ui->projectsTable->selectedItems();
    if (selection.size() == 0)
        return;

    currentRow = selection[0]->row();
    Project* p = current();

    ui->commentEdit->setText(p->comment);
    ui->lobuleCountEdit->setText(QString::number(p->lobuleCount));
    ui->groupCountEdit->setText(QString::number(p->groups.size()-1));

    emit updateMainTools();
}

Project* FormProject::current() {
    QString path = ui->projectsTable->item(currentRow, 1)->text();
    foreach (Project* p, projects) {
        if (p->path == path) {
            return p;
        }
    }
    return NULL;
}

void FormProject::on_saveComment_clicked() {
    ui->projectsTable->setSortingEnabled(false);

    current()->comment = ui->commentEdit->toPlainText();
    ui->projectsTable->item(currentRow, 4)->setText(current()->comment);
    current()->save();

    ui->projectsTable->setSortingEnabled(true);
}

void FormProject::on_toolCopy_clicked() {
    DialogClone* dialog = new DialogClone(this, current());
    connect(dialog, SIGNAL(create(QString,QString)), this, SLOT(copyProject(QString,QString)));
    dialog->show();
}

void FormProject::copyProject(QString name, QString dirPath) {
    QDir dir(dirPath);
    if (dir.isReadable() && dir.mkdir(name)) {
        QString path = dirPath + QDir::separator() + name;
        qDebug() << "dir created: " << path;
        QString srcPath = current()->path;
        if (copyRecursively(srcPath, path)) {
            qDebug() << "project created";
            addProject(path);
        }
    }
}

void FormProject::on_exitButton_clicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Выход из программы"), tr("Вы действительно хотите выйти?\n\nВсе несохраненные данные будут потеряны!"),
            QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        qDebug() << "Yes was clicked";
        qApp->quit();
    } else {
        qDebug() << "Yes was *not* clicked";
    }
}

void FormProject::on_settingsButton_clicked() {
    DialogSetting* dialog = new DialogSetting(this);    
    dialog->show();
}

void FormProject::on_loadComment_clicked() {
    update();
}

void FormProject::setProject(Project *current) {

}
