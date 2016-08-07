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
const static QString BDISK_FILE_KEY_OBJECT_KEY ("object_key");
const static QString BDISK_FILE_KEY_THUBMS ("thumbs");
const static QString BDISK_FILE_KEY_THUBMS_ICON ("icon");
const static QString BDISK_FILE_KEY_THUBMS_SMALL ("url1");
const static QString BDISK_FILE_KEY_THUBMS_MIDDLE ("url2");
const static QString BDISK_FILE_KEY_THUBMS_LARGE ("url3");
const static QString BDISK_FILE_KEY_LEFTTIME ("leftTime");

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
    Q_PROPERTY(QString keyFileObjKey READ keyFileObjKey CONSTANT)
    Q_PROPERTY(QString keyThumbs READ keyThumbs CONSTANT)
    Q_PROPERTY(QString keyThumbsIcon READ keyThumbsIcon CONSTANT)
    Q_PROPERTY(QString keyThumbsSmall READ keyThumbsSmall CONSTANT)
    Q_PROPERTY(QString keyThumbsMiddle READ keyThumbsMiddle CONSTANT)
    Q_PROPERTY(QString keyThumbsLarge READ keyThumbsLarge CONSTANT)
    Q_PROPERTY(QString keyLeftTime READ keyLeftTime CONSTANT)

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

    static QStringList keys() {
        QStringList list;
        list << BDISK_FILE_KEY_SIZE
                << BDISK_FILE_KEY_CATEGORY
                << BDISK_FILE_KEY_UNLIST
                << BDISK_FILE_KEY_IS_DIR
                << BDISK_FILE_KEY_DIR_EMPTY
                << BDISK_FILE_KEY_EMPTY
                << BDISK_FILE_KEY_FS_ID
                << BDISK_FILE_KEY_SERVER_MTIME
                << BDISK_FILE_KEY_OPER_ID
                << BDISK_FILE_KEY_SERVER_CTIME
                << BDISK_FILE_KEY_LOCAL_MTIME
                << BDISK_FILE_KEY_MD5
                << BDISK_FILE_KEY_PATH
                << BDISK_FILE_KEY_LOCAL_CTIME
                << BDISK_FILE_KEY_SERVER_FILENAME
                << BDISK_FILE_KEY_OBJECT_KEY
                << BDISK_FILE_KEY_THUBMS
                << BDISK_FILE_KEY_THUBMS_ICON
                << BDISK_FILE_KEY_THUBMS_LARGE
                << BDISK_FILE_KEY_THUBMS_MIDDLE
                << BDISK_FILE_KEY_THUBMS_SMALL
                << BDISK_FILE_KEY_LEFTTIME;
        return list;
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
    QString keyFileObjKey() const
    {
        return BDISK_FILE_KEY_OBJECT_KEY;
    }
    QString keyThumbs() const
    {
        return BDISK_FILE_KEY_THUBMS;
    }
    QString keyThumbsIcon() const
    {
        return BDISK_FILE_KEY_THUBMS_ICON;
    }
    QString keyThumbsSmall() const
    {
        return BDISK_FILE_KEY_THUBMS_SMALL;
    }
    QString keyThumbsMiddle() const
    {
        return BDISK_FILE_KEY_THUBMS_MIDDLE;
    }
    QString keyThumbsLarge() const
    {
        return BDISK_FILE_KEY_THUBMS_LARGE;
    }
    QString keyLeftTime() const
    {
        return BDISK_FILE_KEY_LEFTTIME;
    }
};

#endif // BDISKFILEOBJECTKEYS_H
