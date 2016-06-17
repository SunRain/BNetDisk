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


#endif // BDISKACTIONS_H
