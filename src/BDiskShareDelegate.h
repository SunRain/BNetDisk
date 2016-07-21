#ifndef BDISKSHAREDELEGATE_H
#define BDISKSHAREDELEGATE_H

#include <QObject>

#include "BDiskRequest/BDiskActions.h"

class BDiskShareDelegate : public QObject
{
    Q_OBJECT
public:
    explicit BDiskShareDelegate(QObject *parent = 0);
    virtual ~BDiskShareDelegate();

    Q_INVOKABLE void privShare(const QString &fsID);
    Q_INVOKABLE void pubShare(const QString &fsID);

signals:
    void startRequest();
    void finishRequest();
    void requestFailure();
    void pubShareLink(const QString &url);
    void privShareLink(const QString &url, const QString &passwd);

private:
    void parseReply(BDiskBaseRequest::RequestRet ret, const QString &replyData, bool isPrivShare);

private:
    BDiskActionPrivShare *m_privShare;
    BDiskActionPubShare *m_pubShare;
};

#endif // BDISKSHAREDELEGATE_H
