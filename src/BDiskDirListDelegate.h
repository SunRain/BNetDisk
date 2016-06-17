#ifndef BDISKDIRLISTDELEGATE_H
#define BDISKDIRLISTDELEGATE_H

#include <QObject>

#include "BDiskRequest/BDiskActions.h"

#include "qslistmodel.h"

class BDiskDirListDelegate : public QSListModel
{
public:
    explicit BDiskDirListDelegate(QObject *parent = 0);
    virtual ~BDiskDirListDelegate();

    void showRoot();
    void show(const QString &dir);

private:
    void sync();
private:
    BDiskActionListDir *m_action;
    QVariantList m_dataList;
};

#endif // BDISKDIRLISTDELEGATE_H
