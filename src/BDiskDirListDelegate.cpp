#include "BDiskDirListDelegate.h"

#include <QDebug>
#include <QJsonParseError>
#include <QJsonDocument>

#include "qsdiffrunner.h"

#include "BDiskRequest/BDiskBaseRequest.h"
#include "BDiskRequest/BDiskFileObjectKeys.h"

BDiskDirListDelegate::BDiskDirListDelegate(QObject *parent)
    : QSListModel(parent)
    , m_currentPath(QString("/"))
{
    m_action = new BDiskActionListDir(this);
    connect(m_action, &BDiskBaseRequest::requestStarted, this, &BDiskDirListDelegate::startRequest);
    connect(m_action, &BDiskBaseRequest::requestResult,
            [&](BDiskBaseRequest::RequestRet ret, const QString &replyData){
        qDebug()<<Q_FUNC_INFO<<" ret "<<ret;
        m_dataList.clear();
        if (ret == BDiskBaseRequest::RET_SUCCESS) {
            qDebug()<<Q_FUNC_INFO<<">>>>>> ok";
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson (replyData.toLocal8Bit(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
                sync();
                return;
            }
            QJsonObject obj = doc.object();
            int ret = obj.value("errno").toInt(-1);
            if (ret != 0) {
                sync();
                return;
            }
            QJsonArray array = obj.value("list").toArray();
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
                m_dataList.append(map);
            }
            sync();
        }
        emit finishRequest();
    });
}

BDiskDirListDelegate::~BDiskDirListDelegate()
{
    if (m_action)
        m_action->deleteLater();
    m_action = nullptr;
}

void BDiskDirListDelegate::showRoot()
{
    m_action->request();
    setCurrentPath("/");
}

void BDiskDirListDelegate::show(const QString &dir)
{
    m_action->setParameters("dir", dir);
    m_action->request();
    setCurrentPath(dir);
}

void BDiskDirListDelegate::cdup()
{
    qDebug()<<Q_FUNC_INFO<<" cdup for "<<m_currentPath;
    if (!m_currentPath.contains("/"))
        return;
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
    m_action->setParameters("dir", str);
    m_action->request();
    setCurrentPath(str);
}

void BDiskDirListDelegate::refresh()
{
    m_action->request();
}

QString BDiskDirListDelegate::currentPath() const
{
    return m_currentPath;
}

void BDiskDirListDelegate::setCurrentPath(const QString &currentPath)
{
    if (m_currentPath == currentPath)
        return;

    m_currentPath = currentPath;
    emit currentPathChanged(currentPath);
}

void BDiskDirListDelegate::sync()
{
    QSDiffRunner runner;
    runner.setKeyField(BDISK_FILE_KEY_FS_ID);

    qDebug()<<">>> m_dataList size "<<m_dataList.size();

    QList<QSPatch> patches = runner.compare(this->storage(), m_dataList);

//    qDebug()<<Q_FUNC_INFO<<"patch size "<<patches.size();
//    foreach (QSPatch p, patches) {
//        qDebug()<<p;
//    }
    this->clear();

    runner.patch(this, patches);

    qDebug()<<Q_FUNC_INFO<<"Data size "<<this->storage().size();
}





