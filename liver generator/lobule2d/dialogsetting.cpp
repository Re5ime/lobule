#include "dialogsetting.h"
#include "ui_dialogsetting.h"
#include "mainwindow.h"
#include "settings.h"

DialogSetting::DialogSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetting)
{
    ui->setupUi(this);    
    //ui->tvsExist->setChecked(Settings::B("tvsExist"));
}

DialogSetting::~DialogSetting()
{
    delete ui;
}

void DialogSetting::on_buttonBox_accepted()
{
    //Settings::Set("tvsExist", ui->tvsExist->isChecked());
}
