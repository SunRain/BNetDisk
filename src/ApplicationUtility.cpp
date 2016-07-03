#include "ApplicationUtility.h"

#include <QDateTime>
#include <QDebug>

ApplicationUtility::ApplicationUtility(QObject *parent)
    : QObject(parent)
{

}

QString ApplicationUtility::fileObjectPathToPath(const QString &path)
{
    if (path.isEmpty())
        return QString();
    QString str = path;
    if (str.startsWith("/"))
        str = str.right(str.length() -1);
    if (!str.contains("/"))
        return "/";
    QStringList list = str.split("/");
    if (list.isEmpty())
        return QString();
    list.removeLast();
    return list.join("/");
}

QString ApplicationUtility::fileObjectPathToFileName(const QString &path)
{
    if (path.isEmpty())
        return QString();
    QString str = path;
    if (str.startsWith("/"))
        str = str.right(str.length() -1);
    if (!str.contains("/"))
        return str;
    QStringList list = str.split("/");
    if (list.isEmpty())
        return QString();
    return list.last();
}

QString ApplicationUtility::sizeToStr(int size)
{
    if (size < 1024)
        return QString("%1 B").arg(size);
    if (size >= 1024 && size < 1024*1024)
        return QString("%1 KB").arg(((double)size)/1024, 0, 'f', 2);
    if (size >= 1024*1024 && size < 1024*1024*1024)
        return QString("%1 MB").arg(((double)size)/1024/1024, 0, 'f', 2);
    return QString("%1 GB").arg(((double)size)/1024/1024/1024, 0, 'f', 2);
}

QString ApplicationUtility::formatDate(int date)
{
    QDateTime dt = QDateTime::fromTime_t(date, Qt::OffsetFromUTC);
    if (!dt.isValid())
        return QString::number(date);
    return dt.toString("yyyy/MM/dd HH:mm");
}
