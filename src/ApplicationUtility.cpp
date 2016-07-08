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

QString ApplicationUtility::bytesPerSecond(int value)
{
    if (value <= 0) {
        return QString("0 KB/s");
    }
    if (value < 1024)
        return QString("%1 B/s").arg(value);
    if (value >= 1024 && value < 1024*1024)
        return QString("%1 KB/s").arg(((double)value)/1024, 0, 'f', 2);
    if (value >= 1024*1024 && value < 1024*1024*1024)
        return QString("%1 MB/s").arg(((double)value)/1024/1024, 0, 'f', 2);
    return QString("%1 GB/s").arg(((double)value)/1024/1024/1024, 0, 'f', 2);
}

QString ApplicationUtility::downloadPercent(int ready, int total)
{
    if (ready <= 0 || total <= 0)
        return QString("0%");
    if (ready >= total)
        return QString("100%");
    return QString("%1%").arg(((double)ready)/((double)total)*100, 0, 'f', 2);
}
