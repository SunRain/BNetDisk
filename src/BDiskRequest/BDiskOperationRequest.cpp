#include "BDiskOperationRequest.h"

#include <QSharedData>
#include <QHash>
#include <QDebug>
#include <QUrlQuery>

#include "BDiskConst.h"

class BDiskBaseOperationRequestPriv : public QSharedData
{
public:
    BDiskBaseOperationRequestPriv()
        : baseUrl(QString(BDISK_URL_PAN_API))
        , urlPath(QString())
        , op(BDiskBaseOperationRequest::OPERATION_UNDEFINED)
        , isInitiated(false)
    {
    }
    QString baseUrl;
    QString urlPath;
    BDiskBaseOperationRequest::OperationType op;
    bool isInitiated;
    QHash<QString, QString> parameters;
    QHash<QString, QString> postDataParameters;
};


BDiskBaseOperationRequest::BDiskBaseOperationRequest()
    : d(new BDiskBaseOperationRequestPriv())
{
}

BDiskBaseOperationRequest::BDiskBaseOperationRequest(const BDiskBaseOperationRequest &other)
    : d(other.d)
{
}

BDiskBaseOperationRequest::~BDiskBaseOperationRequest()
{

}

void BDiskBaseOperationRequest::initiate()
{
    d.data()->op = getOp();
    initParameters();
    d.data()->isInitiated = true;
}

void BDiskBaseOperationRequest::setBaseUrl(const QString &url)
{
    d.data()->baseUrl = url;
}

QUrl BDiskBaseOperationRequest::initUrl() const
{
    if (!d.data()->isInitiated)
        qWarning()<<Q_FUNC_INFO<<"Please call initiate first!!!";

    QString str = d.data()->baseUrl;
    if (!d.data()->urlPath.isEmpty()) {
        if (d.data()->urlPath.startsWith("/"))
           str = QString("%1%2").arg(d.data()->baseUrl).arg (d.data()->urlPath);
        else
            str = QString("%1/%2").arg(d.data()->baseUrl).arg (d.data()->urlPath);
    }
    QUrl url(str);
    if (!d.data()->parameters.isEmpty()) {
        QUrlQuery query;
        QHash<QString, QString>::const_iterator it = d.data()->parameters.constBegin ();
        for (; it != d.data()->parameters.constEnd (); ++it) {
             QString value = it.value ();
             query.addQueryItem (it.key (), value.trimmed ());
        }
        url.setQuery(query);
    }
    return url;
}

bool BDiskBaseOperationRequest::operator ==(const BDiskBaseOperationRequest &other)
{
    return d.data()->baseUrl == other.d.data()->baseUrl
            && d.data()->urlPath == other.d.data()->urlPath
            && d.data()->op == other.d.data()->op
            && d.data()->isInitiated == d.data()->isInitiated;
    //FIXME compare parameters?
}

bool BDiskBaseOperationRequest::operator !=(const BDiskBaseOperationRequest &other) {
    return !operator ==(other);
}

BDiskBaseOperationRequest &BDiskBaseOperationRequest::operator =(const BDiskBaseOperationRequest &other) {
    if (this != &other) {
        d.operator =(other.d);
    }
    return *this;
}

BDiskBaseOperationRequest &BDiskBaseOperationRequest::operator()(const QString &key, const QString &value)
{
    d.data()->parameters.insert(key, value);
    return *this;
}

BDiskBaseOperationRequest::OperationType BDiskBaseOperationRequest::getOp()
{
    return BDiskBaseOperationRequest::OPERATION_UNDEFINED;
}

BDiskBaseOperationRequest::OperationType BDiskBaseOperationRequest::op()
{
    if (!d.data()->isInitiated)
        qWarning()<<Q_FUNC_INFO<<"Please call initiate first!!!";
    return d.data()->op;
}

void BDiskBaseOperationRequest::setParameters(const QString &key, const QString &value)
{
    if (d.data()->parameters.contains(key)) {
        d.data()->parameters.insert(key, value);
    }
}

void BDiskBaseOperationRequest::appendPostDataParameters(const QString &key, const QString &value)
{
    if (key.isEmpty())
        return;
    d.data()->postDataParameters.insert(key.simplified(), value.simplified());
}

QHash<QString, QString> BDiskBaseOperationRequest::postDataParameters() const {
    return d.data()->postDataParameters;
}

QString BDiskBaseOperationRequest::postDataParameter(const QString &key, const QString &defaultValue) const {
    if (!d.data()->postDataParameters.isEmpty() && d.data()->postDataParameters.contains(key))
        return d.data()->postDataParameters.value(key, defaultValue);
    return QString();
}

void BDiskBaseOperationRequest::initParameters()
{
    qWarning()<<Q_FUNC_INFO<<"Empty parameters!";
}

void BDiskBaseOperationRequest::setUrlPath(const QString &path, const QString &tag)
{
    if (!path.isEmpty()) {
        if (tag.isEmpty())
            d.data()->urlPath = path;
        else
            d.data()->urlPath = QString("%1%2").arg(path).arg(tag);
    }
}


