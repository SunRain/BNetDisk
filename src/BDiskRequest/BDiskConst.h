#ifndef BDISKCONST_H
#define BDISKCONST_H

#include <QDir>
#include <QStandardPaths>
#include <QString>

static const char *BDISK_URL_HOME             = "http://www.baidu.com";
static const char *BDISK_URL_DISK_HOME		  = "http://pan.baidu.com/disk/home";
static const char *BDISK_URL_PASSPORT_BASE    = "https://passport.baidu.com/v2/";
static const char *BDISK_URL_PASSPORT_API	  = "https://passport.baidu.com/v2/api/";
static const char *BDISK_URL_GET_PUBLIC_KEY	  = "https://passport.baidu.com/v2/getpublickey";
static const char *BDISK_URL_PASSPORT_LOGOUT  = "https://passport.baidu.com/?logout&u=http://pan.baidu.com";
//static const char *BDISK_URL_CAPTCHA_GET_CODE = "https://passport.baidu.com/cgi-bin/genimage";
static const char *BDISK_URL_CAPTCHA		  = "https://passport.baidu.com/cgi-bin/genimage";
static const char *BDISK_URL_PAN_API		  = "https://pan.baidu.com/api";
static const char *BDISK_URL_PCS_REST		  = "http://c.pcs.baidu.com/rest/2.0/pcs/file";

static const int BDISK_REQUEST_TIMEOUT  = 5*1000;

///
/// \brief getCookieFile
/// \return
///
inline QString getCookieFile() {
    QString dataPath = QStandardPaths::writableLocation (QStandardPaths::DataLocation);
    QDir dir(dataPath);
    if (!dir.exists())
        dir.mkpath(dataPath);
    return QString("%1/cookie.libcurl").arg(dataPath);
}


#endif // BDISKCONST_H
