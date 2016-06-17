#ifndef BDISKFILEOBJECT_H
#define BDISKFILEOBJECT_H

#include <QSharedDataPointer>
#include <QVariantMap>
#include <QList>


class BDiskFileObject;
typedef QList<BDiskFileObject> BDiskFileObjectList;

class QJsonObject;
class BDiskFileObjectPriv;
class BDiskFileObject
{
public:
    BDiskFileObject();
    BDiskFileObject(const BDiskFileObject &other);
    bool operator == (const BDiskFileObject &other);
    bool operator != (const BDiskFileObject &other);
    BDiskFileObject &operator = (const BDiskFileObject &other);


    int size() const;
    void setSize(int value);

    int category() const;
    void setCategory(int value);

    int unlist() const;
    void setUnlist(int value);

    bool isdir() const;
    void setIsdir(bool value);


    bool dirEmpty() const;
    void setDirEmpty(bool value);

    bool empty() const;
    void setEmpty(bool value);

    QString fsId() const;
    void setFsId(const QString &value);

    QString serverMtime() const;
    void setServerMtime(const QString &value);

    QString operId() const;
    void setOperId(const QString &value);

    QString serverCtime() const;
    void setServerCtime(const QString &value);

    QString localMtime() const;
    void setLocalMtime(const QString &value);

    QString md5() const;
    void setMd5(const QString &value);

    QString path() const;
    void setPath(const QString &value);

    QString localCtime() const;
    void setLocalCtime(const QString &value);

    QString serverFilename() const;
    void setServerFilename(const QString &value);

    QJsonObject toObject() const;
    QVariantMap toMap() const;

private:
    QSharedDataPointer<BDiskFileObjectPriv> d;
};

#endif // BDISKFILEOBJECT_H
