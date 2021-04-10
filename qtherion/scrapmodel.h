#ifndef SCRAPMODEL_H
#define SCRAPMODEL_H

#include <QAbstractListModel>
#include <vector>
#include <memory>

#include "objectmodel.h"

class ScrapModel : public QAbstractListModel
{
    Q_OBJECT

    struct ScrapItem {
        QString name;
        QString filename;
        std::unique_ptr<ObjectModel> objects;
    };

    std::vector<ScrapItem> m_items;

public:
    enum ScrapRole {
        NameRole = Qt::UserRole,
        FilenameRole,
        ObjectsRole
    };
    explicit ScrapModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const;
    void add(QString name, QString filename, std::unique_ptr<ObjectModel> objects);
    void clear();
};

#endif // SCRAPMODEL_H
