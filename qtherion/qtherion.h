#ifndef QTHERION_H
#define QTHERION_H

#include <QObject>
#include <QUrl>

#include "scrapmodel.h"

class QTherion : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString projectName READ projectName WRITE setProjectName NOTIFY projectNameChanged)
    Q_PROPERTY(ScrapModel* scrapModel READ scrapModel WRITE setScrapModel NOTIFY scrapModelChanged)

    ScrapModel *m_scrapModel;

    QString m_projectName{"QTherion"};

public:
    explicit QTherion(QObject *parent = nullptr);

    QString projectName() const
    {
        return m_projectName;
    }

    ScrapModel* scrapModel() const
    {
        return m_scrapModel;
    }

public slots:
    void openProject(QUrl url);
    void processIndexFile(QString filename);
    void processDrawingFile(QString filename);

    void setProjectName(QString projectName)
    {
        if (m_projectName == projectName)
            return;

        m_projectName = projectName;
        emit projectNameChanged(m_projectName);
    }

    void setScrapModel(ScrapModel* scrapModel)
    {
        if (m_scrapModel == scrapModel)
            return;

        m_scrapModel = scrapModel;
        emit scrapModelChanged(m_scrapModel);
    }

signals:
    void projectNameChanged(QString projectName);
    void scrapModelChanged(ScrapModel* scrapModel);
};

#endif // QTHERION_H
