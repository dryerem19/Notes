#ifndef STRINGTABLE_H
#define STRINGTABLE_H

class QJsonObject;

#include <QMap>
#include <QString>

class StringTable
{
public:
    StringTable() = default;

    inline void addString(const QString& id, const QString& value) {
        _table[id] = value;
    }

    inline bool removeString(const QString& id) {
        return _table.remove(id) > 0;
    }

    inline QString getString(const QString& id) const {
        return _table.value(id, "");
    }

    inline QList<QString> getAllIds() const { return _table.keys(); }

    QJsonObject toJson() const;
    bool fromJson(const QJsonObject& jsonObject);

    bool saveToFile(const QString& filename) const;
    bool loadFromFile(const QString& filename);

    void printAll() const;

    inline int size() const {
        return _table.size();
    }

    inline void clear() {
        _table.clear();
    }


private:
    QMap<QString, QString> _table;
};

#endif // STRINGTABLE_H
