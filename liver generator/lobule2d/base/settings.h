#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>
#include <QStringList>
#include <QByteArray>

class Settings
{
private:
    Settings();

    QSettings* settings;
    static Settings* instance;

public:
    static void Set(QString key, QVariant value);
    static QString S(QString key, QString defv = "");
    static QStringList L(QString key, QStringList defv = QStringList());
    static int I(QString key, int defv = 0);
    static bool B(QString key, bool defv = true);
    static QByteArray A(QString key, QByteArray defv = QByteArray());
};

#endif // SETTINGS_H
