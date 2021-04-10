#include "qtherion.h"

#include <QDebug>
#include <QDir>
#include <QStringList>
#include <QJsonDocument>

QTherion::QTherion(QObject *parent) : QObject(parent), m_scrapModel(new ScrapModel(this)) {
//    openProject(QUrl::fromLocalFile("/home/micod/Stažené/rabbit"));
}

void QTherion::openProject(QUrl url) {
    QFileInfo fi(url.toLocalFile());
    QDir dir(url.toLocalFile());
    if (!fi.isDir()) {
        qDebug() << "selected path is not a folder";
        return;
    }
    QStringList entries = dir.entryList(QDir::Files);

    m_scrapModel->clear();
    for (QString file : entries) {
        QString fullPath = url.toLocalFile()+QDir::separator()+file;
        if (file == "index.qth") {
            processIndexFile(fullPath);
        } else if (file.startsWith("d_") && file.endsWith(".qth")) {
            processDrawingFile(fullPath);
        }
    }
}

void QTherion::processIndexFile(QString filename) {
    QFile file(filename);
    bool ok = file.open(QIODevice::ReadOnly);
    if (!ok) {
        qDebug() << "cannot open index file" << filename;
        return;
    }
    QByteArray content = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(content);
    if (!doc.isObject()) {
        qDebug() << "invalid index file" << filename;
        return;
    }
    QJsonObject obj = doc.object();
    setProjectName(obj["title"].toString());
}

void QTherion::processDrawingFile(QString filename) {
    QFile file(filename);
    bool ok = file.open(QIODevice::ReadOnly);
    if (!ok) {
        qDebug() << "cannot open drawing file" << filename;
        return;
    }
    QByteArray content = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(content);
    if (!doc.isObject()) {
        qDebug() << "invalid drawing file" << filename;
        return;
    }
    QJsonObject obj = doc.object();
    if (!obj.contains("scraps")) {
        qDebug() << "missing scraps field in drawing file" << filename;
        return;
    }
    QJsonArray scraps = obj["scraps"].toArray();
    for (QJsonValue val : scraps) {
        QJsonObject scrap = val.toObject();
        QString name  = scrap["name"].toString();
        std::unique_ptr<ObjectModel> objects = std::make_unique<ObjectModel>();
        objects->fromJson(scrap["objects"].toArray());
        m_scrapModel->add(name, filename, std::move(objects));
    }
}
