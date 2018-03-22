#ifndef BASEFORM_H
#define BASEFORM_H

#include <QWidget>
#include <QDebug>
#include <QProcess>
#include "settings.h"
#include "project.h"

#include <QListWidget>

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define ENV "x64"
#else
#define ENV "x32"
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENV "x64"
#else
#define ENV "x32"
#endif
#endif

class BaseForm : public QWidget {
    Q_OBJECT

public:
    BaseForm(QWidget *parent = 0);
    virtual ~BaseForm();
    virtual QWidget* tools() = 0;
    virtual void setProject(Project* project) = 0;

    void meshList(QListWidget *listWidget, Project *p);

signals:
    void showMessage(QString message);
    void showProgess(int progress);
    void evalFinished(int code);
    void updateMainTools();

private slots:
    void performEval();
    void cancelEval();

protected:
    //Project* p;
    void showError(QString error);
    void eval(QString cmd, QString title, QString dir, const char *slot);
    void eval(QStringList cmds, QString title, QString dir, const char *slot, bool ingoreErros = false);

private:
    QProcess* process;    
    int progress, index;
};

#endif // BASEFORM_H
