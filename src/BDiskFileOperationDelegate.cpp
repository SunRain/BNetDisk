#include "BDiskFileOperationDelegate.h"

BDiskFileOperationDelegate::BDiskFileOperationDelegate(QObject *parent)
    : QObject(parent)
    , m_fileRename(nullptr)
{

}

BDiskFileOperationDelegate::~BDiskFileOperationDelegate()
{
    if (m_fileRename) {
        if (!m_fileRename->isFinished())
            m_fileRename->abort();
        m_fileRename->deleteLater();
        m_fileRename = nullptr;
    }
}

void BDiskFileOperationDelegate::rename(const QString &path, const QString &newName)
{
    if (path.isEmpty() || newName.isEmpty())
        return;
    if (m_fileRename) {
        if (!m_fileRename->isFinished())
            m_fileRename->abort();
        QObject::disconnect(m_fileRename, 0, 0, 0);
        m_fileRename->deleteLater();
        m_fileRename = nullptr;
    }
}
