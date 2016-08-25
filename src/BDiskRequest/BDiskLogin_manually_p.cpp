#include "BDiskLogin_manually_p.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QEventLoop>
#include <QTimer>
#include <QCoreApplication>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

#include "BDiskCookieJar.h"
#include "BDiskConst.h"
#include "BDiskLogin_p.h"
#include "BDiskTokenProvider.h"

BDiskLoginManually::BDiskLoginManually(InnerStateHandler *handler, QObject *parent)
    : QThread(parent)
    , m_handler(handler)
    , m_tokenProvider(BDiskTokenProvider::instance())
    , m_userName(QString())
    , m_passWord(QString())
    , m_captchaText(QString())
{

}

QWaitCondition *BDiskLoginManually::cond()
{
    return &m_wait;
}

void BDiskLoginManually::run()
{
    QNetworkAccessManager networkMgr;
    BDiskCookieJar *jar = new BDiskCookieJar(this);
    networkMgr.setCookieJar(jar);

    QNetworkReply *reply = Q_NULLPTR;

    bool requestAborted = false;
    bool finish = false;
    bool networkSuccess = false;

    QNetworkReply::NetworkError networkError;

    QByteArray replyData; // from reply->readAll();
    QByteArray replyValue; // truncateCallback of replyData
    QJsonParseError jsonParseError;
    QJsonDocument jsonDoc;
    QJsonObject jsonObject; //final QJsonObject
    QJsonObject jsonData;

#define FREE_REPLY \
    if (reply) { \
        if (!reply->isFinished()) { \
            requestAborted = true; \
            reply->abort(); \
        } \
        disconnect(reply, 0, 0, 0); \
        reply->deleteLater(); \
        reply = Q_NULLPTR; \
    }

#define DO_LOOP_BLOCK \
    timer.start(); loop.exec();

#define STOP_LOOP_BLOCK \
    if (timer.isActive()) timer.stop(); \
    if (loop.isRunning()) loop.quit();

#define CHECK_IF_REPLY_SUCCESS \
    networkError = reply->error (); \
    networkSuccess = (networkError == QNetworkReply::NoError); \
    if (!networkSuccess) { \
        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,  reply->errorString()); \
        FREE_REPLY; \
        finish = true; \
        break; \
    }

#define RESET_FLAGS \
    requestAborted = false; \
    finish = false; \
    networkError = QNetworkReply::NetworkError::NoError; \
    networkSuccess = false;

#define CONNECT_REPLY \
    if (reply) { \
        connect(reply, &QNetworkReply::finished, [&](){ \
            STOP_LOOP_BLOCK; \
        }); \
    } else { \
        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE, QString("Can't visit http server")); \
        finish = true; \
        break; \
    }

#define LOOP_TO_WAIT_REPLY_FINISH \
    while (!reply->isFinished()) { \
        if (currentThread()->isInterruptionRequested()) { \
            requestAborted = true; \
            break; \
        } \
        qApp->processEvents(); \
    }

#define CHECK_IF_REPLY_ABORT \
    if (requestAborted) { \
        FREE_REPLY; \
        m_handler->dispatch(InnerEvent::EVENT_LOGIN_ABORT, QString("Http time out")); \
        finish = true; \
        break; \
    }

#define PARSE_TO_JSONOBJECT \
    replyData = reply->readAll(); \
    FREE_REPLY; \
    replyValue = truncateCallback(QString(replyData)).toLocal8Bit(); \
    jsonParseError.error = QJsonParseError::NoError; \
    jsonDoc = QJsonDocument::fromJson (replyValue, &jsonParseError); \
    if (jsonParseError.error != QJsonParseError::NoError) { \
        qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<jsonParseError.errorString (); \
        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE, \
                            QString("parse json error [%1]").arg(jsonParseError.errorString())); \
        finish = true; \
        break; \
    } \
    jsonObject = jsonDoc.object();


    QEventLoop loop;
    QTimer timer;
    timer.setInterval(BDISK_REQUEST_TIMEOUT);
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout,
            [&]() {
        if (reply && !reply->isFinished()) {
            requestAborted = true;
            reply->abort();
        }
        STOP_LOOP_BLOCK;
    });

    do {
        /*
         * visit baidu.com to check if network is ok
         */
        QUrl url(BDISK_URL_HOME);
        QNetworkRequest request(url);
        fillRequest(&request);

        reply = networkMgr.get(request);
        CONNECT_REPLY;
        DO_LOOP_BLOCK;

        LOOP_TO_WAIT_REPLY_FINISH;

        CHECK_IF_REPLY_ABORT;
        CHECK_IF_REPLY_SUCCESS;

        FREE_REPLY;
        RESET_FLAGS;

        /*
         * setp 1
         */
        url = QUrl(QString("%1?getapi&tpl=netdisk&apiver=v3&tt=%2&class=login&logintype=basicLogin&callback=bd__cbs__k7tkx3")
                   .arg(BDISK_URL_PASSPORT_API).arg(QString::number(QDateTime::currentMSecsSinceEpoch())));
        qDebug()<<Q_FUNC_INFO<<"url "<<url;

        request = QNetworkRequest(url);
        fillRequest(&request);
        reply = networkMgr.get(request);

        CONNECT_REPLY;
        DO_LOOP_BLOCK;

        LOOP_TO_WAIT_REPLY_FINISH;

        CHECK_IF_REPLY_ABORT;
        CHECK_IF_REPLY_SUCCESS;

        PARSE_TO_JSONOBJECT;

        jsonData = jsonObject.value("data").toObject();
        m_tokenProvider->setToken(jsonData.value("token").toString());
//        qDebug()<<Q_FUNC_INFO<<"logincheck token "<<m_tokenProvider->token();

        /*
         * setp 2
         */
        FREE_REPLY;
        RESET_FLAGS;

        //"logincheck" "&token=%s" "&tpl=netdisk" "&apiver=v3" "&tt=%d" "&username=%s" "&isphone=false" "&callback=bd__cbs__q4ztud"
        url = QUrl(QString("%1?logincheck&token=%2&tpl=netdisk&apiver=v3&tt=%3&username=%4&isphone=false&callback=bd__cbs__ps6wfe")
                   .arg(BDISK_URL_PASSPORT_API)
                   .arg(m_tokenProvider->token())
                   .arg(QString::number(QDateTime::currentMSecsSinceEpoch()))
                   .arg(m_userName));
        qDebug()<<Q_FUNC_INFO<<"url "<<url;

        request = QNetworkRequest(url);
        fillRequest(&request);

        reply = networkMgr.get(request);

        CONNECT_REPLY;
        DO_LOOP_BLOCK;

        LOOP_TO_WAIT_REPLY_FINISH;

        CHECK_IF_REPLY_ABORT;
        CHECK_IF_REPLY_SUCCESS;

        PARSE_TO_JSONOBJECT;

        jsonData = jsonObject.value("data").toObject();
//        QString token = jsonData.value("token").toString();
//        qDebug()<<Q_FUNC_INFO<<"logincheck again token "<<jsonData.value("token").toString();

        /*
         * step 3
         */
        FREE_REPLY;
        RESET_FLAGS;

        url = QUrl(BDISK_URL_GET_PUBLIC_KEY);
        QUrlQuery query;
        //"&token=%s" "&tpl=netdisk" "&apiver=v3" "&tt=%d" "&callback=bd__cbs__wl95ks",
        query.addQueryItem("token", m_tokenProvider->token());
        query.addQueryItem("tpl", "netdisk");
        query.addQueryItem("apiver", "v3");
        query.addQueryItem("tt", QString::number(QDateTime::currentMSecsSinceEpoch()));
        query.addQueryItem("callback", "bd__cbs__wl95ks");
        url.setQuery(query);

        qDebug()<<Q_FUNC_INFO<<"url "<<url;

        request = QNetworkRequest(url);
        fillRequest(&request);

        reply = networkMgr.get(request);

        CONNECT_REPLY;
        DO_LOOP_BLOCK;

        LOOP_TO_WAIT_REPLY_FINISH;

        CHECK_IF_REPLY_ABORT;
        CHECK_IF_REPLY_SUCCESS;

        PARSE_TO_JSONOBJECT;

        m_tokenProvider->setPubkey(jsonObject.value("pubkey").toString().trimmed());
        m_tokenProvider->setKey(jsonObject.value("key").toString().trimmed());
//        qDebug()<<Q_FUNC_INFO<<" pubkey "<<m_tokenProvider->pubkey();
//        qDebug()<<Q_FUNC_INFO<<" key "<<m_tokenProvider->key();

        /*
         * step 4
         *
         * get codeString, this is also used for captch image
         * refresh captch image need to call this
         *
         */
        FREE_REPLY;
        RESET_FLAGS;

        QString str(BDISK_URL_PASSPORT_BASE);
        str += "?reggetcodestr";
        str += QString("&token=%1").arg(m_tokenProvider->token());
        str += "&tpl=netdisk&apiver=v3&fr=login&callback=bd__cbs__l6c16p";
        str += QString("tt=%1").arg(QString::number(QDateTime::currentMSecsSinceEpoch()));
        url = QUrl(str);
        qDebug()<<Q_FUNC_INFO<<"url "<<url;

        request = QNetworkRequest(url);
        fillRequest(&request);

        reply = networkMgr.get(request);

        CONNECT_REPLY;
        DO_LOOP_BLOCK;

        LOOP_TO_WAIT_REPLY_FINISH;

        CHECK_IF_REPLY_ABORT;
        CHECK_IF_REPLY_SUCCESS;

        PARSE_TO_JSONOBJECT;

        jsonData = jsonObject.value("data").toObject();
        m_tokenProvider->setCodeString(jsonData.value("verifyStr").toString());
//        qDebug()<<Q_FUNC_INFO<<"codeString "<<m_tokenProvider->codeString();

        FREE_REPLY;
        RESET_FLAGS;

        if (!m_tokenProvider->codeString().isEmpty()) {

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
        }

        qDebug()<<Q_FUNC_INFO<<"======== continue thread";

        /*
         * check verifycode
         */
        if (!m_captchaText.isEmpty()) {
            FREE_REPLY;
            RESET_FLAGS;
            QString str("https://passport.baidu.com/v2/?checkvcode&");
            str += QString("token=%1&").arg(m_tokenProvider->token());
            str += "tpl=netdisk&";
            str += "subpro=netdisk_web&";
            str += "apiver=v3&";
            str += QString("tt=%1&").arg(QString::number(QDateTime::currentMSecsSinceEpoch()));
            str += QString("verifycode=%1&").arg(m_captchaText);
            str += QString("codestring=%1&").arg(m_tokenProvider->codeString());
            str += "callback=bd__cbs__ln4hgj";
            url = QUrl(str);

            request = QNetworkRequest(url);
            fillRequest(&request);

            reply = networkMgr.get(request);

            CONNECT_REPLY;
            DO_LOOP_BLOCK;

            LOOP_TO_WAIT_REPLY_FINISH;

            CHECK_IF_REPLY_ABORT;
            CHECK_IF_REPLY_SUCCESS;

            PARSE_TO_JSONOBJECT;

            const QJsonObject errInfo = jsonObject.value("errInfo").toObject();
            const int err = errInfo.value("no").toString().toInt();
            if (err != 0) {
                qDebug()<<Q_FUNC_INFO<<">>>> need re-start thread to refresh captcha image";

                /*
                 * We'll restart whole thread when receive this signal atm,
                 * as we're using thread and loop event to block QNetworkAccessManager asynchronous call
                 */
                m_handler->dispatch(InnerEvent::EVENT_CAPTCHA_URL_NEED_REFRESH,
                                    QString("Error code [%1], msg [%2]")
                                    .arg(QString::number(err))
                                    .arg(errInfo.value("msg").toString()));
                finish = true;
                break;
            }
        }

        FREE_REPLY;
        RESET_FLAGS;

//        qDebug()<<Q_FUNC_INFO<<"codeString "<<m_tokenProvider->codeString();

        url = QUrl(QString("%1?login").arg(BDISK_URL_PASSPORT_API));

        QString postStr;
        postStr += "staticpage=http://pan.baidu.com/res/static/thirdparty/pass_v3_jump.html&";
        postStr += "charset=utf-8&";
        postStr += QString("token=%1&").arg(m_tokenProvider->token());
        postStr += "tpl=netdisk&";
//        postStr += "subpro=&";
        postStr += "subpro=netdisk_web&";
        postStr += "apiver=v3&";
        postStr += QString("tt=%1&").arg(QString::number(QDateTime::currentMSecsSinceEpoch()));
        postStr += QString("codestring=%1&").arg(m_tokenProvider->codeString());
        postStr += "safeflg=0&";
        postStr += "u=http://pan.baidu.com/&";
        postStr += "isPhone=&";
        postStr += "detect=1&";
        postStr += "gid=5A889AF-EC9D-4EB1-B7C1-182EB882194B&";
        postStr += "quick_user=0&";
        postStr += "logintype=basicLogin&";
        postStr += "logLoginType=pc_loginBasic&";
        postStr += "idc=&";
        postStr += "loginmerge=true&";
        postStr += "foreignusername=&";
        postStr += QString("username=%1&").arg(m_userName);
        postStr += QString("password=%1&").arg(m_passWord);
        postStr += QString("verifycode=%1&").arg(m_captchaText);
        //    postStr += "mem_pass=on&";
        //    postStr += "rsakey=&";// rsa_pwd ? pcs->key : "&";
        //    postStr += "crypttype=&";// rsa_pwd ? "12" : "&";
        postStr += "ppui_logintime=1319881&";
        postStr += "countrycode=&";
        postStr += "callback=parent.bd__pcbs__7vxzg7";

        request = QNetworkRequest(url);
        fillRequest(&request);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(postStr.length()));

//        qDebug()<<Q_FUNC_INFO<<" post data ["<<postStr<<"] to ["<<url<<"]";

        reply = networkMgr.post(request, postStr.toUtf8());

        CONNECT_REPLY;
        DO_LOOP_BLOCK;

        LOOP_TO_WAIT_REPLY_FINISH;

        CHECK_IF_REPLY_ABORT;
        CHECK_IF_REPLY_SUCCESS;

        replyData = reply->readAll();
        FREE_REPLY;

        const int loginErrorCode = getErrorFromPostData(replyData);
        qDebug()<<Q_FUNC_INFO<<">>>>>  loginErrCode "<<loginErrorCode;

        /*
         * see https://github.com/GangZhuo/BaiduPCS/issues/29
         */
        if (loginErrorCode == 3 || loginErrorCode == 6 || loginErrorCode == 257 || loginErrorCode == 200010) {
            QString codeStr = getCodeStringFromPostData(replyData);
            if (codeStr.isEmpty()) {
                m_tokenProvider->setCodeString(QString());
                m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                    QString("Can't read the codestring from the response [%1]")
                                    .arg(QString(replyData)));
                finish = true;
                break;
            }
            m_tokenProvider->setCodeString(codeStr);
            qDebug()<<Q_FUNC_INFO<<">>>> send signal to re-start thread";

            /*
             * We'll restart whole thread when receive this signal atm,
             * as we're using thread and loop event to block QNetworkAccessManager asynchronous call
             */
            m_handler->dispatch(InnerEvent::EVENT_CAPTCHA_URL_NEED_REFRESH,
                                QString("Error code = %1").arg(QString::number(loginErrorCode)));
            finish = true;
            break;
        }

        FREE_REPLY;
        RESET_FLAGS;

        if (loginErrorCode != -1) {
            if (loginErrorCode == 0 || loginErrorCode == 18 || loginErrorCode == 120016
                    || loginErrorCode == 400032 || loginErrorCode == 400034
                    || loginErrorCode == 400037 || loginErrorCode == 400401) {
                url = QUrl(BDISK_URL_DISK_HOME);

                /*
                 *  use loop for cookies login check if url redirected
                 */
                do {
                    FREE_REPLY;
                    RESET_FLAGS;

                    request = QNetworkRequest(url);
                    fillRequest(&request);

                    reply = networkMgr.head(request);
                    if (reply) {
                        connect(reply, &QNetworkReply::finished, [&](){
                            STOP_LOOP_BLOCK;
                        });
                    } else {
                        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                            QString("Can't get http header")); \
                        finish = true;
                        break;
                    }
                    DO_LOOP_BLOCK;

                    LOOP_TO_WAIT_REPLY_FINISH;

                    CHECK_IF_REPLY_ABORT;
                    CHECK_IF_REPLY_SUCCESS;

                    QVariant vrt = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
                    /*
                     * As we call FREE_REPLY first after break loop or loop continue,
                     * it is OK to skip FREE_REPLY below
                     */
                    if (vrt.isNull() || !vrt.isValid()) {
                        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                            QString("Get http status code error!"));
                        finish = true;
                        break;
                    }
                    bool ok = false;
                    int ret = vrt.toInt(&ok);
                    if (!ok) {
                        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                            QString("Can't get http status code!"));
                        finish = true;
                    } else if (ret == 200) {
                        finish = false;
                        break;
                    } else if (ret == 302) {
                        //302 //redirect url
                        url = reply->header(QNetworkRequest::LocationHeader).toUrl();
                        qDebug()<<Q_FUNC_INFO<<" redirect : "<<url;
                        continue;
                    } else {
                        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                            QString("Wrong http status code [%1]").arg(QString::number((ret))));
                        finish = true;
                        break;
                    }
                } while (true);

                FREE_REPLY;
                RESET_FLAGS;

                qDebug()<<Q_FUNC_INFO<<"url "<<url;

                request = QNetworkRequest(url);
                fillRequest(&request);

                reply = networkMgr.get(request);

                CONNECT_REPLY;
                DO_LOOP_BLOCK;

                LOOP_TO_WAIT_REPLY_FINISH;

                CHECK_IF_REPLY_ABORT;
                CHECK_IF_REPLY_SUCCESS;

                const QByteArray qba = reply->readAll();
                FREE_REPLY;
                const QString bdstoken = m_tokenProvider->bdstoken();

                /*
                 * update bdstoken and username if possible
                 */

                //TODO get bdstoken from response by key yunData.MYBDSTOKEN or FileUtils.bdstoken
                const QString value = truncateYunData(QString(qba));
                QJsonParseError error;
                const QJsonDocument doc = QJsonDocument::fromJson (value.toLocal8Bit(), &error);
                if (error.error != QJsonParseError::NoError && bdstoken.isEmpty()) {
                    qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
                    m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                        QString("Parse json error [%1]").arg(error.errorString()));
                    finish = true;
                    break;
                }
                const QJsonObject obj = doc.object();
                const QString token = obj.value("bdstoken").toString();
                if (bdstoken.isEmpty() && token.isEmpty()) {
                    m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                        QString("Can't get bdstoken"));
                    finish = true;
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
                finish = true;
                break;
            }
        }
    } while (true);
    m_tokenProvider->flush();

    if (!finish) {
        m_handler->dispatch(InnerEvent::EVENT_LOGIN_SUCCESS);
    }

    FREE_REPLY;
    RESET_FLAGS;

    if (jar) {
        jar->deleteLater();
        jar = Q_NULLPTR;
    }
}

void BDiskLoginManually::fillRequest(QNetworkRequest *req)
{
    if (!req)
        return;
    req->setRawHeader("User-Agent", "Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0");
    req->setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
}

QString BDiskLoginManually::truncateCallback(const QString &callback) const
{
    if (callback.isEmpty())
        return QString();
    QString str(callback);
    str = str.replace("'", "\"");
    if (str.startsWith("(") && str.endsWith(")")) {
        return str.mid(1, str.length() -1);
    }
    int start = 0;
    for (; start<str.length(); ++start) {
        if (QString(str.at(start)) == "{")
            break;
    }
    int end = str.lastIndexOf(")");
    if (end < 0)
        end = str.length();
    return str.mid(start, end-start);
}

QString BDiskLoginManually::truncateYunData(const QString &data) const
{
    if (data.isEmpty())
        return QString();
    QString str = data.simplified();

//    qDebug()<<Q_FUNC_INFO<<">>>>>>>>>>>>>>> simplified vale "<<str;

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






