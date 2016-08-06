#include "BDiskShareDelegate.h"

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

BDiskShareDelegate::BDiskShareDelegate(QObject *parent)
    : QObject(parent)
    , m_privShare(nullptr)
    , m_pubShare(nullptr)
    , m_shareRecord(nullptr)
{

}

BDiskShareDelegate::~BDiskShareDelegate()
{
    if (m_privShare) {
        if (!m_privShare->isFinished())
            m_privShare->abort();
        m_privShare->deleteLater();
        m_privShare = nullptr;
    }
    if (m_pubShare) {
        if (!m_pubShare->isFinished())
            m_pubShare->abort();
        m_pubShare->deleteLater();
        m_pubShare = nullptr;
    }
}

void BDiskShareDelegate::privShare(const QString &fsID)
{
    if (fsID.isEmpty())
        return;
    if (m_privShare) {
        if (!m_privShare->isFinished())
            m_privShare->abort();
        QObject::disconnect(m_privShare, 0, 0, 0);
        m_privShare->deleteLater();
        m_privShare = nullptr;
    }
    m_privShare = new BDiskActionPrivShare();
    m_privShare->operationPtr()->appendPostDataParameters("fid_list", QString("[%1]").arg(fsID));

    connect(m_privShare, &BDiskActionPrivShare::requestStarted,
            this, &BDiskShareDelegate::startRequest);
    connect(m_privShare, &BDiskActionPrivShare::requestResult,
            [&](BDiskBaseRequest::RequestRet ret, const QString &replyData) {
        parseReply(ret, replyData, true);
    });

    m_privShare->request();

}

void BDiskShareDelegate::pubShare(const QString &fsID)
{
    if (fsID.isEmpty())
        return;
    if (m_pubShare) {
        if (!m_pubShare->isFinished())
            m_pubShare->abort();
        QObject::disconnect(m_pubShare, 0, 0, 0);
        m_pubShare->deleteLater();
        m_pubShare = nullptr;
    }
    m_pubShare = new BDiskActionPubShare();
    m_pubShare->operationPtr()->appendPostDataParameters("fid_list", QString("[%1]").arg(fsID));

    connect(m_pubShare, &BDiskActionPrivShare::requestStarted,
            this, &BDiskShareDelegate::startRequest);

    connect(m_pubShare, &BDiskActionPrivShare::requestResult,
            [&](BDiskBaseRequest::RequestRet ret, const QString &replyData) {
        parseReply(ret, replyData, false);
    });

    m_pubShare->request();
}

void BDiskShareDelegate::showShareRecord(int page)
{
    if (page <= 0)
        return;
    if (m_shareRecord) {
        if (!m_shareRecord->isFinished())
            m_shareRecord->abort();
        QObject::disconnect(m_shareRecord, 0, 0, 0);
        m_shareRecord->deleteLater();
        m_shareRecord = nullptr;
    }
    m_shareRecord = new BDiskActionShareRecord();
    m_shareRecord->operationPtr()->setParameters("page", QString::number(page));

    connect(m_shareRecord, &BDiskActionPrivShare::requestStarted,
            this, &BDiskShareDelegate::startRequest);

    connect(m_shareRecord, &BDiskActionPrivShare::requestResult,
            [&](BDiskBaseRequest::RequestRet ret, const QString &replyData) {
        parseShareRecord(ret, replyData);
    });

    m_shareRecord->request();
}

QVariantList BDiskShareDelegate::shareRecords() const
{
    return m_shareRecords;
}



void BDiskShareDelegate::parseReply(BDiskBaseRequest::RequestRet ret, const QString &replyData, bool isPrivShare)
{
    if (ret != BDiskBaseRequest::RET_SUCCESS) {
        emit requestFailure();
        return;
    }
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(replyData.toLocal8Bit(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
        emit requestFailure();
        return;
    }
    QJsonObject obj = doc.object();
    int err = obj.value("errno").toInt(-1);
    if (err != 0) {
        qDebug()<<Q_FUNC_INFO<<"Error number "<<err;
        emit requestFailure();
        return;
    }
    emit finishRequest();

    QString url = obj.value("shorturl").toString();
    if (isPrivShare)
        emit privShareLink(url, "bdpw"); //"bdpw" from BDiskOpPrivShare
    else
        emit pubShareLink(url);

}

void BDiskShareDelegate::parseShareRecord(BDiskBaseRequest::RequestRet ret, const QString &replyData)
{
    if (ret != BDiskBaseRequest::RET_SUCCESS) {
        emit requestFailure();
        return;
    }
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(replyData.toLocal8Bit(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
        emit requestFailure();
        return;
    }
    QJsonObject obj = doc.object();
    int err = obj.value("errno").toInt(-1);
    if (err != 0) {
        qDebug()<<Q_FUNC_INFO<<"Error number "<<err;
        emit requestFailure();
        return;
    }

    QJsonArray array = obj.value("list").toArray();
    QVariantList list;
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
//        qDebug()<<Q_FUNC_INFO<<" append "<<map;
        list.append(map);
    }
    setShareRecords(list);
    emit finishRequest();
}

void BDiskShareDelegate::setShareRecords(const QVariantList &shareRecords)
{
    if (m_shareRecords == shareRecords)
        return;

    m_shareRecords = shareRecords;
    emit shareRecordsChanged(shareRecords);
}
