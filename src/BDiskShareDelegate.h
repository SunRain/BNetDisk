#ifndef BDISKSHAREDELEGATE_H
#define BDISKSHAREDELEGATE_H

#include <QObject>

#include "BDiskRequest/BDiskActions.h"

class BDiskShareDelegate : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList shareRecords READ shareRecords WRITE setShareRecords NOTIFY shareRecordsChanged)
public:
    explicit BDiskShareDelegate(QObject *parent = 0);
    virtual ~BDiskShareDelegate();

    Q_INVOKABLE void privShare(const QString &fsID);
    Q_INVOKABLE void pubShare(const QString &fsID);
    Q_INVOKABLE void showShareRecord(int page = 1);
    Q_INVOKABLE void cancelShare(const QString &shareId);

    QVariantList shareRecords() const;

signals:
    void startRequest();
    void finishRequest();
    void requestFailure();
    void pubShareLink(const QString &url);
    void privShareLink(const QString &url, const QString &passwd);
    void shareRecordsChanged(const QVariantList &shareRecords);
    void cancelShareSuccess();

private:
    void parseReply(BDiskBaseRequest::RequestRet ret, const QString &replyData, bool isPrivShare);
    void parseShareRecord(BDiskBaseRequest::RequestRet ret, const QString &replyData);
    void parseShareCancel(BDiskBaseRequest::RequestRet ret, const QString &replyData);
    void setShareRecords(const QVariantList &shareRecords);

private:
    BDiskActionPrivShare *m_privShare;
    BDiskActionPubShare *m_pubShare;
    BDiskActionShareRecord *m_shareRecord;
    BDiskActionCancelShare *m_shareCancel;
    QVariantList m_shareRecords;
};

#endif // BDISKSHAREDELEGATE_H
