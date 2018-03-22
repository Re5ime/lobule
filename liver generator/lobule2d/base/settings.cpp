#include "settings.h"

Settings* Settings::instance = new Settings();

Settings::Settings() {
    settings = new QSettings("SSCMM", "nd2d");
}

void Settings::Set(QString key, QVariant value) {
    instance->settings->setValue(key, value);
    instance->settings->sync();
}

QString Settings::S(QString key, QString defaul) {
    return instance->settings->value(key, defaul).toString();
}

QStringList Settings::L(QString key, QStringList defv) {
    return instance->settings->value(key, defv).toStringList();
}

int Settings::I(QString key, int defv) {
    return instance->settings->value(key, defv).toInt();
}

bool Settings::B(QString key, bool defv) {
    return instance->settings->value(key, defv).toBool();
}

QByteArray Settings::A(QString key, QByteArray defv) {
    return instance->settings->value(key, defv).toByteArray();
}
