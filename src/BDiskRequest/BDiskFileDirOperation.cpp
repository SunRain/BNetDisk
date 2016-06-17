#include "BDiskFileDirOperation.h"

#include <QDebug>

#include "BDiskHttpRequest.h"

BDiskFileDirOperation::BDiskFileDirOperation(QObject *parent)
    : QObject(parent)
    , m_request(new BDiskHttpRequest(this))
{

}
