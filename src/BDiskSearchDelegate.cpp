#include "BDiskSearchDelegate.h"

#include <QDebug>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QVariantMap>

#include "BDiskRequest/BDiskBaseRequest.h"

BDiskSearchDelegate::BDiskSearchDelegate(QObject *parent)
    : QObject(parent)
    , m_search(nullptr)
{

}

BDiskSearchDelegate::~BDiskSearchDelegate()
{
    if (m_search) {
        if (!m_search->isFinished())
            m_search->deleteLater();
        m_search = nullptr;
    }
}

void BDiskSearchDelegate::search(const QString &key)
{
    if (key.isEmpty())
        return;
    if (m_search) {
        if (!m_search->isFinished())
            m_search->abort();
    }
    if (!m_search) {
        m_search = new BDiskActionSearch(this);
        connect(m_search, &BDiskBaseRequest::requestStarted, this, &BDiskSearchDelegate::startRequest);
        connect(m_search, &BDiskBaseRequest::requestResult, this, &BDiskSearchDelegate::handleResult);
    }
    m_search->operationPtr()->setParameters("key", key);
    m_search->request();
}

QVariantList BDiskSearchDelegate::resultList() const
{
    return m_dataList;
}

void BDiskSearchDelegate::clear()
{
    m_dataList.clear();
    sync();
}

void BDiskSearchDelegate::handleResult(BDiskBaseRequest::RequestRet ret, const QString &replyData)
{
    m_dataList.clear();

    if (m_search) {
        if (!m_search->isFinished())
            m_search->abort();
        m_search->deleteLater();
        m_search = nullptr;
    }

    if (ret == BDiskBaseRequest::RET_SUCCESS) {
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

void BDiskSearchDelegate::setResultList(const QVariantList &resultList)
{
    if (m_dataList == resultList)
        return;

    m_dataList = resultList;
    emit resultListChanged(resultList);
}

void BDiskSearchDelegate::sync()
{
    emit resultListChanged(m_dataList);
}

QJsonObject BDiskSearchDelegate::parseToJsonObject(const QString &source) const
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
        return QJsonObject();
    }
    return obj;
}
