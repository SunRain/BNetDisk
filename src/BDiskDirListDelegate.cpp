#include "BDiskDirListDelegate.h"

#include <QDebug>
#include <QJsonParseError>
#include <QJsonDocument>

#include "qsdiffrunner.h"

#include "BDiskRequest/BDiskBaseRequest.h"
#include "BDiskRequest/BDiskFileObjectKeys.h"

BDiskDirListDelegate::BDiskDirListDelegate(QObject *parent)
    : QSListModel(parent)
{
    m_action = new BDiskActionListDir(this);
    connect(m_action, &BDiskBaseRequest::requestStarted, this, &BDiskDirListDelegate::startRequest);
    connect(m_action, &BDiskBaseRequest::requestResult,
            [&](BDiskBaseRequest::RequestRet ret, const QString &replyData){
        qDebug()<<Q_FUNC_INFO<<" ret "<<ret;
        if (ret == BDiskBaseRequest::RET_SUCCESS) {
            qDebug()<<Q_FUNC_INFO<<">>>>>> ok";
            m_dataList.clear();

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
                QVariant d = v.toVariant();
                qDebug()<<Q_FUNC_INFO<<" >>>>>> QVariant is "<<d;
                m_dataList.append(d);
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
}

void BDiskDirListDelegate::show(const QString &dir)
{
}

void BDiskDirListDelegate::sync()
{
    QSDiffRunner runner;
    runner.setKeyField(BDISK_FILE_KEY_FS_ID);

    QList<QSPatch> patches = runner.compare(this->storage(), m_dataList);
    runner.patch(this, patches);

    qDebug()<<Q_FUNC_INFO<<"Data size "<<this->storage().size();
}





