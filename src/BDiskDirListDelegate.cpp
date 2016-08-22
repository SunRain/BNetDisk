#include "BDiskDirListDelegate.h"

#include <QDebug>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

//#include "qsdiffrunner.h"

#include "BDiskRequest/BDiskBaseRequest.h"
#include "BDiskRequest/BDiskFileObjectKeys.h"

const static int CATEGORY_TYPE_VIDEO = 1;
const static int CATEGORY_TYPE_IMAGE = 3;
const static int CATEGORY_TYPE_DOC = 4;
const static int CATEGORY_TYPE_EXE = 5;
const static int CATEGORY_TYPE_BT = 7;
const static int CATEGORY_TYPE_MUSIC = 2;
const static int CATEGORY_TYPE_OTHER = 6;


#define FREE_REQUEST(ptr) \
    if (ptr) { \
        if (!ptr->isFinished()) \
            ptr->abort(); \
        ptr->deleteLater(); \
        ptr = nullptr; \
    }

BDiskDirListDelegate::BDiskDirListDelegate(QObject *parent)
    : QObject(parent)
    , m_categoryList(nullptr)
    , m_recycleList(nullptr)
    , m_recycleRestore(nullptr)
    , m_currentPath(QString("/"))
{
    m_dirList = new BDiskActionListDir(this);
    setCurrentPathList((QStringList()<<"/"));

    connect(m_dirList, &BDiskBaseRequest::requestStarted, this, &BDiskDirListDelegate::startRequest);
    connect(m_dirList, &BDiskBaseRequest::requestResult, this, &BDiskDirListDelegate::handleDirList);
}

BDiskDirListDelegate::~BDiskDirListDelegate()
{
//    if (m_dirList) {
//        if (!m_dirList->isFinished())
//            m_dirList->abort();
//        m_dirList->deleteLater();
//        m_dirList = nullptr;
//    }
//    if (m_categoryList) {
//        if (!m_categoryList->isFinished())
//            m_categoryList->abort();
//        m_categoryList->deleteLater();
//        m_categoryList = nullptr;
//    }
//    if (m_recycleList) {
//        if (!m_recycleList->isFinished())
//            m_recycleList->abort();
//        m_recycleList->deleteLater();
//        m_recycleList = nullptr;
//    }
//    if (m_recycleRestore) {
//        if (!m_recycleRestore->isFinished())
//            m_recycleRestore->abort();
//        m_recycleRestore->deleteLater();
//        m_recycleRestore = nullptr;
//    }
    FREE_REQUEST(m_dirList);
    FREE_REQUEST(m_categoryList);
    FREE_REQUEST(m_recycleList);
    FREE_REQUEST(m_recycleRestore);
}

void BDiskDirListDelegate::showRoot()
{
//    m_dirList->request();
//    setCurrentPath("/");
    showDir("/");
}

void BDiskDirListDelegate::showDir(const QString &dir)
{
    if (!m_dirList->isFinished())
        m_dirList->abort();
    m_dirList->operationPtr()->setParameters("dir", dir);
    m_dirList->request();
    setCurrentPath(dir);
}

void BDiskDirListDelegate::cdup()
{
    qDebug()<<Q_FUNC_INFO<<" cdup for "<<m_currentPath;
    if (!m_currentPath.contains("/"))
        return;
    if (m_currentPath == "/") {
        showDir(m_currentPath);
        return;
    }
    QString str = m_currentPath;
    if (str.startsWith("/"))
        str = str.right(str.length() -1);
    if (!str.contains("/"))
        str = "/";
    QStringList list = str.split("/");
    qDebug()<<Q_FUNC_INFO<<"    list "<<list;
    if (list.isEmpty())
        return;
    list.removeLast();
    str = list.join("/");
    qDebug()<<Q_FUNC_INFO<<"    str "<<str;
    if (!str.startsWith("/"))
        str = QString("/%1").arg(str);
//    m_action->setParameters("dir", str);
//    m_dirList->operationPtr()->setParameters("dir", str);
//    m_dirList->request();
//    setCurrentPath(str);
    showDir(str);
}

void BDiskDirListDelegate::refresh()
{
//    m_dirList->request();
    showDir(m_currentPath);
}

void BDiskDirListDelegate::showVideo(int page)
{
    showCategory(CATEGORY_TYPE_VIDEO, page);
}

void BDiskDirListDelegate::showImage(int page)
{
    showCategory(CATEGORY_TYPE_IMAGE, page);
}

void BDiskDirListDelegate::showDoc(int page)
{
    showCategory(CATEGORY_TYPE_DOC, page);
}

void BDiskDirListDelegate::showExe(int page)
{
    showCategory(CATEGORY_TYPE_EXE, page);
}

void BDiskDirListDelegate::showBT(int page)
{
    showCategory(CATEGORY_TYPE_BT, page);
}

void BDiskDirListDelegate::showMusic(int page)
{
    showCategory(CATEGORY_TYPE_MUSIC, page);
}

void BDiskDirListDelegate::showOther(int page)
{
    showCategory(CATEGORY_TYPE_OTHER, page);
}

void BDiskDirListDelegate::showRecycleList(int page)
{
    if (page < 1)
        return;
    if (m_recycleList && !m_recycleList->isFinished())
        m_recycleList->abort();

    if (!m_recycleList) {
        m_recycleList = new BDiskActionRecycleList(this);
        connect(m_recycleList, &BDiskBaseRequest::requestStarted, this, &BDiskDirListDelegate::startRequest);
        connect(m_recycleList, &BDiskBaseRequest::requestResult, this, &BDiskDirListDelegate::handleRecycleList);
    }

    m_recycleList->operationPtr()->setParameters("page", QString::number(page));
    m_recycleList->request();
}

void BDiskDirListDelegate::recycleRestore(const QString &fsId)
{
    if (fsId.isEmpty())
        return;

    if (m_recycleRestore && !m_recycleRestore->isFinished())
        m_recycleRestore->abort();

    if (!m_recycleRestore) {
        m_recycleRestore = new BDiskActionRecycleRestore(this);
        connect(m_recycleRestore, &BDiskBaseRequest::requestStarted, this, &BDiskDirListDelegate::startRequest);
        connect(m_recycleRestore, &BDiskBaseRequest::requestResult,
                [&](BDiskBaseRequest::RequestRet ret, const QString &replyData) {

            FREE_REQUEST(m_recycleRestore);

            if (ret == BDiskBaseRequest::RET_SUCCESS) {
                qDebug()<<Q_FUNC_INFO<<">>>>>> ok";
//                QJsonParseError error;
//                QJsonDocument doc = QJsonDocument::fromJson (replyData.toLocal8Bit(), &error);
//                if (error.error != QJsonParseError::NoError) {
//                    qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
//                    return;
//                }
//                QJsonObject obj = doc.object();
//                int ret = obj.value("errno").toInt(-1);
//                if (ret != 0) {
//                    qDebug()<<Q_FUNC_INFO<<"Error number "<<ret;
//                    emit requestFailure();
//                    return;
//                }
                QJsonObject o = parseToJsonObject(replyData);
                if (o.isEmpty()) {
                    emit requestFailure();
                } else {
                    emit recycleRestoreSuccess();
                }
            }
            emit finishRequest();
        });
    }
    m_recycleRestore->operationPtr()->appendPostDataParameters("fidlist", QString("[%1]").arg(fsId));
    m_recycleRestore->request();
}

QString BDiskDirListDelegate::currentPath() const
{
    return m_currentPath;
}

QVariantList BDiskDirListDelegate::dirList() const
{
    return m_dataList;
}

QStringList BDiskDirListDelegate::currentPathList() const
{
    return m_currentPathList;
}

void BDiskDirListDelegate::handleDirList(BDiskBaseRequest::RequestRet ret, const QString &replyData)
{
    qDebug()<<Q_FUNC_INFO<<" ret "<<ret;
    m_dataList.clear();
    if (ret == BDiskBaseRequest::RET_SUCCESS) {
        qDebug()<<Q_FUNC_INFO<<">>>>>> ok";
//        QJsonParseError error;
//        QJsonDocument doc = QJsonDocument::fromJson (replyData.toLocal8Bit(), &error);
//        if (error.error != QJsonParseError::NoError) {
//            qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
//            sync();
//            return;
//        }
//        QJsonObject obj = doc.object();
//        int ret = obj.value("errno").toInt(-1);
//        if (ret != 0) {
//            qDebug()<<Q_FUNC_INFO<<"Error number "<<ret;
//            sync();
//            emit requestFailure();
//            return;
//        }
        QJsonObject obj = parseToJsonObject(replyData);
        if (obj.isEmpty()) {
            sync();
            emit requestFailure();
            return;
        }
        QJsonArray array = obj.value("list").toArray();
        QMap<QString, QVariant> fileMap, dirMap;
        foreach (QJsonValue v, array) {
            QJsonObject o = v.toObject();
            QVariantMap map;
            foreach (QString key, BDiskFileObjectKeyName::keys()) {
                QJsonValue v = o.value(key);
                if (v.isBool()) {
                    bool ret = v.toBool();
                    map.insert(key, ret ? "1" : "0");
                } else if (v.isDouble()) {
                    map.insert(key, QString::number(v.toDouble(), 'f', 0));
                } else if (v.isString()) {
                    map.insert(key, v.toString());
                }
                //TODO more type
            }
//                qDebug()<<Q_FUNC_INFO<<" >>>>>> QVariant is "<<map;
//                m_dataList.append(map);
            // sort m_dataList to support item order
            if (map.value(BDISK_FILE_KEY_IS_DIR).toString() == "1") {
                dirMap.insert(map.value(BDISK_FILE_KEY_PATH).toString(), map);
            } else {
                fileMap.insert(map.value(BDISK_FILE_KEY_PATH).toString(), map);
            }
        }
        m_dataList.append(dirMap.values());
        m_dataList.append(fileMap.values());
        sync();
    }
    emit finishRequest();
}

void BDiskDirListDelegate::handleCategoryList(BDiskBaseRequest::RequestRet ret, const QString &replyData)
{
    qDebug()<<Q_FUNC_INFO<<" ret "<<ret;

    FREE_REQUEST(m_categoryList);

    m_dataList.clear();
    if (ret == BDiskBaseRequest::RET_SUCCESS) {
        qDebug()<<Q_FUNC_INFO<<">>>>>> ok";
//        QJsonParseError error;
//        QJsonDocument doc = QJsonDocument::fromJson (replyData.toLocal8Bit(), &error);
//        if (error.error != QJsonParseError::NoError) {
//            qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
//            sync();
//            return;
//        }
//        QJsonObject obj = doc.object();
//        int ret = obj.value("errno").toInt(-1);
//        if (ret != 0) {
//            qDebug()<<Q_FUNC_INFO<<"Error number "<<ret;
//            sync();
//            emit requestFailure();
//            return;
//        }
        QJsonObject obj = parseToJsonObject(replyData);
        if (obj.isEmpty()) {
            sync();
            emit requestFailure();
            return;
        }
        QJsonArray array = obj.value("info").toArray();
        foreach (QJsonValue v, array) {
            QJsonObject o = v.toObject();
            QVariantMap map;
            foreach (QString key, BDiskFileObjectKeyName::keys()) {
                QJsonValue v = o.value(key);
                if (v.isBool()) {
                    bool ret = v.toBool();
                    map.insert(key, ret ? "1" : "0");
                } else if (v.isDouble()) {
                    map.insert(key, QString::number(v.toDouble(), 'f', 0));
                } else if (v.isString()) {
                    map.insert(key, v.toString());
                } else if (v.isObject()) {
                    map.insert(key, v.toObject().toVariantMap());
                }
                //TODO more type
            }
            m_dataList.append(map);
        }
        sync();
    }
    emit finishRequest();
}

void BDiskDirListDelegate::handleRecycleList(BDiskBaseRequest::RequestRet ret, const QString &replyData)
{
    qDebug()<<Q_FUNC_INFO<<" ret "<<ret;

    FREE_REQUEST(m_recycleList);

    m_dataList.clear();
    if (ret == BDiskBaseRequest::RET_SUCCESS) {
        qDebug()<<Q_FUNC_INFO<<">>>>>> ok";
//        QJsonParseError error;
//        QJsonDocument doc = QJsonDocument::fromJson (replyData.toLocal8Bit(), &error);
//        if (error.error != QJsonParseError::NoError) {
//            qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
//            sync();
//            return;
//        }
//        QJsonObject obj = doc.object();
//        int ret = obj.value("errno").toInt(-1);
//        if (ret != 0) {
//            qDebug()<<Q_FUNC_INFO<<"Error number "<<ret;
//            sync();
//            emit requestFailure();
//            return;
//        }
        QJsonObject obj = parseToJsonObject(replyData);
        if (obj.isEmpty()) {
            sync();
            emit requestFailure();
            return;
        }
        QJsonArray array = obj.value("list").toArray();
        foreach (QJsonValue v, array) {
            QJsonObject o = v.toObject();
            QVariantMap map;
            foreach (QString key, o.keys()) {
                QJsonValue v = o.value(key);
                if (v.isBool()) {
                    bool ret = v.toBool();
                    map.insert(key, ret ? "1" : "0");
                } else if (v.isDouble()) {
                    map.insert(key, QString::number(v.toDouble(), 'f', 0));
                } else if (v.isString()) {
                    map.insert(key, v.toString());
                } else if (v.isObject()) {
                    map.insert(key, v.toObject().toVariantMap());
                } else if (v.isArray()) {
                    map.insert(key, v.toArray());
                }
            }
            m_dataList.append(map);
        }
        sync();
    }
    emit finishRequest();
}

void BDiskDirListDelegate::setCurrentPathList(const QStringList &currentPathList)
{
    if (m_currentPathList == currentPathList)
        return;

    m_currentPathList = currentPathList;
    emit currentPathListChanged(currentPathList);
}

void BDiskDirListDelegate::setCurrentPath(const QString &currentPath)
{
    if (m_currentPath == currentPath)
        return;

    m_currentPath = currentPath;
    emit currentPathChanged(currentPath);

    QStringList list;
    if (m_currentPath == "/") {
        list.append("/");
        setCurrentPathList(list);
        return;
    }
    QString str = m_currentPath;
    /// remove first /
    str = str.right(str.length() -1);
    list = str.split("/");
    list.prepend("/");
    setCurrentPathList(list);
}

void BDiskDirListDelegate::sync()
{
    emit dirListChanged(m_dataList);
}

void BDiskDirListDelegate::showCategory(int categoryType, int page)
{
    if (page < 1)
        return;
    if (m_categoryList && !m_categoryList->isFinished())
        m_categoryList->abort();
    if (!m_categoryList) {
        m_categoryList = new BDiskActionCategoryList(this);
        connect(m_categoryList, &BDiskBaseRequest::requestStarted, this, &BDiskDirListDelegate::startRequest);
        connect(m_categoryList, &BDiskBaseRequest::requestResult, this, &BDiskDirListDelegate::handleCategoryList);
    }

    m_categoryList->operationPtr()->setParameters("category", QString::number(categoryType));
    m_categoryList->operationPtr()->setParameters("page", QString::number(page));
    m_categoryList->request();
}

QJsonObject BDiskDirListDelegate::parseToJsonObject(const QString &source) const
{
    if (source.isEmpty())
        return QJsonObject();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson (source.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
        return QJsonObject();
    }
    QJsonObject obj = doc.object();
    int ret = obj.value("errno").toInt(-1);
    if (ret != 0) {
        qDebug()<<Q_FUNC_INFO<<"Error number "<<ret;
//        sync();
//        emit requestFailure();
        return QJsonObject();
    }
    return obj;
}





