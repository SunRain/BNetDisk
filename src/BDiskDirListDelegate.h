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
//    enum CategoryType {
//        TYPE_VIDEO = 0x0,
//        TYPE_IMAGE,
//        TYPE_DOC,
//        TYPE_EXE,
//        TYPE_BT,
//        TYPE_MUSIC,
//        TYPE_OTHER
//    };
//    Q_ENUM(CategoryType)

    explicit BDiskDirListDelegate(QObject *parent = 0);
    virtual ~BDiskDirListDelegate();

    Q_INVOKABLE void showRoot();
    Q_INVOKABLE void show(const QString &dir);
    Q_INVOKABLE void cdup();
    Q_INVOKABLE void refresh();

    Q_INVOKABLE void showVideo(int page = 1);
    Q_INVOKABLE void showImage(int page = 1);
    Q_INVOKABLE void showDoc(int page = 1);
    Q_INVOKABLE void showExe(int page = 1);
    Q_INVOKABLE void showBT(int page = 1);
    Q_INVOKABLE void showMusic(int page = 1);
    Q_INVOKABLE void showOther(int page = 1);

    Q_INVOKABLE void showRecycleList(int page = 1);

    Q_INVOKABLE void recycleRestore(const QString &fsId);

    QString currentPath() const;

    ///
    /// \brief dirList
    /// \return dirs and files at current path
    ///
    QVariantList dirList() const;

    ///
    /// \brief currentPathList
    /// \return current path as list, splited by "/"
    ///
    QStringList currentPathList() const;

signals:
    void requestFailure();
    void startRequest();
    void finishRequest();
    void currentPathChanged(QString currentPath);
    void dirListChanged(const QVariantList &dirList);
    void currentPathListChanged(const QStringList &currentPathList);
    void recycleRestoreSuccess();

private slots:
    void handleDirList(BDiskBaseRequest::RequestRet ret, const QString &replyData);
    void handleCategoryList(BDiskBaseRequest::RequestRet ret, const QString &replyData);
    void handleRecycleList(BDiskBaseRequest::RequestRet ret, const QString &replyData);

private:
    void setCurrentPath(const QString &currentPath);
    void setCurrentPathList(const QStringList &currentPathList);
    void sync();
    void showCategory(int categoryType, int page);

private:
    BDiskActionListDir *m_action;
    BDiskActionCategoryList *m_categoryList;
    BDiskActionRecycleList *m_recycleList;
    BDiskActionRecycleRestore *m_recycleRestore;

    QVariantList m_dataList;
    QString m_currentPath;
    QStringList m_currentPathList;
};

#endif // BDISKDIRLISTDELEGATE_H
