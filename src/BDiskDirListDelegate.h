#ifndef BDISKDIRLISTDELEGATE_H
#define BDISKDIRLISTDELEGATE_H

#include <QObject>

#include "BDiskRequest/BDiskActions.h"

#include "qslistmodel.h"

class BDiskDirListDelegate : public QSListModel
{
    Q_OBJECT
public:
    explicit BDiskDirListDelegate(QObject *parent = 0);
    virtual ~BDiskDirListDelegate();

    Q_INVOKABLE void showRoot();
    Q_INVOKABLE void show(const QString &dir);

signals:
    void startRequest();
    void finishRequest();

private:
    void sync();
private:
    BDiskActionListDir *m_action;
    QVariantList m_dataList;
};

#endif // BDISKDIRLISTDELEGATE_H
