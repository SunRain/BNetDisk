#include "BDiskTokenProvider.h"

BDiskTokenProvider::BDiskTokenProvider(QObject *parent)
    : QObject(parent)
    , m_codeString(QString())
    , m_token(QString())
    , m_pubkey(QString())
    , m_key(QString())
    , m_bdstoken(QString())
    , m_uidStr(QString())
{

}

QString BDiskTokenProvider::codeString() const
{
    return m_codeString;
}

void BDiskTokenProvider::setCodeString(const QString &codeString)
{
    m_codeString = codeString;
}

QString BDiskTokenProvider::token() const
{
    return m_token;
}

void BDiskTokenProvider::setToken(const QString &token)
{
    m_token = token;
}

QString BDiskTokenProvider::pubkey() const
{
    return m_pubkey;
}

void BDiskTokenProvider::setPubkey(const QString &pubkey)
{
    m_pubkey = pubkey;
}

QString BDiskTokenProvider::key() const
{
    return m_key;
}

void BDiskTokenProvider::setKey(const QString &key)
{
    m_key = key;
}

QString BDiskTokenProvider::bdstoken() const
{
    return m_bdstoken;
}

void BDiskTokenProvider::setBdstoken(const QString &bdstoken)
{
    m_bdstoken = bdstoken;
}

QString BDiskTokenProvider::uidStr() const
{
    return m_uidStr;
}

void BDiskTokenProvider::setUidStr(const QString &uidStr)
{
    m_uidStr = uidStr;
}
