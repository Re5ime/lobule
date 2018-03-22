#include "dialogproject.h"
#include "ui_dialogproject.h"
#include <QFileDialog>
#include "settings.h"

DialogProject::DialogProject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProject)
{
    ui->setupUi(this);
    ui->nameEdit->setText(Settings::S("lastName", ui->nameEdit->text()));
    ui->dirEdit->setText(Settings::S("lastDir", ui->dirEdit->text()));
}

DialogProject::~DialogProject()
{
    delete ui;
}

void DialogProject::on_browseButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(
                this, tr("Выбрать директорию"), ui->dirEdit->text(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if ( ! dir.isEmpty()) {
        ui->dirEdit->setText(dir);
    }
}

void DialogProject::on_buttonBox_accepted()
{
    Settings::Set("lastName", ui->nameEdit->text());
    Settings::Set("lastDir", ui->dirEdit->text());
    emit create(ui->nameEdit->text(), ui->dirEdit->text());
}
