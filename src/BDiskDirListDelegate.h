#ifndef BDISKDIRLISTDELEGATE_H
#define BDISKDIRLISTDELEGATE_H

#include <QObject>

#include "BDiskRequest/BDiskActions.h"

#include "qslistmodel.h"

class BDiskDirListDelegate : public QSListModel
{
    Q_OBJECT
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)
public:
    explicit BDiskDirListDelegate(QObject *parent = 0);
    virtual ~BDiskDirListDelegate();

    Q_INVOKABLE void showRoot();
    Q_INVOKABLE void show(const QString &dir);
    Q_INVOKABLE void cdup();
    Q_INVOKABLE void refresh();

    QString currentPath() const;

signals:
    void startRequest();
    void finishRequest();

    void currentPathChanged(QString currentPath);

private:
    void setCurrentPath(const QString &currentPath);
    void sync();
private:
    BDiskActionListDir *m_action;
    QVariantList m_dataList;
    QString m_currentPath;
};

#endif // BDISKDIRLISTDELEGATE_H
