#ifndef BDISKTOKENPROVIDER_H
#define BDISKTOKENPROVIDER_H

#include <QObject>
#include "SingletonPointer.h"

class QSettings;
class BDiskTokenProvider : public QObject
{
    Q_OBJECT

    DECLARE_SINGLETON_POINTER(BDiskTokenProvider)
public:
    virtual ~BDiskTokenProvider();

    QString codeString() const;
    void setCodeString(const QString &codeString);

    QString token() const;
    void setToken(const QString &token);

    QString pubkey() const;
    void setPubkey(const QString &pubkey);

    QString key() const;
    void setKey(const QString &key);

    QString bdstoken() const;
    void setBdstoken(const QString &bdstoken);

    QString uidStr() const;
    void setUidStr(const QString &uidStr);

    void clear();

private:
    QSettings *m_settings;
    QString m_codeString;
    QString m_token;
    QString m_pubkey;
    QString m_key;
    QString m_bdstoken;
    QString m_uidStr;

};

#endif // BDISKTOKENPROVIDER_H
