#include "ui_mainwindow.h"
#include "mainwindow.h"

#include "formproject.h"
#include "formgeo.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tabs << ui->toolProject;
    tabs << ui->toolGeo;

    foreach (QToolButton* tab, tabs) {
        connect(tab, SIGNAL(clicked()), this, SLOT(toolClicked()));
    }

    projects = new FormProject();
    forms << projects;
    forms << new FormGeo();

    foreach (BaseForm* form, forms) {
        connect(form, SIGNAL(showMessage(QString)), this, SLOT(showMessage(QString)));
        connect(form, SIGNAL(showProgess(int)), this, SLOT(showProgress(int)));
        connect(form, SIGNAL(updateMainTools()), this, SLOT(updateTools()));

        ui->forms->addWidget(form);
        ui->tools->addWidget(form->tools());
    }

    connect(this, SIGNAL(close()), projects, SLOT(saveSettings()));

    for (int i = 0; i < ui->tools->count(); i++) {
        ui->tools->widget(i)->setMinimumHeight(50);
        ui->tools->widget(i)->setMaximumHeight(50);
    }

    messageLabel = new QLabel();
    statusBar()->addPermanentWidget(messageLabel, 0);
    progress = new QProgressBar();
    statusBar()->addPermanentWidget(progress, 0);
    statusBar()->showMessage(tr("Проект не выбран"));

    restoreGeometry(Settings::A("mainwindow.g", saveGeometry()));
    restoreState(Settings::A("mainwindow.s", saveState()));

}

MainWindow::~MainWindow(){
    emit close();
    delete ui;
}

void MainWindow::updateTools() {    
    Project* project = projects->current();    
    statusBar()->showMessage(tr("Проект: ") + project->name);    
}

// window setting
void MainWindow::closeEvent(QCloseEvent *event) {
    Settings::Set("mainwindow.g", saveGeometry());
    Settings::Set("mainwindow.s", saveState());
    emit close();
    QMainWindow::closeEvent(event);
}


void MainWindow::toolClicked() {
    updateTools();

    QToolButton *tab = (QToolButton*)sender();
    int index = tabs.indexOf(tab);
    forms[index]->setProject(projects->current());
    ui->forms->setCurrentIndex(index);
    ui->tools->setCurrentIndex(index);
    if (index == 0) {
        projects->update();
    }
}

void MainWindow::showMessage(QString message) {
    messageLabel->setText(message);
}

void MainWindow::showProgress(int progressValue) {
    if (progressValue == 0) {
        progress->reset();
    } else {
        progress->setValue(progressValue);
    }
}
