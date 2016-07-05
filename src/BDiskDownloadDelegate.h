#ifndef BDISKDOWNLOADDELEGATE_H
#define BDISKDOWNLOADDELEGATE_H

#include <QObject>
#include <QHash>

#include "DLTaskAccessMgr.h"
//#include "DLTask.h"

#include "BDiskRequest/BDiskOperationRequest.h"

namespace YADownloader {
class DLTask;
class DLTaskInfo;
}
class BDiskDownloadDelegate : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList tasks READ tasks WRITE setTasks NOTIFY tasksChanged)
public:
    explicit BDiskDownloadDelegate(QObject *parent = 0);
    virtual ~BDiskDownloadDelegate();

    Q_INVOKABLE void download(const QString &from, const QString &savePath, const QString &saveName);

    QVariantList tasks() const;

signals:
    void tasksChanged(const QVariantList &tasks);

public slots:
    void setTasks(const QVariantList &tasks);

private:
    ///
    /// \brief parseDLTaskInfoList
    /// Parse DLTaskInfoList and insert to m_taskInfoHash
    /// \param list
    ///
    void parseDLTaskInfoList(const YADownloader::DLTaskInfoList &list);
    QVariantList convertTaskInfoHash();

private:
    BDisOpDownload m_downloadOp;
    YADownloader::DLTaskAccessMgr *m_downloadMgr;
//    YADownloader::DLTask *m_downloadTask;
    QHash<QString, YADownloader::DLTask*> m_taskHash;
    QHash<QString, YADownloader::DLTaskInfo> m_taskInfoHash;
    QVariantList m_tasks;
    //    QVariantList m_runnings;

};

#endif // BDISKDOWNLOADDELEGATE_H
