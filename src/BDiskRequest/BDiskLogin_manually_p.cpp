#include "BDiskLogin_manually_p.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QEventLoop>
#include <QTimer>
#include <QUuid>
#include <QCoreApplication>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

#include "BDiskCookieJar.h"
#include "BDiskConst.h"
#include "BDiskLogin_p.h"
#include "BDiskTokenProvider.h"
#include "SyncNetworkRequest.h"

#include "QCNetworkAccessManager.h"
#include "QCNetworkRequest.h"
#include "QCNetworkSyncReply.h"
#include "QCNetworkAsyncReply.h"
#include "Utility.h"

using namespace QCurl;

BDiskLoginManually::BDiskLoginManually(InnerStateHandler *handler, QObject *parent)
    : QThread(parent)
    , m_handler(handler)
    , m_tokenProvider(BDiskTokenProvider::instance())
//    , m_cookieJar(new BDiskCookieJar(0))
    , m_userName(QString())
    , m_passWord(QString())
    , m_captchaText(QString())
{
    m_uuid = QUuid::createUuid().toString();
    m_callbackName = QString("bd__cbs__bdpand");
}

BDiskLoginManually::~BDiskLoginManually()
{
//    if (m_cookieJar) {
//        m_cookieJar->flush();
//        m_cookieJar->deleteLater();
//        m_cookieJar = Q_NULLPTR;
//    }
}

QWaitCondition *BDiskLoginManually::cond()
{
    return &m_wait;
}

void BDiskLoginManually::run()
{
    bool breakFlag = false;

    NetworkError networkError;
    bool networkSuccess;
    QByteArray replyData; // from reply->readAll();
    QByteArray replyValue; // truncateCallback of replyData
    QJsonParseError jsonParseError;
    QJsonDocument jsonDoc;
    QJsonObject jsonObject; //final QJsonObject
    QJsonObject jsonData;

//    SyncNetworkRequest networkMgr;
//    networkMgr.setCookieJar(m_cookieJar);
//    m_cookieJar->setParent(0);
//    networkMgr.setTimeOut(BDISK_REQUEST_TIMEOUT);
//    networkMgr.timeOut([&]{
//        breakFlag = true;
//        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE, QString("Request timeout"));
//    });
    //TODO set timeout
    QCNetworkAccessManager networkMgr;
    networkMgr.setCookieFilePath(getCookieFile());


#define FREE_REPLY(reply) \
    if(reply) { \
        reply->deleteLater(); \
        reply = Q_NULLPTR; \
    }

//#define CHECK_REPLY_VALID(reply) \
//    if (!reply) { \
//        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE, QString("Nullptr network reply")); \
//        breakFlag = true; \
//    }

#define CHECK_IF_REPLY_SUCCESS(reply) \
    if (!reply) { \
        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE, QString("Nullptr network reply")); \
        breakFlag = true; \
        return; \
    } \
    networkError = reply->error(); \
    networkSuccess = (networkError == NetworkNoError); \
    if (!networkSuccess) { \
        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,  reply->errorString()); \
        breakFlag = true; \
        reply->deleteLater(); \
        reply = Q_NULLPTR; \
        return; \
    }

#define PARSE_TO_JSONOBJECT(data) \
    replyValue.clear(); \
    jsonDoc = QJsonDocument(); \
    jsonObject = QJsonObject(); \
    jsonData = QJsonObject(); \
    replyValue = truncateCallback(QString(data), m_callbackName).toUtf8(); \
    jsonParseError.error = QJsonParseError::NoError; \
    jsonDoc = QJsonDocument::fromJson (replyValue, &jsonParseError); \
    if (jsonParseError.error != QJsonParseError::NoError) { \
        qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<jsonParseError.errorString (); \
        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE, \
                            QString("parse json error [%1]").arg(jsonParseError.errorString())); \
        breakFlag = true; \
        return; \
    } \
    jsonObject = jsonDoc.object();

    do {
        /*
         * visit baidu.com to check if network is ok
         */
        QUrl url(BDISK_URL_HOME);
        QCNetworkRequest request(url);
        fillRequest(&request);
//        networkMgr.get(request, [&](QNetworkReply *reply) {
//            CHECK_IF_REPLY_SUCCESS(reply);
//        });
        QCNetworkSyncReply *reply = networkMgr.create(request);
//        reply->setWriteFunction([](char *buffer, size_t size) ->size_t {
//            return size;
//        });
        reply->perform();
        CHECK_IF_REPLY_SUCCESS(reply);
        FREE_REPLY(reply);

        if (breakFlag) {
            break;
        }
        breakFlag = false;

        /*
         * setp 1
         */
        url = QUrl(BDISK_URL_PASSPORT_API);
        {
            QUrlQuery query;
            query.addQueryItem("getapi", QString());
            query.addQueryItem("tpl", "netdisk");
            query.addQueryItem("subpro", "netdisk_web");
            query.addQueryItem("apiver", "v3");
            query.addQueryItem("tt", QString::number(QDateTime::currentMSecsSinceEpoch()));
            query.addQueryItem("class", "login");
            query.addQueryItem("gid", m_uuid);
            query.addQueryItem("loginversion", "v4");
            query.addQueryItem("logintype", "basicLogin");
            query.addQueryItem("traceid", QString());
            query.addQueryItem("callback", m_callbackName);
            url.setQuery(query);
        }
        request = QCNetworkRequest(url);
        fillRequest(&request);
//        networkMgr.get(request, [&](QNetworkReply *reply) {
//            CHECK_IF_REPLY_SUCCESS(reply);
//            PARSE_TO_JSONOBJECT(reply);
//            jsonData = jsonObject.value("data").toObject();
//            m_tokenProvider->setToken(jsonData.value("token").toString());
////            qDebug()<<Q_FUNC_INFO<<"getapi token "<<m_tokenProvider->token();
//        });
        reply = networkMgr.create(request);
        replyData.clear();
        reply->setWriteFunction([&](char *buffer, size_t size) ->size_t {
            replyData.append(buffer, static_cast<int>(size));
            return size;
        });
        reply->setCustomHeaderFunction([&](char *buffer, size_t size)->size_t {
            if (!replyData.isEmpty()) {
                const QByteArray header(buffer, static_cast<int>(size));
                const int pos = replyData.indexOf(header);
                if (pos >= 0) {
                    replyData.remove(pos, static_cast<int>(size));
                }
            }
            return size;
        });
        reply->perform();
        CHECK_IF_REPLY_SUCCESS(reply);

        qDebug()<<Q_FUNC_INFO<<" --------------------setp1 ";

//        replyData = truncateCallback(QString(replyData), m_callbackName).toUtf8();

//        qDebug()<<Q_FUNC_INFO<<" setp1 now reply data "<<replyData;

        PARSE_TO_JSONOBJECT(replyData);
        FREE_REPLY(reply);
        jsonData = jsonObject.value("data").toObject();
        m_tokenProvider->setToken(jsonData.value("token").toString());

        if (breakFlag) {
            break;
        }
        breakFlag = false;

        /*
         * setp 2
         */
        url = QUrl(BDISK_URL_GET_PUBLIC_KEY);
        {
            QUrlQuery query;
            query.addQueryItem("token", m_tokenProvider->token());
            query.addQueryItem("tpl", "netdisk");
            query.addQueryItem("subpro", "netdisk_web");
            query.addQueryItem("apiver", "v3");
            query.addQueryItem("tt", QString::number(QDateTime::currentMSecsSinceEpoch()));
            query.addQueryItem("gid", m_uuid);
            query.addQueryItem("loginversion", "v4");
            query.addQueryItem("traceid", QString());
            query.addQueryItem("callback", m_callbackName);
            url.setQuery(query);
        }
        request = QCNetworkRequest(url);
        fillRequest(&request);
//        networkMgr.get(request, [&](QNetworkReply *reply) {
//            CHECK_IF_REPLY_SUCCESS(reply);
//            PARSE_TO_JSONOBJECT(reply);
//            QString pubkey = jsonObject.value("pubkey").toString().trimmed();
//            QString key = jsonObject.value("key").toString().trimmed();
//            m_tokenProvider->setPubkey(pubkey);
//            m_tokenProvider->setKey(key);
//        });
        reply = networkMgr.create(request);
        replyData.clear();
        reply->setWriteFunction([&](char *buffer, size_t size) ->size_t {
            replyData.append(buffer, static_cast<int>(size));
            return size;
        });
        reply->setCustomHeaderFunction([&](char *buffer, size_t size)->size_t {
            if (!replyData.isEmpty()) {
                const QByteArray header(buffer, static_cast<int>(size));
                const int pos = replyData.indexOf(header);
                if (pos >= 0) {
                    replyData.remove(pos, static_cast<int>(size));
                }
            }
            return size;
        });
        reply->perform();
        CHECK_IF_REPLY_SUCCESS(reply);

        qDebug()<<Q_FUNC_INFO<<"--------------------- setp2 reply data ";

        PARSE_TO_JSONOBJECT(replyData);
        FREE_REPLY(reply);
        QString pubkey = jsonObject.value("pubkey").toString().trimmed();
        QString key = jsonObject.value("key").toString().trimmed();
        m_tokenProvider->setPubkey(pubkey);
        m_tokenProvider->setKey(key);

        if (breakFlag) {
            break;
        }
        breakFlag = false;

        /*
         * setp 3
         */
        {
            QString str(BDISK_URL_PASSPORT_API);
            str += "?logincheck&";
            str += "apiver=v3&";
            str += QString("callback=%1&").arg(m_callbackName);
            str += "dv=&";
            str += "logincheck=&";
            str += "loginversion=v4&";
            str += "sub_source=leadsetpwd&";
            str += "subpro=netdisk_web&";
            str += QString("token=%1&").arg(m_tokenProvider->token());
            str += "tpl=netdisk&";
            str += "traceid=&";
            str += QString("tt=%1&").arg(QString::number(QDateTime::currentMSecsSinceEpoch()));
            str += QString("username=%1").arg(m_userName);
            url = QUrl(str);
        }
//        qDebug()<<Q_FUNC_INFO<<"url "<<url;
        request = QCNetworkRequest(url);
        fillRequest(&request);
//        networkMgr.get(request, [&](QNetworkReply *reply) {
//            CHECK_IF_REPLY_SUCCESS(reply);
//            PARSE_TO_JSONOBJECT(reply);
//            jsonData = jsonObject.value(("errInfo")).toObject();
//            const int err = jsonData.value("no").toString().toInt();
//            qDebug()<<Q_FUNC_INFO<<"errInfo no "<<err;
//            //TODO if err != 0
//            if (!err) {
//                jsonData = jsonObject.value("data").toObject();
//                QString str = jsonData.value("vcodetype").toString();
////                qDebug()<<Q_FUNC_INFO<<"vcodetype "<<str;
//                if (!str.isEmpty()) {
//                    m_tokenProvider->setVcodeType(str);
//                }
//                str.clear();
//                str = jsonData.value("codeString").toString();
////                qDebug()<<Q_FUNC_INFO<<"codeString "<<str;
//                if (!str.isEmpty()) {
//                    m_tokenProvider->setCodeString(str);
//                    QUrl cpUrl(QString("%1?%2").arg(BDISK_URL_CAPTCHA).arg(m_tokenProvider->codeString()));
//                    qDebug()<<Q_FUNC_INFO<<" m_captchaImgUrl "<<cpUrl;
//                    m_handler->dispatch(InnerEvent::EVENT_CAPTCHA_URL, cpUrl);
//                    /*
//                    * lock thread to wait captcha
//                    */
//                    QMutex lock;
//                    qDebug()<<Q_FUNC_INFO<<"============= lock thread";
//                    lock.lock();
//                    m_wait.wait(&lock);
//                    qDebug()<<Q_FUNC_INFO<<"============= continue thread in logincheck";
//                }
//            }
//        });
        reply = networkMgr.create(request);
        replyData.clear();
        reply->setWriteFunction([&](char *buffer, size_t size) ->size_t {
            replyData.append(buffer, static_cast<int>(size));
            return size;
        });
        reply->setCustomHeaderFunction([&](char *buffer, size_t size)->size_t {
            if (!replyData.isEmpty()) {
                const QByteArray header(buffer, static_cast<int>(size));
                const int pos = replyData.indexOf(header);
                if (pos >= 0) {
                    replyData.remove(pos, static_cast<int>(size));
                }
            }
            return size;
        });
        reply->perform();
        CHECK_IF_REPLY_SUCCESS(reply);

        qDebug()<<Q_FUNC_INFO<<" ------------------- setp3 reply data ";

        PARSE_TO_JSONOBJECT(replyData);
        FREE_REPLY(reply);
        {
            jsonData = jsonObject.value(("errInfo")).toObject();
            const int err = jsonData.value("no").toString().toInt();
            qDebug()<<Q_FUNC_INFO<<"errInfo no "<<err;
            //TODO if err != 0
            if (!err) {
                jsonData = jsonObject.value("data").toObject();
                QString str = jsonData.value("vcodetype").toString();
                //                qDebug()<<Q_FUNC_INFO<<"vcodetype "<<str;
                if (!str.isEmpty()) {
                    m_tokenProvider->setVcodeType(str);
                }
                str.clear();
                str = jsonData.value("codeString").toString();
                //                qDebug()<<Q_FUNC_INFO<<"codeString "<<str;
                if (!str.isEmpty()) {
                    m_tokenProvider->setCodeString(str);
                    QUrl cpUrl(QString("%1?%2").arg(BDISK_URL_CAPTCHA).arg(m_tokenProvider->codeString()));
                    qDebug()<<Q_FUNC_INFO<<" m_captchaImgUrl "<<cpUrl;
                    m_handler->dispatch(InnerEvent::EVENT_CAPTCHA_URL, cpUrl);
                    /*
                     * lock thread to wait captcha
                     */
                    QMutex lock;
                    qDebug()<<Q_FUNC_INFO<<"============= lock thread";
                    lock.lock();
                    m_wait.wait(&lock);
                    qDebug()<<Q_FUNC_INFO<<"============= continue thread in logincheck";
                }
            }
        }

        if (breakFlag) {
            break;
        }
        breakFlag = false;

        /*
         * step 4
         *
         */

DO_LOGIN_POST:
        url = QUrl(QString("%1?login").arg(BDISK_URL_PASSPORT_API));
        QString postStr;
        postStr += "apiver=v3&";
        postStr += QString("callback=parent.%1&").arg(m_callbackName);
        postStr += "charset=UTF-8&";
        postStr += QString("codestring=%1&").arg(m_tokenProvider->codeString());
//        postStr += "crypttype=12&";
        postStr += "crypttype=&";
        postStr += "detect=1&";
        postStr += "dv=&";
        postStr += "foreignusername=&";
        postStr += "fp_info=&";
        postStr += "fp_uid=&";
        postStr += QString("gid=%1&").arg(m_uuid);
        postStr += "idc=&";
        postStr += "isPhone=&";
        postStr += "loginmerge=true&";
        postStr += "logintype=basicLogin&";
        postStr += "loginversion=v4&";
        postStr += "logLoginType=pc_loginBasic&";
        postStr += QString("password=%1&").arg(m_passWord);
        postStr += "ppui_logintime=162602&"; //what is this?
        postStr += "quick_user=0&";
//        postStr += QString("rsakey=%1&").arg(m_tokenProvider->pubkey());
        postStr += "rsakey=&";
        postStr += "safeflg=0&";
        postStr += "staticpage=http://pan.baidu.com/res/static/thirdparty/pass_v3_jump.html&";
        postStr += "subpro=netdisk_web&";
        postStr += QString("token=%1&").arg(m_tokenProvider->token());
        postStr += "tpl=netdisk&";
        postStr += "traceid=&";
        postStr += QString("tt=%1&").arg(QString::number(QDateTime::currentMSecsSinceEpoch()));
        postStr += "u=http://pan.baidu.com/&";
        postStr += QString("username=%1&").arg(m_userName);
        postStr += QString("verifycode=%1").arg(m_captchaText);

        request = QCNetworkRequest(url);
        fillRequest(&request);
        request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
        request.setRawHeader("Content-Length", QByteArray::number(postStr.length()));

        qDebug()<<Q_FUNC_INFO<<" post data ["<<postStr<<"] to ["<<url<<"]";

//        networkMgr.post(request, postStr.toUtf8(), [&](QNetworkReply *reply) {
//            CHECK_IF_REPLY_SUCCESS(reply);
//            replyData.clear();
//            replyData = reply->readAll();
//            FREE_REPLY(reply);
//        });
        reply = networkMgr.create(request);
        replyData.clear();
        reply->setWriteFunction([&](char *buffer, size_t size) ->size_t {
            replyData.append(buffer, static_cast<int>(size));
            return size;
        });
        reply->setPostData(postStr.toUtf8());
        reply->perform();
        CHECK_IF_REPLY_SUCCESS(reply);
        FREE_REPLY(reply);

        if (breakFlag) {
            break;
        }
        breakFlag = false;
        const int loginErrorCode = getErrorFromPostData(replyData);
        qDebug()<<Q_FUNC_INFO<<">>>>>  loginErrCode "<<loginErrorCode;
        /*
         * see https://github.com/GangZhuo/BaiduPCS/issues/29
         */
        if (loginErrorCode == 3 || loginErrorCode == 6 || loginErrorCode == 257 || loginErrorCode == 200010) {
            QString codeStr = getCodeStringFromPostData(replyData);
//            qDebug()<<Q_FUNC_INFO<<">>>>>  codeString  "<<codeStr;
            if (codeStr.isEmpty()) {
                m_tokenProvider->setCodeString(QString());
                m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                    QString("Can't read the codestring from the response [%1]")
                                    .arg(QString(replyData)));
                breakFlag = true;
                break;
            }
            m_tokenProvider->setCodeString(codeStr);

            //https://passport.baidu.com/cgi-bin/genimage?njGa006de4b5716f53302d914c2a6018300c6ad5b0671018a5b
            QUrl cpUrl(QString("%1?%2").arg(BDISK_URL_CAPTCHA).arg(m_tokenProvider->codeString()));
            qDebug()<<Q_FUNC_INFO<<" m_captchaImgUrl "<<cpUrl;
            m_handler->dispatch(InnerEvent::EVENT_CAPTCHA_URL, cpUrl);

            /*
            * lock thread to wait captcha
            */
            QMutex lock;
            qDebug()<<Q_FUNC_INFO<<"============= lock thread";
            lock.lock();
            m_wait.wait(&lock);

            qDebug()<<Q_FUNC_INFO<<"======== continue thread";
            goto DO_LOGIN_POST;
        }
        if (breakFlag) {
            break;
        }
        breakFlag = false;

        if (loginErrorCode != -1) {
            if (loginErrorCode == 0 || loginErrorCode == 18 || loginErrorCode == 120016
                    || loginErrorCode == 400032 || loginErrorCode == 400034
                    || loginErrorCode == 400037 || loginErrorCode == 400401) {
//                url = QUrl(BDISK_URL_DISK_HOME);
//                QNetworkReply *redirectedReply = Q_NULLPTR;
//                /*
//                 *  use loop for cookies login check if url redirected
//                 */
//                do {

//                    FREE_REPLY(redirectedReply);
//                    breakFlag = false;
//                    request = QNetworkRequest(url);
//                    fillRequest(&request);
//                    networkMgr.head(request, [&](QNetworkReply *reply) {
//                        CHECK_IF_REPLY_SUCCESS(reply);
//                        redirectedReply = reply;
//                    });
//                    QVariant vrt = redirectedReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
//                    /*
//                     * As we call FREE_REPLY first after break loop or loop continue,
//                     * it is OK to skip FREE_REPLY below
//                     */
//                    if (vrt.isNull() || !vrt.isValid()) {
//                        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
//                                            QString("Get http status code error!"));
//                        breakFlag = true;
//                        break;
//                    }
//                    bool ok = false;
//                    int ret = vrt.toInt(&ok);
//                    if (!ok) {
//                        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
//                                            QString("Can't get http status code!"));
//                        breakFlag = true;
//                        break; //FIXME should we break this loop here?
//                    } else if (ret == 200) {
//                        breakFlag = false;
//                        break;
//                    } else if (ret == 302) {
//                        //302 //redirect url
//                        QVariant str = redirectedReply->header(QNetworkRequest::LocationHeader);
//                        /* NOTE:
//                         * this is an dirty hack for baidu pan redirect location
//                         * as the last redirect location is start with /disk/home...
//                         * whitch is an invalid value in Qt
//                         */
//                        if (!str.isValid()) {
//                            foreach(const auto &h, redirectedReply->rawHeaderPairs()) {
//                                if (QString(h.first).toLower() == QString("Location").toLower()) {
//                                    QString u = QString("https://pan.baidu.com%1").arg(QString(h.second));
//                                    url = QUrl(u);
//                                    break;
//                                }
//                            }
//                            breakFlag = false;
//                            break;
//                        }
//                        url = QUrl(str.toString());
//                        continue;
//                    } else {
//                        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
//                                            QString("Wrong http status code [%1]").arg(QString::number((ret))));
//                        breakFlag = true;
//                        break;
//                    }
//                } while (true);

//                if (redirectedReply) {
//                    redirectedReply->deleteLater();
//                    redirectedReply = Q_NULLPTR;
//                }

                // http 302 call
                url = QUrl(BDISK_URL_DISK_HOME);
                request = QCNetworkRequest(url);
                fillRequest(&request);
                request.setFollowLocation(true);
                QByteArray qba;
                reply = networkMgr.create(request);
                reply->setWriteFunction([&](char *buffer, size_t size) ->size_t {
                    qba.append(buffer, static_cast<int>(size));
                    return size;
                });
                reply->setCustomHeaderFunction([&](char *buffer, size_t size)->size_t {
                    //for http 302
                    QString header(QByteArray(buffer, static_cast<int>(size)));
                    qDebug()<<Q_FUNC_INFO<<"header "<<header;
                    if (!qba.isEmpty()) {
                        const int pos = qba.indexOf(header);
                        if (pos >= 0) {
                            qba.remove(pos, static_cast<int>(size));
                        }
                    }
                    const int pos = header.trimmed().indexOf(":");
                    if (pos > 0) {
                        QString key = header.mid(0, pos).simplified();
                        QString value = header.mid(pos+1, header.length()-pos-1).simplified();
                        if (key == "Location" && !value.isEmpty()) {
                            url = QString("https://pan.baidu.com%1").arg(value);
                        }
                    }
                    return size;
                });
                reply->perform();
                CHECK_IF_REPLY_SUCCESS(reply);

                qDebug()<<Q_FUNC_INFO<<"------ 302 qba "<<qba;

                FREE_REPLY(reply);

                if (breakFlag) {
                    break;
                }
                breakFlag = false;

                qDebug()<<Q_FUNC_INFO<<"--- now url "<<url;
//                url = QUrl("http://pan.baidu.com/disk/home");
                request = QCNetworkRequest(url);
                fillRequest(&request);
//                QByteArray qba;
//                networkMgr.get(request, [&](QNetworkReply *reply) {
//                    CHECK_IF_REPLY_SUCCESS(reply);
//                    qba = reply->readAll();
//                    FREE_REPLY(reply);
//                });
                qba.clear();
                reply = networkMgr.create(request);
                reply->setWriteFunction([&](char *buffer, size_t size) ->size_t {
                    qba.append(buffer, static_cast<int>(size));
                    return size;
                });
                reply->perform();
                CHECK_IF_REPLY_SUCCESS(reply);
                FREE_REPLY(reply);

                qDebug()<<Q_FUNC_INFO<<"------ qba "<<qba;

                const QString bdstoken = m_tokenProvider->bdstoken();

                /*
                 * update bdstoken and username if possible
                 */

                //TODO get bdstoken from response by key yunData.MYBDSTOKEN or FileUtils.bdstoken
                const QString value = truncateYunData(QString(qba));

                qDebug()<<Q_FUNC_INFO<<"------ value "<<value;

                QJsonParseError error;
                const QJsonDocument doc = QJsonDocument::fromJson (value.toUtf8(), &error);
                if (error.error != QJsonParseError::NoError && bdstoken.isEmpty()) {
                    qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
                    m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                        QString("Parse json error [%1]").arg(error.errorString()));
                    breakFlag = true;
                    break;
                }
                const QJsonObject obj = doc.object();
                const QString token = obj.value("bdstoken").toString();
                if (bdstoken.isEmpty() && token.isEmpty()) {
                    m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                        QString("Can't get bdstoken"));
                    breakFlag = true;
                    break;
                }
                if (!token.isEmpty()) { //update bdstoken
                    m_tokenProvider->setBdstoken(token);
                }
                QString uname = obj.value("username").toString();
                //FIXME send failure if uname is empty
                m_tokenProvider->setUidStr(uname);
                break;
            } else {
                qDebug()<<Q_FUNC_INFO<<"Login error, Error code "<<loginErrorCode;
                m_tokenProvider->setCodeString(QString());
                m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                    QString("Try to login error code = %1").arg(QString::number(loginErrorCode)));
                breakFlag = true;
                break;
            }
        }
    } while (true);
    m_tokenProvider->flush();

    if (!breakFlag) {
        m_handler->dispatch(InnerEvent::EVENT_LOGIN_SUCCESS);
    }
// <<<<<<< HEAD
// 
//     FREE_REPLY;
//     RESET_FLAGS;
// 
//     if (jar) {
//         jar->flush();
//         jar->deleteLater();
//         jar = Q_NULLPTR;
// =======
//    if (m_cookieJar) {
//        m_cookieJar->flush();
//// >>>>>>> no_qrc_built-in
//    }
}

inline void BDiskLoginManually::fillRequest(QCNetworkRequest *req)
{
    if (!req)
        return;
    req->setRawHeader("User-Agent", "Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0");
    req->setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
}

QString BDiskLoginManually::truncateCallback(const QString &callback, const QString &callbackName) const
{
    if (callback.isEmpty())
        return QString();
    QString str(callback);

    qDebug()<<Q_FUNC_INFO<<"-------------------";
    qDebug()<<Q_FUNC_INFO<<"before "<<str;

//    str = str.replace("\\", "");
    str = str.replace("'", "\"");
//    str = str.replace("\\","");
    const int length = callbackName.length();
    str = str.trimmed().mid(length+1, str.length()-length-2);

    qDebug()<<Q_FUNC_INFO<<"after "<<str;

    return str;
}

QString BDiskLoginManually::truncateYunData(const QString &data) const
{
    if (data.isEmpty())
        return QString();

    QString str = data.simplified();

    qDebug()<<Q_FUNC_INFO<<">>>>>>>>>>>>>>> simplified vale "<<str;

    //TODO, check for key yunData.MYBDSTOKEN and FileUtils.bdstoken

    int start = str.indexOf("context=");
    if (start <0)
        return QString();

    int lNum = 0; //number of "{"
    int rNum = 0; //number of "}"
    int end = -1;
    for (int i=start; i<str.length(); ++i) {
        if (QString(str.at(i)) == "{")
            lNum++;
        if (QString(str.at(i+1)) == "}")
            rNum++;

        if ((lNum == rNum) && (lNum != 0) && (rNum != 0)) {
            end = i+2;
            break;
        }
    }
//    qDebug()<<Q_FUNC_INFO<<"lnum "<<lNum<<" rnum "<<rNum<<" end "<<end;
    if (lNum == 0 || rNum == 0 || end <= 0)
        return QString();

    start += 8; //length of "context="
    if (start >= end)
        return QString();
    return str.mid(start, end - start);
}

QString BDiskLoginManually::getCodeStringFromPostData(const QByteArray &data) const
{
    if (data.isEmpty())
        return QString();
    QByteArray qba = data.simplified();
    int start = qba.indexOf("&codestring=");

    if (start > 0) {
        start += 12; //length of &codestring=
    } else {
        start = qba.indexOf("&codeString=");
        if (start > 0)
            start += 12; //length of &codeString=
    }
    if (start < 0)
        return QString();

    int end = -1;
    for (int i=start; i<qba.length(); ++i) {
        if (QString(qba.at(i)) == "&") {
            end = i;
            break;
        }
    }
    if (end < 0)
        return QString();
    qba = qba.mid(start, end - start);
    qDebug()<<Q_FUNC_INFO<<" >>> data "<<qba;
    if (qba.isEmpty())
        return QString();
    return  QString(qba);
}

int BDiskLoginManually::getErrorFromPostData(const QByteArray &data) const
{
    if (data.isEmpty())
        return -1;
    QByteArray qba = data.simplified();

    qDebug()<<Q_FUNC_INFO<<"........... data  "<<qba;

    int start = qba.indexOf("&error=");

    if (start > 0) {
        start += 7; //length of &error=
    } else {
        start = qba.indexOf("err_no=");
        if (start > 0)
            start += 7; //length of err_no=
    }
    if (start < 0)
        return -1;

    int end = -1;
    for (int i=start; i<qba.length(); ++i) {
        if (QString(qba.at(i)) == "&") {
            end = i;
            break;
        }
    }
    if (end < 0)
        return -1;
    qba = qba.mid(start, end - start);
    qDebug()<<Q_FUNC_INFO<<" >>> data "<<qba;
    if (!qba.isEmpty())
        return qba.toInt();
    return  -1;
}

QString BDiskLoginManually::captchaText() const
{
    return m_captchaText;
}

void BDiskLoginManually::setCaptchaText(const QString &captchaText)
{
    m_captchaText = captchaText;
}

void BDiskLoginManually::refreshCaptchaImgUrl()
{
//    QNetworkAccessManager mgr;
//    mgr.setCookieJar(m_cookieJar);
//    m_cookieJar->setParent(0);
    QCNetworkAccessManager mgr;
    mgr.setCookieFilePath(getCookieFile());

    QUrl url(BDISK_URL_PASSPORT_BASE);
    QUrlQuery query;
//    reggetcodestr=
//    token=xxx
//    tpl=netdisk
//    subpro=netdisk_web
//    apiver=v3
//    tt=1523201043180
//    fr=login
//    loginversion=v4
//    vcodetype=xxxx
//    traceid=
//    callback=xxx

    query.addQueryItem("reggetcodestr", QString());
    query.addQueryItem("token", m_tokenProvider->token());
    query.addQueryItem("tpl", "netdisk");
    query.addQueryItem("subpro", "netdisk_web");
    query.addQueryItem("apiver", "v3");
    query.addQueryItem("tt", QString::number(QDateTime::currentMSecsSinceEpoch()));
    query.addQueryItem("fr", "login");
    query.addQueryItem("loginversion", "v4");
    query.addQueryItem("vcodetype", m_tokenProvider->vcodeType());
    query.addQueryItem("traceid", QString());
    query.addQueryItem("callback", m_callbackName);
    url.setQuery(query);

    qDebug()<<Q_FUNC_INFO<<"url: "<<url;

    QCNetworkRequest request(url);
    fillRequest(&request);

    QCNetworkAsyncReply *reply = mgr.get(request);
    connect(reply, &QCNetworkAsyncReply::finished, [&]() {
        NetworkError error = reply->error();
        if (error != NetworkNoError) {
            qDebug()<<Q_FUNC_INFO<<"network error "<<reply->errorString();
            reply->deleteLater();
            reply = Q_NULLPTR;
            return;
        }
        QByteArray ba = reply->readAll();
        reply->deleteLater();
        reply = Q_NULLPTR;
        QByteArray replyValue = truncateCallback(QString(ba), m_callbackName).toUtf8();
        QJsonParseError jsonParseError;
        jsonParseError.error = QJsonParseError::NoError;
        QJsonDocument doc = QJsonDocument::fromJson(replyValue, &jsonParseError);
        if (jsonParseError.error != QJsonParseError::NoError) {
             qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<jsonParseError.errorString();
             return;
        }
        QJsonObject jsonObject = doc.object();
        QJsonObject jsonData = jsonObject.value(("errInfo")).toObject();
        const int err = jsonData.value("no").toString().toInt();
        qDebug()<<Q_FUNC_INFO<<"errInfo no "<<err;
        if (!err) {
            jsonData = jsonObject.value("data").toObject();
            QString str = jsonData.value("verifyStr").toString();
            qDebug()<<Q_FUNC_INFO<<"verifyStr "<<str;
            if (!str.isEmpty()) {
                m_tokenProvider->setCodeString(str);
                QUrl cpUrl(QString("%1?%2").arg(BDISK_URL_CAPTCHA).arg(m_tokenProvider->codeString()));
                qDebug()<<Q_FUNC_INFO<<" m_captchaImgUrl "<<cpUrl;
                m_handler->dispatch(InnerEvent::EVENT_CAPTCHA_URL, cpUrl);
            }
        }
    });
    reply->perform();
}

QString BDiskLoginManually::passWord() const
{
    return m_passWord;
}

void BDiskLoginManually::setPassWord(const QString &passWord)
{
    m_passWord = passWord;
}

QString BDiskLoginManually::userName() const
{
    return m_userName;
}

void BDiskLoginManually::setUserName(const QString &userName)
{
    m_userName = userName;
}






 
