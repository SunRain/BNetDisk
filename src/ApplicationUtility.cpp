#include "ApplicationUtility.h"

#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QClipboard>

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

QString ApplicationUtility::milliSecsToStr(qint64 millseconds)
{
    if (millseconds <= 0) {
        return QString("0s");
    } else if (millseconds > 0 && millseconds <1000) {
        return QString("%1s").arg(((double)millseconds)/1000, 0, 'f', 2);
    } else if (millseconds >= 1000 && millseconds <60*1000) {
        return QString("%1s").arg(millseconds/1000);
    } else if (millseconds >= 60*1000 && millseconds<60*60*1000) {
        qint64 m = millseconds / (60*1000);
        qint64 s = (millseconds%(60*1000))/1000;
        return QString("%1m%2s").arg(m).arg(s);
    } else {
        qint64 h = millseconds / (60*60*1000);
        qint64 ret = millseconds - h*60*60*1000;
        qint64 m = ret / (60*1000);
        ret = ret - m*60*1000;
        qint64 s = ret/1000;
        return QString("%1h%2m%3s").arg(h).arg(m).arg(s);
    }
}

void ApplicationUtility::copyToClipboard(const QString &text)
{
    if (text.isEmpty())
        return;
    QGuiApplication::clipboard()->setText(text);
}
