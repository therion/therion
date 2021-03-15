#include "objectmodel.h"
#include <QJsonValue>

ObjectModel::ObjectModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void ObjectModel::fromJson(QJsonArray array) {
    beginResetModel();
    m_items.clear();
    endResetModel();
    for (auto val : array) {
        QJsonObject obj = val.toObject();
        QString objClass = obj["class"].toString();
        QString objType = obj["type"].toString();

        if (objClass == "point") {
            QJsonArray point = obj["point"].toArray();
            addPoint(objClass, objType, point[0].toDouble(), point[1].toDouble());
        } else if (objClass == "line") {
            QJsonArray points = obj["points"].toArray();
            std::unique_ptr<LinePointModel> linePointModel = std::make_unique<LinePointModel>();
            linePointModel->fromJson(points);
            addLine(objClass, objType, std::move(linePointModel));
        }
    }
}

void ObjectModel::addPoint(QString objClass, QString objType, double x, double y) {
    beginInsertRows({}, rowCount(), rowCount());
    m_items.push_back(Point{objClass, objType, x, y});
    endInsertRows();
}

void ObjectModel::addLine(QString objClass, QString objType, std::unique_ptr<LinePointModel> points) {
    beginInsertRows({}, rowCount(), rowCount());
    m_items.push_back(Line{objClass, objType, std::move(points)});
    endInsertRows();
    QPersistentModelIndex pIndex = index(rowCount()-1);
    LinePointModel *lpm = pIndex.data(LinePointsRole).value<LinePointModel*>();
    auto updateLine = [this,pIndex](){ emit dataChanged(pIndex, pIndex, {LineSvgRole}); };
    connect(lpm, &LinePointModel::modelReset, this, updateLine);
    connect(lpm, &LinePointModel::rowsInserted, this, updateLine);
    connect(lpm, &LinePointModel::rowsRemoved, this, updateLine);
    connect(lpm, &LinePointModel::dataChanged, this, updateLine);
}

int ObjectModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_items.size();
}

QVariant ObjectModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) {
        return {};
    }

    const ObjectItem &item = m_items[index.row()];

    switch (role) {
    case ClassNameRole:
        return std::visit(overloaded {[](const auto &arg) {return arg.className; }}, item);
    case TypeNameRole:
        return std::visit(overloaded {[](const auto &arg) {return arg.typeName; }}, item);
    default:
        return std::visit(overloaded {
                              [role](const Point &point) -> QVariant {
                                  switch (role) {
                                  case XRole:
                                      return point.x;
                                  case YRole:
                                      return point.y;
                                  default:
                                      return {};
                                  }
                              },
                              [role](const Line &line) -> QVariant {
                                  switch (role) {
                                  case LinePointsRole:
                                      return QVariant::fromValue<LinePointModel*>(line.points.get());
                                  case LineSvgRole:
                                      return line.points->svgPath();
                                  default:
                                      return {};
                                  }
                              }
                          },
                          item);
    }
}

QHash<int, QByteArray> ObjectModel::roleNames() const {
    return {
        {ClassRole, "classRole"},
        {ClassNameRole, "classNameRole"},
        {TypeRole, "typeRole"},
        {TypeNameRole, "typeNameRole"},
        {XRole, "xRole"},
        {YRole, "yRole"},
        {LinePointsRole, "linePointsRole"},
        {LineSvgRole, "lineSvgRole"}
    };
}
