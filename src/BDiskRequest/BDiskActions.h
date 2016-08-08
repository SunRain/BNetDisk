#ifndef BDISKACTIONS_H
#define BDISKACTIONS_H

#include "BDiskBaseRequest.h"
#include "BDiskOperationRequest.h"

#define CREATE_OPR(Class) \
    Class op; \
    return dynamic_cast<BDiskBaseOperationRequest&>(op);

class BDiskActionListDir : public BDiskBaseRequest
{
    Q_OBJECT
public:
    explicit BDiskActionListDir(QObject *parent = 0) : BDiskBaseRequest(parent) {}

    // BDiskBaseRequest interface
protected:
    BDiskBaseOperationRequest operation() {
//        BDiskOpListDir op;
//        return dynamic_cast<BDiskBaseOperationRequest&>(op);
        CREATE_OPR(BDiskOpListDir);
    }
};


class BDiskActionPubShare : public BDiskBaseRequest
{
    Q_OBJECT
public:
    explicit BDiskActionPubShare(QObject *parent = 0): BDiskBaseRequest(parent) {}

    // BDiskBaseRequest interface
protected:
    BDiskBaseOperationRequest operation() {
        CREATE_OPR(BDiskOpPubShare);
    }
};

class BDiskActionPrivShare : public BDiskBaseRequest
{
    Q_OBJECT
public:
    explicit BDiskActionPrivShare(QObject *parent = 0): BDiskBaseRequest(parent) {}

    // BDiskBaseRequest interface
protected:
    BDiskBaseOperationRequest operation() {
        CREATE_OPR(BDiskOpPrivShare);
    }
};

class BDiskActionFileRename : public BDiskBaseRequest
{
    Q_OBJECT
public:
    explicit BDiskActionFileRename(QObject *parent = 0): BDiskBaseRequest(parent) {}

    // BDiskBaseRequest interface
protected:
    BDiskBaseOperationRequest operation() {
        CREATE_OPR(BDiskOpFileRename);
    }
};

class BDiskActionFileDelete : public BDiskBaseRequest
{
    Q_OBJECT
public:
    explicit BDiskActionFileDelete(QObject *parent = 0): BDiskBaseRequest(parent) {}
    virtual ~BDiskActionFileDelete() {}

    // BDiskBaseRequest interface
protected:
    BDiskBaseOperationRequest operation() {
        CREATE_OPR(BDiskOpFileDelete);
    }
};

class BDiskActionCategoryList : public BDiskBaseRequest
{
    Q_OBJECT
public:
    explicit BDiskActionCategoryList(QObject *parent = 0): BDiskBaseRequest(parent) {}
    virtual ~BDiskActionCategoryList() {}

    // BDiskBaseRequest interface
protected:
    BDiskBaseOperationRequest operation() {
        CREATE_OPR(BDiskOpCategoryList);
    }
};

class BDiskActionShareRecord : public BDiskBaseRequest
{
    Q_OBJECT
public:
    explicit BDiskActionShareRecord(QObject *parent = 0): BDiskBaseRequest(parent) {}
    virtual ~BDiskActionShareRecord() {}

    // BDiskBaseRequest interface
protected:
    BDiskBaseOperationRequest operation() {
        CREATE_OPR(BDiskOpShareRecord);
    }
};

class BDiskActionCancelShare : public BDiskBaseRequest
{
    Q_OBJECT
public:
    explicit BDiskActionCancelShare(QObject *parent = 0): BDiskBaseRequest(parent) {}
    virtual ~BDiskActionCancelShare() {}

    // BDiskBaseRequest interface
protected:
    BDiskBaseOperationRequest operation() {
        CREATE_OPR(BDiskOpCancelShare);
    }
};

class BDiskActionRecycleList : public BDiskBaseRequest
{
    Q_OBJECT
public:
    explicit BDiskActionRecycleList(QObject *parent = 0): BDiskBaseRequest(parent) {}
    virtual ~BDiskActionRecycleList() {}

    // BDiskBaseRequest interface
protected:
    BDiskBaseOperationRequest operation() {
        CREATE_OPR(BDiskOpRecycleList);
    }
};

class BDiskActionRecycleRestore : public BDiskBaseRequest
{
    Q_OBJECT
public:
    explicit BDiskActionRecycleRestore(QObject *parent = 0): BDiskBaseRequest(parent) {}
    virtual ~BDiskActionRecycleRestore() {}

    // BDiskBaseRequest interface
protected:
    BDiskBaseOperationRequest operation() {
        CREATE_OPR(BDiskOpRecycleRestore);
    }
};

class BDiskActionSearch : public BDiskBaseRequest
{
    Q_OBJECT
public:
    explicit BDiskActionSearch(QObject *parent = 0): BDiskBaseRequest(parent) {}
    virtual ~BDiskActionSearch() {}

    // BDiskBaseRequest interface
protected:
    BDiskBaseOperationRequest operation() {
        CREATE_OPR(BDiskOpSearch);
    }
};

#endif // BDISKACTIONS_H
