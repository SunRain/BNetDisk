#include "ApplicationUtility.h"

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
