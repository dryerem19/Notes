#include "stringtable.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

QJsonObject StringTable::toJson() const
{
    QJsonObject jsonObject;

    for (auto it = _table.constBegin(); it != _table.constEnd(); ++it) {
        jsonObject[it.key()] = it.value();
    }

    return jsonObject;
}

bool StringTable::fromJson(const QJsonObject &jsonObject)
{
    _table.clear();

    QStringList keys = jsonObject.keys();
    for (const QString& key : keys) {
        if (jsonObject[key].isString()) {
            _table[key] = jsonObject[key].toString();
        } else {
            qWarning() << "Некорректный тип данных для ключа:" << key;
            return false;
        }
    }

    return true;
}

bool StringTable::saveToFile(const QString &filename) const
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Не удалось открыть файл для записи:" << filename;
        return false;
    }

    QJsonDocument doc(toJson());
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}

bool StringTable::loadFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл для чтения:" << filename;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Некорректный JSON файл:" << filename;
        return false;
    }

    return fromJson(doc.object());
}

void StringTable::printAll() const
{
    qDebug() << "Таблица строк:";
    for (auto it = _table.constBegin(); it != _table.constEnd(); ++it) {
        qDebug() << "  " << it.key() << ":" << it.value();
    }
    qDebug() << "Всего строк:" << _table.size();
}
