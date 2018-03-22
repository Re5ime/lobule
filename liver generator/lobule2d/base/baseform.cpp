#include "baseform.h"
#include <QMessageBox>
#include <QMetaMethod>

BaseForm::BaseForm(QWidget *parent) :
    QWidget(parent) {

}

BaseForm::~BaseForm() {
}

void BaseForm::showError(QString error) {
    QMessageBox::critical(this, tr("Ошибка"), error, QMessageBox::Ok);
}

void BaseForm::eval(QString cmd, QString title, QString dir, const char *slot) {
    progress = 0;
    index = 0;
    emit showMessage(title);

    process = new QProcess(this);
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(performEval()));

    connect(process, SIGNAL(finished(int)), this, SLOT(cancelEval()));
    connect(process, SIGNAL(finished(int)), this, slot);
    qDebug() <<  cmd << "eval";
    process->setWorkingDirectory(dir);
    process->start(cmd);
}

void BaseForm::eval(QStringList cmds, QString title, QString dir, const char *slot, bool ingoreErros) {
    emit showMessage(title);

    for (int i = 0; i < cmds.size(); i++) {
        qDebug() << "eval " << cmds[i];
        emit showProgess((i + 1) * 100 / cmds.size());
        process = new QProcess(this);
        process->setWorkingDirectory(dir);
        process->start(cmds[i]);
        process->waitForFinished(60000*10);
        if (process->exitCode() != 0) {
            qDebug() << "eval error" <<  QString(process->readAll()) << process->exitCode();
            if (!ingoreErros)
                break;
        }
        process->deleteLater();
    }

    emit showMessage("");
    emit showProgess(0);

    connect(this, SIGNAL(evalFinished(int)), this, slot);
    emit evalFinished(process->exitCode());
    disconnect(this, SIGNAL(evalFinished(int)), this, slot);
}

void BaseForm::performEval() {
    if (process->readAllStandardOutput().length() == 66 ) {
        if (index < 100) {
            index += 2;
            emit showProgess(index);
        }
    } else {
        if (progress < 100) {
        progress += 25;
        emit showProgess(progress);
        //qDebug() << process->readAllStandardOutput();
        }
    }
}

void BaseForm::cancelEval() {
    process->terminate();
    process->deleteLater();
    emit showMessage("");
    emit showProgess(0);
}

void BaseForm::meshList(QListWidget *listWidget, Project *p) {
    listWidget->clear();
    for (int i = 0; i < 5; i++) {
        if (p->meshList[i] > 0) {
            QListWidgetItem *item = new QListWidgetItem;
            QString text = tr("Сетка ") + QString::number(p->meshList[i]);
            item->setData(3, p->meshList[i]);
            item->setText(text);
            listWidget->addItem(item);
        }
    }
}
