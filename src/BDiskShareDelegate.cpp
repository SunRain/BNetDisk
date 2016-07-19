#include "BDiskShareDelegate.h"

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

BDiskShareDelegate::BDiskShareDelegate(QObject *parent)
    : QObject(parent)
    , m_privShare(nullptr)
    , m_pubShare(nullptr)
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

    connect(m_privShare, &BDiskActionPrivShare::requestStarted,
            this, &BDiskShareDelegate::startRequest);

    connect(m_pubShare, &BDiskActionPrivShare::requestResult,
            [&](BDiskBaseRequest::RequestRet ret, const QString &replyData) {
        parseReply(ret, replyData, false);
    });

    m_pubShare->request();
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
    int ret = obj.value("errno").toInt(-1);
    if (ret != 0) {
        qDebug()<<Q_FUNC_INFO<<"Error number "<<ret;
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
