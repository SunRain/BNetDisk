#ifndef BDISKFILEOBJECTKEYS_H
#define BDISKFILEOBJECTKEYS_H

#include <QString>
#include <QObject>
#include <QJSEngine>
#include <QQmlEngine>

const static QString BDISK_FILE_KEY_SIZE ("size");
const static QString BDISK_FILE_KEY_CATEGORY ("category");
const static QString BDISK_FILE_KEY_UNLIST ("unlist");
const static QString BDISK_FILE_KEY_IS_DIR ("isdir");
const static QString BDISK_FILE_KEY_DIR_EMPTY ("dir_empty");
const static QString BDISK_FILE_KEY_EMPTY ("empty");
const static QString BDISK_FILE_KEY_FS_ID ("fs_id");
const static QString BDISK_FILE_KEY_SERVER_MTIME ("server_mtime");
const static QString BDISK_FILE_KEY_OPER_ID ("oper_id");
const static QString BDISK_FILE_KEY_SERVER_CTIME ("server_ctime");
const static QString BDISK_FILE_KEY_LOCAL_MTIME ("local_mtime");
const static QString BDISK_FILE_KEY_MD5 ("md5");
const static QString BDISK_FILE_KEY_PATH ("path");
const static QString BDISK_FILE_KEY_LOCAL_CTIME ("local_ctime");
const static QString BDISK_FILE_KEY_SERVER_FILENAME ("server_filename");


class BDiskFileObjectKeyName : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString keySize READ keySize CONSTANT)
    Q_PROPERTY(QString keyCategory READ keyCategory CONSTANT)
    Q_PROPERTY(QString keyUnlist READ keyUnlist CONSTANT)
    Q_PROPERTY(QString keyIsdir READ keyIsdir CONSTANT)
    Q_PROPERTY(QString keyDirEmpty READ keyDirEmpty CONSTANT)
    Q_PROPERTY(QString keyEmpty READ keyEmpty CONSTANT)
    Q_PROPERTY(QString keyFsId READ keyFsId CONSTANT)
    Q_PROPERTY(QString keyServerMTime READ keyServerMTime CONSTANT)
    Q_PROPERTY(QString keyOperId READ keyOperId CONSTANT)
    Q_PROPERTY(QString keyServerCTime READ keyServerCTime CONSTANT)
    Q_PROPERTY(QString keyLocalMTime READ keyLocalMTime CONSTANT)
    Q_PROPERTY(QString keyMd5 READ keyMd5 CONSTANT)
    Q_PROPERTY(QString keyPath READ keyPath CONSTANT)
    Q_PROPERTY(QString keyLocalCTime READ keyLocalCTime CONSTANT)
    Q_PROPERTY(QString keyServerFilename READ keyServerFilename CONSTANT)

public:
    BDiskFileObjectKeyName(QObject *parent = 0)
        : QObject(parent)
    {
    }
    virtual ~BDiskFileObjectKeyName() {}

    static QObject *qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new BDiskFileObjectKeyName();
    }

    QString keySize() const
    {
        return BDISK_FILE_KEY_SIZE;
    }
    QString keyCategory() const
    {
        return BDISK_FILE_KEY_CATEGORY;
    }
    QString keyUnlist() const
    {
        return BDISK_FILE_KEY_UNLIST;
    }
    QString keyIsdir() const
    {
        return BDISK_FILE_KEY_IS_DIR;
    }
    QString keyDirEmpty() const
    {
        return BDISK_FILE_KEY_DIR_EMPTY;
    }
    QString keyEmpty() const
    {
        return BDISK_FILE_KEY_EMPTY;
    }
    QString keyFsId() const
    {
        return BDISK_FILE_KEY_FS_ID;
    }
    QString keyServerMTime() const
    {
        return BDISK_FILE_KEY_SERVER_MTIME;
    }
    QString keyOperId() const
    {
        return BDISK_FILE_KEY_OPER_ID;
    }
    QString keyServerCTime() const
    {
        return BDISK_FILE_KEY_SERVER_CTIME;
    }
    QString keyLocalMTime() const
    {
        return BDISK_FILE_KEY_LOCAL_MTIME;
    }
    QString keyMd5() const
    {
        return BDISK_FILE_KEY_MD5;
    }
    QString keyPath() const
    {
        return BDISK_FILE_KEY_PATH;
    }
    QString keyLocalCTime() const
    {
        return BDISK_FILE_KEY_LOCAL_CTIME;
    }
    QString keyServerFilename() const
    {
        return BDISK_FILE_KEY_SERVER_FILENAME;
    }
};

#endif // BDISKFILEOBJECTKEYS_H
