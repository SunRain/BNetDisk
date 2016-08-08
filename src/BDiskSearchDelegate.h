#ifndef BDISKSEARCHDELEGATE_H
#define BDISKSEARCHDELEGATE_H

#include <QObject>
#include <QVariantList>

#include "BDiskRequest/BDiskActions.h"

class QJsonObject;
class BDiskSearchDelegate : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList resultList READ resultList WRITE setResultList NOTIFY resultListChanged)
public:
    explicit BDiskSearchDelegate(QObject *parent = 0);
    virtual ~BDiskSearchDelegate();

    Q_INVOKABLE void search(const QString &key);

    QVariantList resultList() const;

signals:
    void startRequest();
    void finishRequest();
    void requestFailure();
    void resultListChanged(const QVariantList &resultList);

private slots:
    void handleResult(BDiskBaseRequest::RequestRet ret, const QString &replyData);
    void setResultList(const QVariantList &resultList);

private:
    void sync();
    QJsonObject parseToJsonObject(const QString &source) const;

private:
    BDiskActionSearch *m_search;
    QVariantList m_dataList;
};

#endif // BDISKSEARCHDELEGATE_H
