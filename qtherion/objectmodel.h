#ifndef OBJECTMODEL_H
#define OBJECTMODEL_H

#include <QAbstractListModel>
#include <QJsonArray>
#include <QJsonObject>
#include <vector>

#include "objectitem.h"

class ObjectModel : public QAbstractListModel
{
    Q_OBJECT
    std::vector<ObjectItem> m_items;

public:
    enum ObjectRole {
        ClassRole = Qt::UserRole,
        ClassNameRole,
        TypeRole,
        TypeNameRole,
        XRole,
        YRole,
        LinePointsRole,
        LineSvgRole
    };
    Q_ENUM(ObjectRole)
    explicit ObjectModel(QObject *parent = nullptr);
    void fromJson(QJsonArray array);
    void addPoint(QString objClass, QString objType, double x, double y);
    void addLine(QString objClass, QString objType, std::unique_ptr<LinePointModel> points);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const;

};

#endif // OBJECTMODEL_H
