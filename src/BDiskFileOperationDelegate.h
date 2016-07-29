#ifndef BDISKFILEOPERATIONDELEGATE_H
#define BDISKFILEOPERATIONDELEGATE_H

#include <QObject>

#include "BDiskRequest/BDiskActions.h"

class BDiskFileOperationDelegate : public QObject
{
    Q_OBJECT
public:
    explicit BDiskFileOperationDelegate(QObject *parent = 0);
    virtual ~BDiskFileOperationDelegate();

    Q_INVOKABLE void rename(const QString &path, const QString &newName);
private:
    BDiskActionFileRename *m_fileRename;
};

#endif // BDISKFILEOPERATIONDELEGATE_H
