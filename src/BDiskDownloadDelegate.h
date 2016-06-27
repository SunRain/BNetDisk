#ifndef BDISKDOWNLOADDELEGATE_H
#define BDISKDOWNLOADDELEGATE_H

#include <QObject>

#include "DownloadMgr.h"
#include "DLTask.h"

#include "BDiskRequest/BDiskOperationRequest.h"

class BDiskDownloadDelegate : public QObject
{
    Q_OBJECT
public:
    explicit BDiskDownloadDelegate(QObject *parent = 0);
    virtual ~BDiskDownloadDelegate();

    Q_INVOKABLE void download();

signals:

public slots:

private:
    BDisOpDownload m_downloadOp;
    YADownloader::DownloadMgr *m_downloadMgr;
    YADownloader::DLTask *m_downloadTask;
};

#endif // BDISKDOWNLOADDELEGATE_H
