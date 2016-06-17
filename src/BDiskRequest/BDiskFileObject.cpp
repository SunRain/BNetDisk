#include "BDiskFileObject.h"

#include <QSharedData>
#include <QJsonObject>

#include "BDiskFileObjectKeys.h"

class BDiskFileObjectPriv : public QSharedData
{
public:
    BDiskFileObjectPriv() {
         size = -1;
         category = -1;
         unlist = -1;
         isdir = false;
         dir_empty = false;
         empty = false;
         fs_id = QString();
         server_mtime = QString();
         oper_id = QString();
         server_ctime = QString();
         local_mtime = QString();
         md5 = QString();
         path = QString();
         local_ctime = QString();
         server_filename = QString();
    }
    virtual ~BDiskFileObjectPriv(){}
    //TODO more paramaters
    int size;// 35321,
    int category; //3,
    int unlist; //0
    bool isdir;// 0,
    bool dir_empty; //0
    bool empty; //0
    QString fs_id; //1033103909397405,
    QString server_mtime; //1463392169,
    QString oper_id;// 0,
    QString server_ctime;// 1441012455,
    QString local_mtime;// 1440599246,
    // thumbs; {
    //  icon;
    //  url3;
    //  url2;
    //  url1;
    // },
    QString md5;// 21e8d266892d1b95a36756febdefc342,
    QString path;// /临时图片/mmexport1440599246959.jpg,
    QString local_ctime;// 1440599246,
    QString server_filename;// mmexport1440599246959.jpg"
};

BDiskFileObject::BDiskFileObject()
    : d(new BDiskFileObjectPriv())
{

}

BDiskFileObject::BDiskFileObject(const BDiskFileObject &other)
    : d(other.d)
{

}

bool BDiskFileObject::operator ==(const BDiskFileObject &other)
{
    return d.data()->category == other.d.data()->category
            && d.data()->dir_empty == other.d.data()->dir_empty
            && d.data()->empty == other.d.data()->empty
            && d.data()->fs_id == other.d.data()->fs_id
            && d.data()->isdir == other.d.data()->isdir
            && d.data()->local_ctime == other.d.data()->local_ctime
            && d.data()->local_mtime == other.d.data()->local_mtime
            && d.data()->md5 == other.d.data()->md5
            && d.data()->oper_id == other.d.data()->oper_id
            && d.data()->path == other.d.data()->path
            && d.data()->server_ctime == other.d.data()->server_ctime
            && d.data()->server_filename == other.d.data()->server_filename
            && d.data()->server_mtime == other.d.data()->server_mtime
            && d.data()->size == other.d.data()->size
            && d.data()->unlist == other.d.data()->unlist;
}

bool BDiskFileObject::operator !=(const BDiskFileObject &other)
{
    return !operator ==(other);
}

BDiskFileObject &BDiskFileObject::operator =(const BDiskFileObject &other)
{
    if (this != &other)
        d.operator =(other.d);
    return *this;
}

int BDiskFileObject::size() const
{
    return d.data()->size;
}

void BDiskFileObject::setSize(int value)
{
    d.data()->size = value;
}

int BDiskFileObject::category() const
{
    return d.data()->category;
}

void BDiskFileObject::setCategory(int value)
{
    d.data()->category = value;
}

int BDiskFileObject::unlist() const
{
    return d.data()->unlist;
}

void BDiskFileObject::setUnlist(int value)
{
    d.data()->unlist = value;
}

bool BDiskFileObject::isdir() const
{
    return d.data()->isdir;
}

void BDiskFileObject::setIsdir(bool value)
{
    d.data()->isdir = value;
}

bool BDiskFileObject::dirEmpty() const
{
    return d.data()->dir_empty;
}

void BDiskFileObject::setDirEmpty(bool value)
{
    d.data()->dir_empty = value;
}

bool BDiskFileObject::empty() const
{
    return d.data()->empty;
}

void BDiskFileObject::setEmpty(bool value)
{
    d.data()->empty = value;
}

QString BDiskFileObject::fsId() const
{
    return d.data()->fs_id;
}

void BDiskFileObject::setFsId(const QString &value)
{
    d.data()->fs_id = value;
}

QString BDiskFileObject::serverMtime() const
{
    return d.data()->server_mtime;
}

void BDiskFileObject::setServerMtime(const QString &value)
{
    d.data()->server_mtime = value;
}

QString BDiskFileObject::operId() const
{
    return d.data()->oper_id;
}

void BDiskFileObject::setOperId(const QString &value)
{
    d.data()->oper_id = value;
}

QString BDiskFileObject::serverCtime() const
{
    return d.data()->server_ctime;
}

void BDiskFileObject::setServerCtime(const QString &value)
{
    d.data()->server_ctime = value;
}

QString BDiskFileObject::localMtime() const
{
    return d.data()->local_mtime;
}

void BDiskFileObject::setLocalMtime(const QString &value)
{
    d.data()->local_mtime = value;
}

QString BDiskFileObject::md5() const
{
    return d.data()->md5;
}

void BDiskFileObject::setMd5(const QString &value)
{
    d.data()->md5 = value;
}

QString BDiskFileObject::path() const
{
    return d.data()->path;
}

void BDiskFileObject::setPath(const QString &value)
{
    d.data()->path = value;
}

QString BDiskFileObject::localCtime() const
{
    return d.data()->local_ctime;
}

void BDiskFileObject::setLocalCtime(const QString &value)
{
    d.data()->local_ctime = value;
}

QString BDiskFileObject::serverFilename() const
{
    return d.data()->server_filename;
}

void BDiskFileObject::setServerFilename(const QString &value)
{
    d.data()->server_filename = value;
}

QJsonObject BDiskFileObject::toObject() const
{
    QJsonObject o;
    o.insert(BDISK_FILE_KEY_SIZE, d.data()->size);
    o.insert(BDISK_FILE_KEY_CATEGORY, d.data()->category);
    o.insert(BDISK_FILE_KEY_UNLIST, d.data()->unlist);
    o.insert(BDISK_FILE_KEY_IS_DIR, d.data()->isdir);
    o.insert(BDISK_FILE_KEY_DIR_EMPTY, d.data()->dir_empty);
    o.insert(BDISK_FILE_KEY_EMPTY, d.data()->empty);
    o.insert(BDISK_FILE_KEY_FS_ID, d.data()->fs_id);
    o.insert(BDISK_FILE_KEY_SERVER_MTIME, d.data()->server_mtime);
    o.insert(BDISK_FILE_KEY_OPER_ID, d.data()->oper_id);
    o.insert(BDISK_FILE_KEY_SERVER_CTIME, d.data()->server_ctime);
    o.insert(BDISK_FILE_KEY_LOCAL_MTIME, d.data()->local_mtime);
    o.insert(BDISK_FILE_KEY_MD5, d.data()->md5);
    o.insert(BDISK_FILE_KEY_PATH, d.data()->path);
    o.insert(BDISK_FILE_KEY_LOCAL_CTIME, d.data()->local_ctime);
    o.insert(BDISK_FILE_KEY_SERVER_FILENAME, d.data()->server_filename);
    return o;
}

QVariantMap BDiskFileObject::toMap() const
{
    QVariantMap o;
    o.insert(BDISK_FILE_KEY_SIZE, d.data()->size);
    o.insert(BDISK_FILE_KEY_CATEGORY, d.data()->category);
    o.insert(BDISK_FILE_KEY_UNLIST, d.data()->unlist);
    o.insert(BDISK_FILE_KEY_IS_DIR, d.data()->isdir);
    o.insert(BDISK_FILE_KEY_DIR_EMPTY, d.data()->dir_empty);
    o.insert(BDISK_FILE_KEY_EMPTY, d.data()->empty);
    o.insert(BDISK_FILE_KEY_FS_ID, d.data()->fs_id);
    o.insert(BDISK_FILE_KEY_SERVER_MTIME, d.data()->server_mtime);
    o.insert(BDISK_FILE_KEY_OPER_ID, d.data()->oper_id);
    o.insert(BDISK_FILE_KEY_SERVER_CTIME, d.data()->server_ctime);
    o.insert(BDISK_FILE_KEY_LOCAL_MTIME, d.data()->local_mtime);
    o.insert(BDISK_FILE_KEY_MD5, d.data()->md5);
    o.insert(BDISK_FILE_KEY_PATH, d.data()->path);
    o.insert(BDISK_FILE_KEY_LOCAL_CTIME, d.data()->local_ctime);
    o.insert(BDISK_FILE_KEY_SERVER_FILENAME, d.data()->server_filename);
    return o;
}


