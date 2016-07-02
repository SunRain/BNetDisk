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
public:
    explicit BDiskDownloadDelegate(QObject *parent = 0);
    virtual ~BDiskDownloadDelegate();

    Q_INVOKABLE void download(const QString &from, const QString &savePath, const QString &saveName);
//    Q_INVOKABLE void download(const QString &from, const QString &save);

signals:

public slots:

private:
    BDisOpDownload m_downloadOp;
    YADownloader::DLTaskAccessMgr *m_downloadMgr;
//    YADownloader::DLTask *m_downloadTask;
    QHash<QString, YADownloader::DLTask*> m_taskHash;
};

#endif // BDISKDOWNLOADDELEGATE_H
