#include "linepointmodel.h"
#include <QJsonObject>

LinePointModel::LinePointModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int LinePointModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_points.size();
}

QVariant LinePointModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) {
        return {};
    }

    const LinePoint &point = m_points[index.row()];

    switch (static_cast<LinePointRole>(role)) {
    case XRole:
        return point.x;
    case YRole:
        return point.y;
    case Cp1xRole:
        return point.cp1x;
    case Cp1yRole:
        return point.cp1y;
    case Cp2xRole:
        return point.cp2x;
    case Cp2yRole:
        return point.cp2y;
    case HasCpsRole:
        return point.hasCps;
    }

    return {};
}

bool LinePointModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) {
        return false;
    }

    LinePoint &point = m_points[index.row()];

    switch (static_cast<LinePointRole>(role)) {
    case LinePointModel::XRole: {
        double newX = value.toDouble();
        if (point.x != newX) {
            point.x = newX;
            emit dataChanged(index, index, {XRole});
            return true;
        }
        break;
    }
    case LinePointModel::YRole: {
        double newY = value.toDouble();
        if (point.y != newY) {
            point.y = newY;
            emit dataChanged(index, index, {YRole});
            return true;
        }
        break;
    }
    case LinePointModel::Cp1xRole: {
        double newCp1x = value.toDouble();
        if (point.cp1x != newCp1x) {
            point.cp1x = newCp1x;
            emit dataChanged(index, index, {Cp1xRole});
            return true;
        }
        break;
    }
    case LinePointModel::Cp1yRole: {
        double newCp1y = value.toDouble();
        if (point.cp1y != newCp1y) {
            point.cp1y = newCp1y;
            emit dataChanged(index, index, {Cp1yRole});
            return true;
        }
        break;
    }
    case LinePointModel::Cp2xRole: {
        double newCp2x = value.toDouble();
        if (point.cp2x != newCp2x) {
            point.cp2x = newCp2x;
            emit dataChanged(index, index, {Cp2xRole});
            return true;
        }
        break;
    }
    case LinePointModel::Cp2yRole: {
        double newCp2y = value.toDouble();
        if (point.cp2y != newCp2y) {
            point.cp2y = newCp2y;
            emit dataChanged(index, index, {Cp2yRole});
            return true;
        }
        break;
    }
    case LinePointModel::HasCpsRole: {
        bool newHasCps = value.toDouble();
        if (point.hasCps != newHasCps) {
            point.hasCps = newHasCps;
            emit dataChanged(index, index, {HasCpsRole});
            return true;
        }
        break;
    }
    }
    return false;
}

QHash<int, QByteArray> LinePointModel::roleNames() const {
    return {
        {XRole, "xRole"},
        {YRole, "yRole"},
        {Cp1xRole, "cp1xRole"},
        {Cp1yRole, "cp1yRole"},
        {Cp2xRole, "cp2xRole"},
        {Cp2yRole, "cp2yRole"},
        {HasCpsRole, "hasCpsRole"}
    };
}

void LinePointModel::fromJson(QJsonArray array) {
    beginResetModel();
    m_points.clear();
    for (auto val : array) {
        QJsonObject obj = val.toObject();
        QJsonArray point = obj["point"].toArray();
        bool hasCps = obj.contains("cp1") && obj.contains("cp2");
        LinePoint linePoint;
        linePoint.x = point[0].toDouble();
        linePoint.y = point[1].toDouble();
        linePoint.hasCps = hasCps;
        if (hasCps) {
            QJsonArray cp1 = obj["cp1"].toArray();
            QJsonArray cp2 = obj["cp2"].toArray();
            linePoint.cp1x = cp1[0].toDouble();
            linePoint.cp1y = cp1[1].toDouble();
            linePoint.cp2x = cp2[0].toDouble();
            linePoint.cp2y = cp2[1].toDouble();
        }
        m_points << linePoint;
    }
    endResetModel();

}

QString LinePointModel::svgPath() const {
    QString path;
    for (int i = 0; i < m_points.size(); ++i) {
        const LinePoint &p = m_points[i];
        if (i == 0) {
            path = QString("M %1 %2").arg(p.x).arg(p.y);
        } else if (!p.hasCps) {
            path += QString(" L %1 %2").arg(p.x).arg(p.y);
        } else {
            path += QString(" C %1 %2, %3 %4, %5 %6").arg(p.cp1x).arg(p.cp1y).arg(p.cp2x).arg(p.cp2y).arg(p.x).arg(p.y);
        }
    }
    return path;
}
