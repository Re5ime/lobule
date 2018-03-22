#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include "baseform.h"
#include "formproject.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
signals:
    void close();

private slots:
    void toolClicked();
    void updateTools();

    void showMessage(QString message);
    void showProgress(int progressValue);

private:
    Ui::MainWindow *ui;
    QList<QToolButton*> tabs;

    FormProject *projects;
    QList<BaseForm*> forms;

    QLabel* messageLabel;    
    QProgressBar* progress;
};

#endif // MAINWINDOW_H
