#ifndef DIALOGSETTING_H
#define DIALOGSETTING_H

#include <QDialog>

namespace Ui {
class DialogSetting;
}

class DialogSetting : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetting(QWidget *parent = 0);
    ~DialogSetting();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogSetting *ui;
};

#endif // DIALOGSETTING_H
