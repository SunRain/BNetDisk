#include "BDiskLogin.h"

#include <QTimer>
#include <QDebug>
#include <QEvent>
#include <QObject>
#include <QCoreApplication>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

#include <QEventLoop>

#include <QScopedPointer>

#include "BDiskConst.h"
#include "BDiskTokenProvider.h"
#include "BDiskCookieJar.h"


#define INNER_EVENT (QEvent::Type(QEvent::User))
//#define INNER_CAPTCHA_URL (QEvent::Type(QEvent::User + 1))

class InnerEvent : public QEvent
{
public:
    enum EventState {
        EVENT_LOGIN_ABORT = 0x0,
        EVENT_LOGIN_SUCCESS,
        EVENT_LOGIN_FAILURE,
        EVENT_CAPTCHA_URL,
        EVENT_CAPTCHA_TEXT,
        EVENT_CAPTCHA_URL_NEED_REFRESH
    };

public:
    InnerEvent(InnerEvent::EventState ret, const QString &value)
        : QEvent(INNER_EVENT)
        , m_url(QUrl())
    {
        m_ret = ret;
        m_data = value;
    }
    InnerEvent(InnerEvent::EventState ret, const QUrl &value)
        : QEvent(INNER_EVENT)
        , m_data(QString())
    {
        m_ret = ret;
        m_url = value;
    }
    virtual ~InnerEvent() {}

    EventState state() const {
        return m_ret;
    }
    QString data() const {
        return m_data;
    }
    QUrl url() const {
        return m_url;
    }
private:
    EventState m_ret;
    QString m_data;
    QUrl m_url;
};


class InnerStateHandler : public QObject
{
    Q_OBJECT
public:
    InnerStateHandler(QObject *parent = 0)
        : QObject(parent)
        , m_mutex(QMutex::Recursive)
    {

    }
    virtual ~InnerStateHandler() {}

    void dispatch(InnerEvent::EventState ret, const QString &data = QString()) {
        m_mutex.lock();
        qApp->postEvent(parent(), new InnerEvent(ret, data));
        m_mutex.unlock();
    }
    void dispatch(InnerEvent::EventState ret, const QUrl &data) {
        m_mutex.lock();
        qApp->postEvent(parent(), new InnerEvent(ret, data));
        m_mutex.unlock();
    }

private:
    QMutex m_mutex;
};


BDiskLogin::BDiskLogin(QObject *parent)
    : QThread(parent)
    , m_networkMgr(new QNetworkAccessManager(this))
    , m_reply(nullptr)
    , m_tokenProvider(BDiskTokenProvider::instance())
    , m_handler(new InnerStateHandler(this))
    , m_cookieJar(BDiskCookieJar::instance())
    , m_userName(QString())
    , m_passWord(QString())
    , m_captchaText(QString())
    , m_captchaImgUrl(QUrl())
    , m_requestAborted(false)
    , m_abortLogin(false)
    , m_breakThread(false)
    , m_loginFailureOrAborted(false)
    , m_loginErrCode(-1)
{

}

void BDiskLogin::login()
{
    if (this->isRunning()) {
        qDebug()<<Q_FUNC_INFO<<"========== thread running";
        //        this->quit();
        //        this->wait();
        m_wait.wakeAll();
    } else {
        qDebug()<<Q_FUNC_INFO<<"=========== start thead";
        this->start();
    }

}

void BDiskLogin::loginByCookie()
{
    QUrl url = QUrl(BDISK_URL_DISK_HOME);
    QNetworkRequest request(url);
    fillRequest(&request);
    m_reply = m_networkMgr->get(request);

    if (m_reply) {
        connect(m_reply, &QNetworkReply::finished,
                [&](){
//            if (m_requestAborted) {
//                m_requestAborted = false;
//                m_handler->dispatch(InnerEvent::EVENT_LOGIN_ABORT);
//                m_breakThread = true;
//                freeReply();
//                return;
//            }
            QNetworkReply::NetworkError e = m_reply->error ();
            bool success = (e == QNetworkReply::NoError);
            if (!success) {
                QString str = m_reply->errorString();
                freeReply();
                emit loginByCookieFailure(str);
                return;
            }
            QByteArray qba = m_reply->readAll();
            qDebug()<<Q_FUNC_INFO<<" reply data "<<qba;
            if (qba.trimmed().isEmpty()) {
                freeReply();
                emit loginByCookieSuccess();
                return;
            }

            //TODO get bdstoken from response by key yunData.MYBDSTOKEN or FileUtils.bdstoken
//            if (m_tokenProvider->bdstoken().isEmpty()) {
                QString value = truncateYunData(QString(qba));
                if (value.isEmpty()) {
                    qDebug()<<Q_FUNC_INFO<<"Can't get yunData values";
                    freeReply();
                    emit loginByCookieFailure("Can't get yunData values");
                    return;
                }
                QJsonParseError error;
                QJsonDocument doc = QJsonDocument::fromJson (value.toLocal8Bit(), &error);
                if (error.error != QJsonParseError::NoError) {
                    qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
                    freeReply();
                    emit loginByCookieFailure(QString("Parse json error [%1]").arg(error.errorString()));
                    return;
                }
                QJsonObject obj = doc.object();
                QString bdstoken = obj.value("bdstoken").toString();
                if (bdstoken.isEmpty()) {
                    emit loginByCookieFailure("Can't get bdstoken");
                    return;
                }
                m_tokenProvider->setBdstoken(bdstoken);
                QString uname = obj.value("username").toString();
                //FIXME emit failure if uname is empty;
                m_tokenProvider->setUidStr(uname);
//            }
            freeReply();
            emit loginByCookieSuccess();
        });
    }
}

QString BDiskLogin::userName() const
{
    return m_userName;
}

QString BDiskLogin::passWord() const
{
    return m_passWord;
}

QUrl BDiskLogin::captchaImgUrl() const
{
    return m_captchaImgUrl;
}

bool BDiskLogin::event(QEvent *e)
{
    if (e->type() == INNER_EVENT) {
        InnerEvent *event = (InnerEvent*)e;
        InnerEvent::EventState state =  event->state();
        if (state == InnerEvent::EVENT_LOGIN_ABORT) {
            m_loginFailureOrAborted = true;
            emit loginAbort();
        }
        if (state == InnerEvent::EVENT_LOGIN_FAILURE) {
            m_loginFailureOrAborted = true;
            emit loginFailure(event->data());
        }
        if (state == InnerEvent::EVENT_LOGIN_SUCCESS) {
            emit loginSuccess();
        }
        if (state == InnerEvent::EVENT_CAPTCHA_URL) {
            m_captchaImgUrl = event->url();
            emit captchaImgUrlChanged(m_captchaImgUrl);
        }
        if (state == InnerEvent::EVENT_CAPTCHA_TEXT) {
            emit captchaTextChanged(m_captchaText);
        }
        if (state == InnerEvent::EVENT_CAPTCHA_URL_NEED_REFRESH) {
            qDebug()<<Q_FUNC_INFO<<"+++++ re-start thread";
            m_loginFailureOrAborted = true;
            if (this->isRunning()) {
                m_wait.wakeAll();
                this->quit();
                this->wait();
            }
            freeReply();
            reset();
            this->start();
        }


        return true;
    }
    return QObject::event(e);
}

void BDiskLogin::run()
{
    QNetworkAccessManager *networkMgr = new QNetworkAccessManager();
    networkMgr->setCookieJar(m_cookieJar);

    QEventLoop loop;
    QTimer timer;
    timer.setInterval(BDISK_REQUEST_TIMEOUT);
    timer.setSingleShot(true);

//    connect(m_networkMgr, &QNetworkAccessManager::finished,
//            [&](){
//        qDebug()<<Q_FUNC_INFO<<">>>>>> QNetworkAccessManager finished";
//        if (timer.isActive())
//            timer.stop();
//        if (loop.isRunning())
//            loop.quit();
//    });
    connect(&timer, &QTimer::timeout,
            [&]() {
        qDebug()<<Q_FUNC_INFO<<">>>>>> QTimer timeout";
        freeReply();
        m_loginFailureOrAborted = true;
        if (timer.isActive())
            timer.stop();
        if (loop.isRunning())
            loop.quit();
    });

#define DO_LOOP_BLOCK           timer.start(); loop.exec();

#define CHECK_IF_REPLY_ABORTED  if (m_requestAborted) { \
                                    m_requestAborted = false; \
                                    m_handler->dispatch(InnerEvent::EVENT_LOGIN_ABORT); \
                                    m_breakThread = true; \
                                    freeReply(); \
                                    if (timer.isActive()) timer.stop(); \
                                    if (loop.isRunning()) loop.quit(); \
                                    return; \
                                }

#define CHECK_IF_REPLY_SUCCESS  QNetworkReply::NetworkError e = m_reply->error (); \
                                bool success = (e == QNetworkReply::NoError); \
                                if (!success) { \
                                    m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,  m_reply->errorString()); \
                                    m_breakThread = true; \
                                    freeReply(); \
                                    if (timer.isActive()) timer.stop(); \
                                    if (loop.isRunning()) loop.quit(); \
                                    return; \
                                }

#define DO_RESET_AND_FREE       freeReply(); reset();

#define CHECK_AND_BREAK_THREAD  if (m_breakThread) break;

#define FREE_REPLY_AND_STOP_BLOCK   freeReply(); \
                                    if (timer.isActive()) timer.stop(); \
                                    if (loop.isRunning()) loop.quit();


    do {
        /*
         * visit baidu.com to check if network is ok
         */
        QUrl url(BDISK_URL_HOME);
        QNetworkRequest request(url);
        fillRequest(&request);
        m_reply = networkMgr->get(request);
        if (m_reply) {
            connect(m_reply, &QNetworkReply::finished,
                    [&](){
                qDebug()<<Q_FUNC_INFO<<">>>>>> QNetworkReply finished";

                CHECK_IF_REPLY_ABORTED;
                CHECK_IF_REPLY_SUCCESS;
//                freeReply();
                FREE_REPLY_AND_STOP_BLOCK;
            });
            DO_LOOP_BLOCK;
        }

        CHECK_AND_BREAK_THREAD;

        DO_RESET_AND_FREE;

        url = QUrl(QString("%1?getapi&tpl=netdisk&apiver=v3&tt=%2&class=login&logintype=basicLogin&callback=bd__cbs__k7tkx3")
                   .arg(BDISK_URL_PASSPORT_API).arg(QString::number(QDateTime::currentMSecsSinceEpoch())));
        qDebug()<<Q_FUNC_INFO<<"url "<<url;

        request = QNetworkRequest(url);
        fillRequest(&request);

        m_reply = networkMgr->get(request);
        if (m_reply) {
            connect(m_reply, &QNetworkReply::finished,
                    [&](){
                CHECK_IF_REPLY_ABORTED;
                CHECK_IF_REPLY_SUCCESS;

                QByteArray qba = m_reply->readAll();
//                qDebug()<<Q_FUNC_INFO<<" URL_PASSPORT_API ok "<<qba;
                QByteArray value = truncateCallback(QString(qba)).toLocal8Bit();
//                qDebug()<<Q_FUNC_INFO<<" json value "<<value;

                QJsonParseError error;
                QJsonDocument doc = QJsonDocument::fromJson (value, &error);
                if (error.error != QJsonParseError::NoError) {
                    qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
                    m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                        QString("arse json error [%1]").arg(error.errorString()));
                    m_breakThread = true;
                } else {
                    QJsonObject obj = doc.object();
                    QJsonObject data = obj.value("data").toObject();
                    m_tokenProvider->setToken(data.value("token").toString());
                    qDebug()<<Q_FUNC_INFO<<"token "<<m_tokenProvider->token();
                }
//                freeReply();
                FREE_REPLY_AND_STOP_BLOCK;
            });
            DO_LOOP_BLOCK
        }

        CHECK_AND_BREAK_THREAD;

        DO_RESET_AND_FREE;

        //"logincheck" "&token=%s" "&tpl=netdisk" "&apiver=v3" "&tt=%d" "&username=%s" "&isphone=false" "&callback=bd__cbs__q4ztud"
        url = QUrl(QString("%1?logincheck&token=%s&tpl=netdisk&apiver=v3&tt=%2&username=%3&isphone=false&callback=bd__cbs__ps6wfe")
                   .arg(BDISK_URL_PASSPORT_API).arg(QString::number(QDateTime::currentMSecsSinceEpoch()))
                   .arg(m_userName));
        qDebug()<<Q_FUNC_INFO<<"url "<<url;

        request = QNetworkRequest(url);
        fillRequest(&request);

        m_reply = networkMgr->get(request);
        if (m_reply) {
            connect(m_reply, &QNetworkReply::finished,
                    [&](){
                CHECK_IF_REPLY_ABORTED;
                CHECK_IF_REPLY_SUCCESS;

                QByteArray qba = m_reply->readAll();
//                qDebug()<<Q_FUNC_INFO<<" logincheck ok "<<qba;
                QByteArray value = truncateCallback(QString(qba)).toLocal8Bit();
//                qDebug()<<Q_FUNC_INFO<<" json value "<<value;

                QJsonParseError error;
                QJsonDocument doc = QJsonDocument::fromJson (value, &error);
                if (error.error != QJsonParseError::NoError) {
                    qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
                    m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                        QString("arse json error [%1]").arg(error.errorString()));
                    m_breakThread = true;
                } else {
                    QJsonObject obj = doc.object();
                    QJsonObject data = obj.value("data").toObject();
                    QString token = data.value("token").toString();
                    qDebug()<<Q_FUNC_INFO<<"logincheck token "<<token;
                }
//                freeReply();
                FREE_REPLY_AND_STOP_BLOCK;
            });
            DO_LOOP_BLOCK
        }

        CHECK_AND_BREAK_THREAD;

        DO_RESET_AND_FREE;

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
        m_reply = networkMgr->get(request);
        if (m_reply) {
            connect(m_reply, &QNetworkReply::finished,
                    [&](){
                CHECK_IF_REPLY_ABORTED;
                CHECK_IF_REPLY_SUCCESS;

                QByteArray qba = m_reply->readAll();
//                qDebug()<<Q_FUNC_INFO<<" getpublickey ok "<<qba;
                QByteArray value = truncateCallback(QString(qba)).toLocal8Bit();
//                qDebug()<<Q_FUNC_INFO<<" json value "<<value;

                QJsonParseError error;
                QJsonDocument doc = QJsonDocument::fromJson (value, &error);
                if (error.error != QJsonParseError::NoError) {
                    qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
                    m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                        QString("arse json error [%1]").arg(error.errorString()));
                    m_breakThread = true;
                } else {
                    QJsonObject obj = doc.object();
                    m_tokenProvider->setPubkey(obj.value("pubkey").toString().trimmed());
                    m_tokenProvider->setKey(obj.value("key").toString().trimmed());
                    qDebug()<<Q_FUNC_INFO<<" pubkey "<<m_tokenProvider->pubkey();
                    qDebug()<<Q_FUNC_INFO<<" key "<<m_tokenProvider->key();
                }
//                freeReply();
                FREE_REPLY_AND_STOP_BLOCK;
            });
            DO_LOOP_BLOCK;
        }
        CHECK_AND_BREAK_THREAD;
        DO_RESET_AND_FREE;

        /*
         * get codeString, this is also used for captch image
         * refresh captch image need to call this
         */

        //    QString captchaVerifyStr;
        QString str(BDISK_URL_PASSPORT_BASE);
        str += "?reggetcodestr";
        str += QString("&token=%1").arg(m_tokenProvider->token());
        str += "&tpl=netdisk&apiver=v3&fr=login&callback=bd__cbs__l6c16p";
        str += QString("tt=%1").arg(QString::number(QDateTime::currentMSecsSinceEpoch()));
        url = QUrl(str);
        qDebug()<<Q_FUNC_INFO<<"url "<<url;
        request = QNetworkRequest(url);
        fillRequest(&request);

        m_reply = networkMgr->get(request);
        if (m_reply) {
            connect(m_reply, &QNetworkReply::finished,
                    [&](){
                CHECK_IF_REPLY_ABORTED;
                CHECK_IF_REPLY_SUCCESS;
                QByteArray qba = m_reply->readAll();
//                qDebug()<<Q_FUNC_INFO<<" reggetcodestr ok "<<qba;
                QByteArray value = truncateCallback(QString(qba)).toLocal8Bit();
//                qDebug()<<Q_FUNC_INFO<<" json value "<<value;

                QJsonParseError error;
                QJsonDocument doc = QJsonDocument::fromJson (value, &error);
                if (error.error != QJsonParseError::NoError) {
                    qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
                    m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                        QString("parse json error [%1]").arg(error.errorString()));
                    m_breakThread = true;
                } else {
                    QJsonObject obj = doc.object();
                    QJsonObject data = obj.value("data").toObject();
                    m_tokenProvider->setCodeString(data.value("verifyStr").toString());
                    qDebug()<<Q_FUNC_INFO<<"codeString "<<m_tokenProvider->codeString();
                }
//                freeReply();
                FREE_REPLY_AND_STOP_BLOCK;
            });
            DO_LOOP_BLOCK;
        }
        CHECK_AND_BREAK_THREAD;
        DO_RESET_AND_FREE;

        if (!m_tokenProvider->codeString().isEmpty()) {

            //https://passport.baidu.com/cgi-bin/genimage?njGa006de4b5716f53302d914c2a6018300c6ad5b0671018a5b
            m_captchaImgUrl = QUrl(QString("%1?%2").arg(BDISK_URL_CAPTCHA).arg(m_tokenProvider->codeString()));
            qDebug()<<Q_FUNC_INFO<<" m_captchaImgUrl "<<m_captchaImgUrl;
            m_handler->dispatch(InnerEvent::EVENT_CAPTCHA_URL, m_captchaImgUrl);

            /*
            * lock thread to wait captcha
            */
            QMutex lock;
            qDebug()<<Q_FUNC_INFO<<"============= lock thread";
            lock.lock();
            m_wait.wait(&lock);
        }

        qDebug()<<Q_FUNC_INFO<<"======== continue thread";

        url = QUrl(QString("%1?login").arg(BDISK_URL_PASSPORT_API));

        QString postStr;
        postStr += "staticpage=http://pan.baidu.com/res/static/thirdparty/pass_v3_jump.html&";
        postStr += "charset=utf-8&";
        postStr += QString("token=%1&").arg(m_tokenProvider->token());
        postStr += "tpl=netdisk&";
        postStr += "subpro=&";
        postStr += "apiver=v3&";
        postStr += QString("tt=%1&").arg(QString::number(QDateTime::currentMSecsSinceEpoch()));
        postStr += QString("codestring=%1&").arg(m_tokenProvider->codeString());
        postStr += "safeflg=0&";
        postStr += "u=http://pan.baidu.com/&";
        postStr += "isPhone=&";
        postStr += "quick_user=0&";
        postStr += "logintype=basicLogin&";
        postStr += "logLoginType=pc_loginBasic&";
        postStr += "idc=&";
        postStr += "loginmerge=true&";
        postStr += QString("username=%1&").arg(m_userName);
        postStr += QString("password=%1&").arg(m_passWord);
        postStr += QString("verifycode=%1&").arg(m_captchaText);
        //    postStr += "mem_pass=on&";
        //    postStr += "rsakey=&";// rsa_pwd ? pcs->key : "&";
        //    postStr += "crypttype=&";// rsa_pwd ? "12" : "&";
        postStr += "ppui_logintime=1319881&";
        postStr += "callback=parent.bd__pcbs__7vxzg7";

        request = QNetworkRequest(url);
        fillRequest(&request);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(postStr.length()));

        qDebug()<<Q_FUNC_INFO<<" post data ["<<postStr<<"] to ["<<url<<"]";

        m_reply = networkMgr->post(request, postStr.toLocal8Bit());
        if (m_reply) {
            connect(m_reply, &QNetworkReply::finished,
                    [&](){
                CHECK_IF_REPLY_ABORTED;
                CHECK_IF_REPLY_SUCCESS;
                QByteArray qba = m_reply->readAll();
                qDebug()<<Q_FUNC_INFO<<" post login ok "<<qba;
                int ret = getErrorFromPostData(qba);

                /*
                 * see https://github.com/GangZhuo/BaiduPCS/issues/29
                 */
                if (ret == 3 || ret == 6 || ret == 257 || ret == 200010) {
                    QString codeStr = getCodeStringFromPostData(qba);
                    if (codeStr.isEmpty()) {
                        m_tokenProvider->setCodeString(QString());
                        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                            QString("Can't read the codestring from the response [%1]")
                                            .arg(QString(qba)));
                        m_breakThread = true;
                    } else {
                        m_tokenProvider->setCodeString(codeStr);
                        qDebug()<<Q_FUNC_INFO<<">>>> send signal to re-start thread";

                        /// we'll restart whole thread when receive this signal atm,
                        /// as we're using thread and loop event to block QNetworkAccessManager asynchronous call
                        m_handler->dispatch(InnerEvent::EVENT_CAPTCHA_URL_NEED_REFRESH,
                                            QString("Error code = %1").arg(QString::number(ret)));
                        //FIX set m_loginFailureOrAborted to avoid sending EVENT_LOGIN_SUCCESS at thread end
                        m_loginFailureOrAborted = true;
                        m_breakThread = true;
                    }
                } else {
                    /// as we're using event loop to block QNetworkAccessManager asynchronous call
                    /// this flag will indicate wether we need to check cookies login
                    m_loginErrCode = ret;
                }
//                freeReply();
                FREE_REPLY_AND_STOP_BLOCK;
            });
            DO_LOOP_BLOCK;
        }
        CHECK_AND_BREAK_THREAD;
        DO_RESET_AND_FREE;

        qDebug()<<Q_FUNC_INFO<<">>>>>  m_loginErrCode "<<m_loginErrCode;

        if (m_loginErrCode != -1) {
            if (m_loginErrCode == 0 || m_loginErrCode == 18 || m_loginErrCode == 120016
                    || m_loginErrCode == 400032 || m_loginErrCode == 400034
                    || m_loginErrCode == 400037 || m_loginErrCode == 400401) {
                url = QUrl(BDISK_URL_DISK_HOME);

                /*
                 *  use loop for cookies login check if url redirected
                 */

                /// sine we use block event for QNetworkAccessManager asynchronous call
                /// it is ok to add breakFlag to QNetworkAccessManager lambda call
                bool breakFlag = false;
                do {
                    freeReply();
                    reset();

                    request = QNetworkRequest(url);
                    fillRequest(&request);
                    m_reply = networkMgr->head(request);
                    if (m_reply) {
                        connect(m_reply, &QNetworkReply::finished,
                                [&](){
                            if (m_requestAborted) {
                                m_requestAborted = false;
                                m_handler->dispatch(InnerEvent::EVENT_LOGIN_ABORT);
                                m_breakThread = true;
                                if (timer.isActive()) timer.stop();
                                if (loop.isRunning()) loop.quit();
                                breakFlag = true;
                                return;
                            }
                            QNetworkReply::NetworkError error = m_reply->error ();
                            bool success = (error == QNetworkReply::NoError);
                            if (!success) {
                                qDebug()<<Q_FUNC_INFO<<" check login head error "<<m_reply->errorString();
                                m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE, m_reply->errorString());
                                m_breakThread = true;
                                if (timer.isActive()) timer.stop();
                                if (loop.isRunning()) loop.quit();
                                breakFlag = true;
                            } else {
                                bool ok = false;
//                                int ret = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(&ok);
                                QVariant vrt = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
                                qDebug()<<Q_FUNC_INFO<<"==== http status code "<<vrt;
                                if (vrt.isNull() || !vrt.isValid()) {
                                    m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                                        QString("Get http status code error!"));
                                    m_breakThread = true;
                                    if (timer.isActive()) timer.stop();
                                    if (loop.isRunning()) loop.quit();
                                    breakFlag = true;
                                }
                                int ret = vrt.toInt(&ok);
                                if (!ok) {
                                    m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                                        QString("Can't get http status code!"));
                                    m_breakThread = true;
                                    if (timer.isActive()) timer.stop();
                                    if (loop.isRunning()) loop.quit();
                                    breakFlag = true;
                                } else if (ret == 200) {
                                    if (timer.isActive()) timer.stop();
                                    if (loop.isRunning()) loop.quit();
                                    breakFlag = true;
                                } else {
                                    if (ret == 302) {
                                        //302 //redirect url
                                        url = m_reply->header(QNetworkRequest::LocationHeader).toUrl();
                                        qDebug()<<Q_FUNC_INFO<<" redirect : "<<url;
                                        if (timer.isActive()) timer.stop();
                                        if (loop.isRunning()) loop.quit();
                                    } else {
                                        m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                                            QString("Wrong http status code [%1]").arg(QString::number((ret))));
                                        m_breakThread = true;
                                        if (timer.isActive()) timer.stop();
                                        if (loop.isRunning()) loop.quit();
                                        breakFlag = true;
                                    }
                                }
                            }
//                            freeReply();
                            FREE_REPLY_AND_STOP_BLOCK;
                        });
                        DO_LOOP_BLOCK;
                    }
                } while (!breakFlag);

                CHECK_AND_BREAK_THREAD;
                DO_RESET_AND_FREE;

                url = QUrl(BDISK_URL_DISK_HOME);
                request = QNetworkRequest(url);
                fillRequest(&request);
                m_reply = networkMgr->get(request);

                if (m_reply) {
                    connect(m_reply, &QNetworkReply::finished,
                            [&](){
                        CHECK_IF_REPLY_ABORTED;
                        CHECK_IF_REPLY_SUCCESS;

                        QByteArray qba = m_reply->readAll();
                        //TODO get bdstoken from response by key yunData.MYBDSTOKEN or FileUtils.bdstoken
                        if (m_tokenProvider->bdstoken().isEmpty()) {
                            QString value = truncateYunData(QString(qba));

                            QJsonParseError error;
                            QJsonDocument doc = QJsonDocument::fromJson (value.toLocal8Bit(), &error);
                            if (error.error != QJsonParseError::NoError) {
                                qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
                                m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                                    QString("Parse json error [%1]").arg(error.errorString()));
                                m_breakThread = true;
                            } else {
                                QJsonObject obj = doc.object();
                                QString bdstoken = obj.value("bdstoken").toString();
                                if (bdstoken.isEmpty()) {
                                    m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                                        QString("Can't get bdstoken"));
                                    m_breakThread = true;
                                } else {
                                    m_tokenProvider->setBdstoken(bdstoken);
                                }
                                QString uname = obj.value("username").toString();
                                //FIXME send failure if uname is empty
                                m_tokenProvider->setUidStr(uname);
                            }
                        }
//                        freeReply();
                        FREE_REPLY_AND_STOP_BLOCK;
                    });
                    DO_LOOP_BLOCK;
                }
            } else {
                qDebug()<<Q_FUNC_INFO<<"Login error, Error code "<<m_loginErrCode;
                m_tokenProvider->setCodeString(QString());
                m_handler->dispatch(InnerEvent::EVENT_LOGIN_FAILURE,
                                    QString("Try to login error code = %1").arg(QString::number(m_loginErrCode)));
                m_breakThread = true;
            }
        }
        m_breakThread = true;
    } while (true);

    if (!m_loginFailureOrAborted)
        m_handler->dispatch(InnerEvent::EVENT_LOGIN_SUCCESS);

    freeReply();
    reset();

    networkMgr->deleteLater();
    networkMgr = nullptr;

}

void BDiskLogin::setUserName(QString userName)
{
    if (m_userName == userName)
        return;

    m_userName = userName;
    emit userNameChanged(userName);
}

void BDiskLogin::setPassWord(QString passWord)
{
    if (m_passWord == passWord)
        return;

    m_passWord = passWord;
    emit passWordChanged(passWord);
}

void BDiskLogin::setCaptchaText(QString captchaText)
{
    if (m_captchaText == captchaText)
        return;

    m_captchaText = captchaText;
    emit captchaTextChanged(captchaText);
}

QString BDiskLogin::truncateCallback(const QString &callback)
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

QString BDiskLogin::truncateYunData(const QString &data)
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

int BDiskLogin::getErrorFromPostData(const QByteArray &data)
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

QString BDiskLogin::getCodeStringFromPostData(const QByteArray &data)
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

void BDiskLogin::reset()
{
    m_requestAborted = false;
    m_breakThread = false;
    m_loginFailureOrAborted = false;
}

void BDiskLogin::freeReply()
{
    if (m_reply) {
        if (!m_reply->isFinished()) {
            m_requestAborted = true;
            m_reply->abort();
        }
        QObject::disconnect(m_reply, 0, 0, 0);
        m_reply->deleteLater();
        m_reply = nullptr;
    }
}

void BDiskLogin::fillRequest(QNetworkRequest *req)
{
    if (!req)
        return;
    req->setRawHeader("User-Agent", "Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0");
    req->setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
}


#include "BDiskLogin.moc"
