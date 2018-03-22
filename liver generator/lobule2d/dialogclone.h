#ifndef DIALOGCLONE_H
#define DIALOGCLONE_H

#include <QDialog>
#include "project.h"

namespace Ui {
class DialogClone;
}

class DialogClone : public QDialog
{
    Q_OBJECT

public:
    explicit DialogClone(QWidget *parent, Project *source);
    ~DialogClone();    

private slots:
    void on_browseButton_clicked();
    void on_buttonBox_accepted();

signals:
    void create(QString name, QString dir);

private:
    Ui::DialogClone *ui;
};

#endif // DIALOGCLONE_H
