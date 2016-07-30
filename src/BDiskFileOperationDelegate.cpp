#include "BDiskFileOperationDelegate.h"

#include <QJsonDocument>
#include <QJsonObject>

BDiskFileOperationDelegate::BDiskFileOperationDelegate(QObject *parent)
    : QObject(parent)
    , m_fileRename(nullptr)
{

}

BDiskFileOperationDelegate::~BDiskFileOperationDelegate()
{
    if (m_fileRename) {
        if (!m_fileRename->isFinished())
            m_fileRename->abort();
        m_fileRename->deleteLater();
        m_fileRename = nullptr;
    }
}

void BDiskFileOperationDelegate::rename(const QString &path, const QString &newName)
{
    if (path.isEmpty() || newName.isEmpty())
        return;
    if (m_fileRename) {
        if (!m_fileRename->isFinished())
            m_fileRename->abort();
        QObject::disconnect(m_fileRename, 0, 0, 0);
        m_fileRename->deleteLater();
        m_fileRename = nullptr;
    }
    m_fileRename = new BDiskActionFileRename();
//    filelist:"[{"path":"/Material+Design/linux软件/xware-desktop.tar.gz","newname":"xware-desktop-aa.tar.gz"}]"
    QJsonObject o;
    o.insert("path", path);
    o.insert("newname", newName);
    QJsonDocument doc(o);
    QByteArray array = doc.toJson(QJsonDocument::Compact);
    qDebug()<<Q_FUNC_INFO<<" ================= "<<array;
    m_fileRename->operationPtr()->appendPostDataParameters("filelist", QString("[%1]").arg(QString(array)));
    connect(m_fileRename, &BDiskActionFileRename::requestStarted, [&] {
        emit startRequest(OperationType::OPERATION_RENAME);
    });
    connect(m_fileRename, &BDiskActionFileRename::requestResult,
            [&] (BDiskBaseRequest::RequestRet ret, const QString &replyData) {
        if (ret != BDiskBaseRequest::RET_SUCCESS) {
            emit requestFailure(OperationType::OPERATION_RENAME);
            return;
        }
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(replyData.toLocal8Bit(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
            emit requestFailure(OperationType::OPERATION_RENAME);
            return;
        }
        QJsonObject obj = doc.object();
        int err = obj.value("errno").toInt(-1);
        if (err != 0) {
            qDebug()<<Q_FUNC_INFO<<"Error number "<<err;
            emit requestFailure(OperationType::OPERATION_RENAME);
            return;
        }
        emit finishRequest(OperationType::OPERATION_RENAME);
        emit requestSuccess(OperationType::OPERATION_RENAME);
    });
    m_fileRename->request();
}
