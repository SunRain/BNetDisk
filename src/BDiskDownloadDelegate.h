#ifndef BDISKDOWNLOADDELEGATE_H
#define BDISKDOWNLOADDELEGATE_H

#include <QObject>
#include <QHash>

#include "DLTaskAccessMgr.h"
//#include "DLTask.h"

#include "BDiskRequest/BDiskOperationRequest.h"

namespace YADownloader {
class DLTask;
}
class BDiskDownloadDelegate : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList resumables READ resumables WRITE setResumables NOTIFY resumablesChanged)
public:
    explicit BDiskDownloadDelegate(QObject *parent = 0);
    virtual ~BDiskDownloadDelegate();

    Q_INVOKABLE void download(const QString &from, const QString &savePath, const QString &saveName);
//    Q_INVOKABLE void download(const QString &from, const QString &save);

    QVariantList resumables() const;

signals:
    void resumablesChanged(const QVariantList &resumables);

public slots:
    void setResumables(const QVariantList &resumables);

private:
    BDisOpDownload m_downloadOp;
    YADownloader::DLTaskAccessMgr *m_downloadMgr;
//    YADownloader::DLTask *m_downloadTask;
    QHash<QString, YADownloader::DLTask*> m_taskHash;
    QVariantList m_resumables;
};

#endif // BDISKDOWNLOADDELEGATE_H
