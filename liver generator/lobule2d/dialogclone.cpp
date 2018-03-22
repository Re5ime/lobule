#include "dialogclone.h"
#include "ui_dialogclone.h"
#include <QFileDialog>
#include <QDebug>

DialogClone::DialogClone(QWidget *parent, Project *source) :
    QDialog(parent),
    ui(new Ui::DialogClone)
{
    ui->setupUi(this);
    ui->sourceEdit->setText(source->path);
    ui->nameEdit->setText(source->name + "_clone");
    QString dir = source->path;
    dir.remove(QDir::separator() + source->name);
    ui->dirEdit->setText(dir);
}

DialogClone::~DialogClone()
{
    delete ui;
}

void DialogClone::on_browseButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(
                this, tr("Выбрать директорию"), ui->dirEdit->text(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if ( ! dir.isEmpty()) {
        ui->dirEdit->setText(dir);
    }
}

void DialogClone::on_buttonBox_accepted()
{
    emit create(ui->nameEdit->text(), ui->dirEdit->text());
}
