#ifndef DIALOGPROJECT_H
#define DIALOGPROJECT_H

#include <QDialog>

namespace Ui {
class DialogProject;
}

class DialogProject : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProject(QWidget *parent = 0);
    ~DialogProject();

private slots:
    void on_browseButton_clicked();
    void on_buttonBox_accepted();

signals:
    void create(QString name, QString dir);

private:
    Ui::DialogProject *ui;
};

#endif // DIALOGCLIENT_H
