#ifndef BDISKOPERATIONREQUEST_H
#define BDISKOPERATIONREQUEST_H

#include <QSharedDataPointer>
#include <QString>
#include <QUrl>
#include <QDateTime>

#include "BDiskTokenProvider.h"
#include "BDiskConst.h"

class BDiskBaseOperationRequestPriv;
class BDiskBaseOperationRequest
{
public:
    enum OperationType {
        OPERATION_GET = 0x0,
        OPERATION_POST,
        OPERATION_UNDEFINED
    };

    BDiskBaseOperationRequest();
    BDiskBaseOperationRequest(const BDiskBaseOperationRequest &other);
    virtual ~BDiskBaseOperationRequest();

    void initiate();
    void setBaseUrl(const QString &url);
    QUrl initUrl() const;
    OperationType op();

    bool operator == (const BDiskBaseOperationRequest &other);
    bool operator != (const BDiskBaseOperationRequest &other);
    BDiskBaseOperationRequest &operator = (const BDiskBaseOperationRequest &other);
    BDiskBaseOperationRequest &operator()(const QString &key, const QString &value);

protected:
    virtual OperationType getOp();
    virtual void initParameters();
    void setUrlPath(const QString &path, const QString &tag = QString());

private:
    QSharedDataPointer<BDiskBaseOperationRequestPriv> d;
};

class BDiskOpListDir : public BDiskBaseOperationRequest
{
public:
    BDiskOpListDir()
        : BDiskBaseOperationRequest()
    {
        setUrlPath("list");
        initiate();
    }

    // BDiskBaseOperationRequest interface
protected:
    void initParameters() {
//        showempty=0
//        app_id=
//        logid=
        (*this)
        ("dir", "/")
//        ("page", "1") //empty will show all list
//        ("num", "100") //empty will show all list
        ("order", "time")
        ("desc", "1")
        ("channel", "chunlei")
        ("clienttype", "0")
        ("web", "1")
        ("bdstoken", BDiskTokenProvider::instance()->bdstoken())
        ("t", QString::number(QDateTime::currentMSecsSinceEpoch()))
        ;
    }
    OperationType getOp() {
        return OPERATION_GET;
    }
};

#endif // BDISKOPERATIONREQUEST_H
