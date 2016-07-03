#ifndef BDISKDIRLISTDELEGATE_H
#define BDISKDIRLISTDELEGATE_H

#include <QObject>
#include <QVariantList>

#include "BDiskRequest/BDiskActions.h"

#include "qslistmodel.h"

class BDiskDirListDelegate : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)
    Q_PROPERTY(QStringList currentPathList READ currentPathList NOTIFY currentPathListChanged)
    Q_PROPERTY(QVariantList dirList READ dirList NOTIFY dirListChanged)
public:
    explicit BDiskDirListDelegate(QObject *parent = 0);
    virtual ~BDiskDirListDelegate();

    Q_INVOKABLE void showRoot();
    Q_INVOKABLE void show(const QString &dir);
    Q_INVOKABLE void cdup();
    Q_INVOKABLE void refresh();

    QString currentPath() const;

    QVariantList dirList() const;

    QStringList currentPathList() const;

signals:
    void requestFailure();
    void startRequest();
    void finishRequest();
    void currentPathChanged(QString currentPath);
    void dirListChanged(const QVariantList &dirList);
    void currentPathListChanged(const QStringList &currentPathList);

private:
    void setCurrentPath(const QString &currentPath);
    void setCurrentPathList(const QStringList &currentPathList);
    void sync();
private:
    BDiskActionListDir *m_action;
    QVariantList m_dataList;
    QString m_currentPath;
    QStringList m_currentPathList;
};

#endif // BDISKDIRLISTDELEGATE_H
