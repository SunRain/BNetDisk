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
    OperationType operationType();

    void setParameters(const QString &key, const QString &value);

    void appendPostDataParameters(const QString &key, const QString &value);
    QHash<QString, QString> postDataParameters() const;
    QString postDataParameter(const QString &key, const QString &defaultValue = QString()) const;

    bool operator == (const BDiskBaseOperationRequest &other);
    bool operator != (const BDiskBaseOperationRequest &other);
    BDiskBaseOperationRequest &operator = (const BDiskBaseOperationRequest &other);
    BDiskBaseOperationRequest &operator()(const QString &key, const QString &value);

protected:
    virtual OperationType initOperationType();
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
    OperationType initOperationType() {
        return OPERATION_GET;
    }
};

class BDisOpDownload : public BDiskBaseOperationRequest
{
public:
    BDisOpDownload()
        : BDiskBaseOperationRequest() {
        setBaseUrl(BDISK_URL_PCS_REST);
//        setUrlPath();
        initiate();

    }

    // BDiskBaseOperationRequest interface
protected:
    OperationType initOperationType() {
        return OPERATION_GET;
    }

    void initParameters() {
        (*this)
        ("method", "download")
        ("app_id", "250528")
        ("path", "")
        ;
    }
};

class BDiskOpPubShare : public BDiskBaseOperationRequest
{
public:
    BDiskOpPubShare()
        : BDiskBaseOperationRequest() {
        setBaseUrl("http://pan.baidu.com/share/set");
        initiate();
        appendPostDataParameters("fid_list", "[]"); // file or dir object fs_id
        appendPostDataParameters("schannel", "0");
        appendPostDataParameters("channel_list", "[]");
    }
    virtual ~BDiskOpPubShare() {}

    // BDiskBaseOperationRequest interface
protected:
    OperationType initOperationType() {
        return OPERATION_POST;
    }

    void initParameters() {
        (*this)
        ("channel", "chunlei")
        ("clienttype", "0")
        ("web", "1")
        ("app_id", "250528")  //去掉appid也可以实现分享，暂时不知道appid从何而来， 当前本机测试为250528
        ("bdstoken", "")
//        ("logid", "")
        ;
    }
};

class BDiskOpPrivShare : public BDiskBaseOperationRequest
{
public:
    BDiskOpPrivShare()
        : BDiskBaseOperationRequest() {
        setBaseUrl("http://pan.baidu.com/share/set");
        initiate();
        appendPostDataParameters("fid_list", "[]"); // file or dir object fs_id
        appendPostDataParameters("schannel", "4");
        appendPostDataParameters("channel_list", "[]");
        appendPostDataParameters("pwd", "bdpw");
    }
    virtual ~BDiskOpPrivShare() {}

    // BDiskBaseOperationRequest interface
protected:
    OperationType initOperationType() {
        return OPERATION_POST;
    }

    void initParameters() {
        (*this)
        ("channel", "chunlei")
        ("clienttype", "0")
        ("web", "1")
        ("app_id", "250528")  //去掉appid也可以实现分享，暂时不知道appid从何而来， 当前本机测试为250528
        ("bdstoken", "")
//        ("logid", "")
        ;
    }
};

class BDiskOpFileRename : public BDiskBaseOperationRequest
{
public:
    BDiskOpFileRename()
        : BDiskBaseOperationRequest() {
        setUrlPath("filemanager");
        initiate();
//        filelist:"[{"path":"/Material+Design/linux软件/xware-desktop.tar.gz","newname":"xware-desktop-aa.tar.gz"}]"
        appendPostDataParameters("filelist", "[]");
    }
    virtual ~BDiskOpFileRename() {}

    // BDiskBaseOperationRequest interface
protected:
    OperationType initOperationType() {
        return OPERATION_POST;
    }

    void initParameters() {
        (*this)
        ("opera", "rename")
        ("async", "2")
        ("channel", "chunlei")
        ("web", "1")
        ("app_id", "250528")
        ("bdstoken", "")
//        ("logid", "")
        ("clienttype", "0")
        ;
    }
};

class BDiskOpFileDelete : public BDiskBaseOperationRequest
{
public:
    BDiskOpFileDelete()
        : BDiskBaseOperationRequest() {
        setUrlPath("filemanager");
        initiate();
        //filelist:"["/aaa/aaa.file"]"
        appendPostDataParameters("filelist", "[]");
    }
    virtual ~BDiskOpFileDelete() {}

    // BDiskBaseOperationRequest interface
protected:
    OperationType initOperationType() {
        return OPERATION_POST;
    }

    void initParameters() {
        (*this)
        ("opera", "delete")
        ("async", "2")
        ("channel", "chunlei")
        ("web", "1")
        ("app_id", "250528")
        ("bdstoken", "")
//        ("logid", "")
        ("clienttype", "0")
        ;
    }
};

class BDiskOpCategoryList : public BDiskBaseOperationRequest
{
public:
    BDiskOpCategoryList()
        : BDiskBaseOperationRequest() {
        setUrlPath("categorylist");
        initiate();
    }
    virtual ~BDiskOpCategoryList() {}

    // BDiskBaseOperationRequest interface
protected:
    OperationType initOperationType() {
        return OPERATION_GET;
    }

    void initParameters() {
        (*this)
        ("category", "3") // 1 video, 3 pic, 4 doc, 5 exe, 6 other, 7 bt, 2 music
        ("bdstoken", "")
//        ("logid", "")
        ("num", "100")
        ("order", "name") // name, size, time
        ("desc", "0")
        ("clienttype", "0")
        ("showempty", "0")
        ("web", "1")
        ("page", "1")
        ("channel", "chunlei")
        ("app_id", "250528")
        ;
    }
};

class BDiskOpShareRecord : public BDiskBaseOperationRequest
{
public:
    BDiskOpShareRecord()
        : BDiskBaseOperationRequest() {
        setBaseUrl("https://pan.baidu.com/share/record");
        initiate();
    }
    virtual ~BDiskOpShareRecord() {}

    // BDiskBaseOperationRequest interface
protected:
    OperationType initOperationType() {
        return OPERATION_GET;
    }
    void initParameters() {
        (*this)
        ("channel", "chunlei")
        ("clienttype", "0")
        ("web", "1")
        ("page", "1")
        ("order", "ctime")
        ("desc", "1")
        ("bdstoken", "")
        ("app_id", "250528")
        ;
    }
};

class BDiskOpCancelShare : public BDiskBaseOperationRequest
{
public:
    BDiskOpCancelShare()
        : BDiskBaseOperationRequest() {
        setBaseUrl("https://pan.baidu.com/share/cancel");
        initiate();
        appendPostDataParameters("shareid_list", "[]");
    }
    virtual ~BDiskOpCancelShare() {}

    // BDiskBaseOperationRequest interface
protected:
    OperationType initOperationType() {
        return OPERATION_POST;
    }
    void initParameters() {
        (*this)
        ("channel", "chunlei")
        ("clienttype", "0")
        ("web", "1")
        ("bdstoken", "")
        ("app_id", "250528")
        ;
    }
};

class BDiskOpRecycleList : public BDiskBaseOperationRequest
{
public:
    BDiskOpRecycleList()
        : BDiskBaseOperationRequest() {
        setUrlPath("recycle/list");
        initiate();
    }
    virtual ~BDiskOpRecycleList() {}

    // BDiskBaseOperationRequest interface
protected:
    OperationType initOperationType() {
        return OPERATION_GET;
    }
    void initParameters() {
        (*this)
        ("channel", "chunlei")
        ("clienttype", "0")
        ("web", "1")
        ("page", "1")
        ("desc", "1")
        ("bdstoken", "")
        ("app_id", "250528")
        ;
    }
};

class BDiskOpRecycleRestore : public BDiskBaseOperationRequest
{
public:
    BDiskOpRecycleRestore()
        : BDiskBaseOperationRequest() {
        setUrlPath("recycle/restore");
        initiate();
        appendPostDataParameters("fidlist", "[]");
    }
    virtual ~BDiskOpRecycleRestore() {}

    // BDiskBaseOperationRequest interface
protected:
    OperationType initOperationType() {
        return OPERATION_POST;
    }
    void initParameters() {
        (*this)
        ("channel", "chunlei")
        ("clienttype", "0")
        ("web", "1")
        ("t", QString::number(QDateTime::currentMSecsSinceEpoch()))
        ("bdstoken", "")
        ("async", "1")
        ("app_id", "250528")
        ;
    }
};

class BDiskOpSearch : public BDiskBaseOperationRequest
{
public:
    BDiskOpSearch()
        : BDiskBaseOperationRequest() {
        setUrlPath("search");
        initiate();
    }
    virtual ~BDiskOpSearch() {}

    // BDiskBaseOperationRequest interface
protected:
    OperationType initOperationType() {
        return OPERATION_GET;
    }
    void initParameters() {
        (*this)
        ("recursion", "1")
        ("order", "time")
        ("desc", "1")
        ("showempty", "0")
        ("web", "1")
        ("page", "1")
        ("num", "100")
        ("key", "")
        ("t", QString::number(QDateTime::currentMSecsSinceEpoch()))
        ("channel", "chunlei")
        ("clienttype", "0")
        ("bdstoken", "")
        ("app_id", "250528")
        ;
    }
};


#endif // BDISKOPERATIONREQUEST_H
