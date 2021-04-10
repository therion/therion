#include "scrapmodel.h"

ScrapModel::ScrapModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ScrapModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_items.size();
}

QVariant ScrapModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) {
        return {};
    }

    const ScrapItem &item = m_items[index.row()];

    switch (static_cast<ScrapRole>(role)) {
    case ScrapModel::NameRole:
        return item.name;
    case ScrapModel::FilenameRole:
        return item.filename;
    case ScrapModel::ObjectsRole:
        return QVariant::fromValue<ObjectModel*>(item.objects.get());
    }

    return {};
}

QHash<int, QByteArray> ScrapModel::roleNames() const {
    return {
        {NameRole, "nameRole"},
        {FilenameRole, "filenameRole"},
        {ObjectsRole, "objectsRole"}
    };
}

void ScrapModel::add(QString name, QString filename, std::unique_ptr<ObjectModel> objects) {
    beginInsertRows({}, rowCount(), rowCount());
    m_items.push_back(ScrapItem{name, filename, std::move(objects)});
    endInsertRows();
}

void ScrapModel::clear() {
    beginResetModel();
    m_items.clear();
    endResetModel();
}
