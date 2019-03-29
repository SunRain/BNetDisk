#include "BDiskTokenProvider.h"

#include <QCoreApplication>
#include <QSettings>

const static char *KEY_CODE_STRING = "BDISK_LIB/KEY_CODE_STRING";
const static char *KEY_TOKEN = "BDISK_LIB/KEY_TOKEN";
const static char *KEY_PUBKEY = "BDISK_LIB/KEY_PUBKEY";
const static char *KEY_KEY = "BDISK_LIB/KEY_KEY";
const static char *KEY_BDS_TOKEN = "BDISK_LIB/KEY_BDS_TOKEN";
const static char *KEY_UID_STR = "BDISK_LIB/KEY_UID_STR";

BDiskTokenProvider::BDiskTokenProvider(QObject *parent)
    : QObject(parent)
{
    m_settings = new QSettings(qApp->organizationName(), qApp->applicationName(), parent);
    m_codeString = QString();//m_settings->value(KEY_CODE_STRING).toString();
    m_token = m_settings->value(KEY_TOKEN).toString();
    m_pubkey = m_settings->value(KEY_PUBKEY).toString();
    m_bdstoken = m_settings->value(KEY_BDS_TOKEN).toString();
    m_uidStr = m_settings->value(KEY_UID_STR).toString();
}

BDiskTokenProvider::~BDiskTokenProvider()
{
    if (m_settings) {
//        m_settings->setValue(KEY_CODE_STRING, m_codeString);
        m_settings->setValue(KEY_TOKEN, m_token);
        m_settings->setValue(KEY_PUBKEY, m_pubkey);
        m_settings->setValue(KEY_BDS_TOKEN, m_bdstoken);
        m_settings->setValue(KEY_UID_STR, m_uidStr);
        m_settings->sync();
        m_settings->deleteLater();
    }
    m_settings = nullptr;
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

void BDiskTokenProvider::clear()
{
    m_codeString = QString();
    m_token = QString();
    m_pubkey = QString();
    m_bdstoken = QString();
    m_uidStr = QString();
    flush();
}

void BDiskTokenProvider::flush()
{
//    m_settings->setValue(KEY_CODE_STRING, m_codeString);
    m_settings->setValue(KEY_TOKEN, m_token);
    m_settings->setValue(KEY_PUBKEY, m_pubkey);
    m_settings->setValue(KEY_BDS_TOKEN, m_bdstoken);
    m_settings->setValue(KEY_UID_STR, m_uidStr);
    m_settings->sync();
}

QString BDiskTokenProvider::vcodeType() const
{
    return m_vcodeType;
}

void BDiskTokenProvider::setVcodeType(const QString &vcodeType)
{
    m_vcodeType = vcodeType;
}
