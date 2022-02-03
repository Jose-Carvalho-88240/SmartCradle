#include "databasehandler.h"
#include <QNetworkRequest>
#include <QtDebug>
#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>


DatabaseHandler::DatabaseHandler(QObject *parent)
    : QObject{parent}
{
    m_networkManager = new QNetworkAccessManager(this);
    m_networkManagerWithPatch = new QNetworkAccessManagerWithPatch(this);
    /* API Key was hidden, but must be defined using setAPIKey(<API Key>)*/
    signinErroremail = false;
    signinErrorpassword = false;
    signupError = false;
}

/*******************************************Sets************************************************************/
void DatabaseHandler::setQUrl(const QString & userid)
{
    url = QUrl("https://projeto-gamma.firebaseio.com/users/" + userid + "/parameters.json");
}

void DatabaseHandler::setAPIKey(const QString &apikey)
{
    m_apiKey = apikey;
}

QString DatabaseHandler::getUserid()
{
    return userid;
}
/***********************************************************************************************************/

/*******************************************SignIn**********************************************************/
/**
 * @brief DatabaseHandler::signUserIn
 * @param emailAddress
 * @param password
 */
void DatabaseHandler::signUserIn(const QString &emailAddress, const QString &password)
{
    QString signInEndPoint = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=" + m_apiKey;

    QVariantMap payload;
    payload["email"] = emailAddress;
    payload["password"] = password;
    payload["returnSecureToken"] = true;
    QJsonDocument jdoc = QJsonDocument::fromVariant(payload);

    QNetworkRequest newRequest((QUrl(signInEndPoint)));
    newRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    m_networkReply = m_networkManager->post(newRequest, jdoc.toJson());
    QEventLoop loop;
    connect(m_networkReply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    parseSignInResponse(m_networkReply->readAll());
    m_networkReply->deleteLater();
}

void DatabaseHandler::parseSignInResponse(const QByteArray &response)
{
    QJsonDocument json = QJsonDocument::fromJson(response);
    //qDebug() << response;
    if(json.object().contains("error"))
    {
        QVariant map = json.object().value("error").toVariant();
        QJsonObject obj = map.toJsonObject();

        if(obj.value("message").toString() == "INVALID_PASSWORD" )
        {
            signinErrorpassword = true;
            //qDebug() << "invalid password";
        }
        else if(obj.value("message").toString() == "EMAIL_NOT_FOUND")
        {
            signinErroremail = true;
            //qDebug() << "invalid email";
        }
    }
    else
    {
        signinErroremail = false;
        signinErrorpassword = false;
        userid = json.object().value("localId").toString();
        setQUrl(userid);
        //qDebug() << userid;
        //qDebug() << url;
        updateValues();
    }
}
/***********************************************************************************************************/

/*******************************************SignUp**********************************************************/
/**
 * @brief DatabaseHandler::signUserUp
 * @param emailAddress
 * @param password
 */
void DatabaseHandler::signUserUp(const QString &emailAddress, const QString &password)
{
    QString signUserEndPoint = "https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=" + m_apiKey;

    QVariantMap payload;
    payload["email"] = emailAddress;
    payload["password"] = password;
    payload["returnSecureToken"] = true;
    QJsonDocument jdoc = QJsonDocument::fromVariant(payload);

    QNetworkRequest newRequest((QUrl(signUserEndPoint)));
    newRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    m_networkReply = m_networkManager->post(newRequest, jdoc.toJson());
    QEventLoop loop;
    connect(m_networkReply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    parseSignUpResponse(m_networkReply->readAll());
    if(!signupError)
    {
      setDatabase();
    }
    m_networkReply->deleteLater();
}


void DatabaseHandler::parseSignUpResponse(const QByteArray &response)
{
    QJsonDocument json = QJsonDocument::fromJson(response);
    qDebug() << response;
    if(json.object().contains("error"))
        signupError = true;
    else
      {
        signupError = false;
        userid = json.object().value("localId").toString();
        //qDebug() << userid;
      }
}

void DatabaseHandler::setDatabase()
{
    QUrl url = QUrl("https://projeto-gamma.firebaseio.com/users/" + userid + "/parameters.json");
    m_networkReply = m_networkManager->get(QNetworkRequest(url));

    QVariantMap newuser;
    m_networkReply = m_networkManager->get(QNetworkRequest(url));
    newuser["temperature"] = 0;
    newuser["humidity"] = 0;
    newuser["live_flag"] = 0;
    newuser["notification_flag"] = 0;
    newuser["swing_flag"] = 0;

    QJsonDocument jdoc = QJsonDocument::fromVariant(newuser);
    QNetworkRequest newUser(url);
    newUser.setHeader( QNetworkRequest::ContentTypeHeader , QString("application/json"));
    m_networkManagerWithPatch->patch(newUser, jdoc.toJson());
}
/************************************************************************************************************/

/*******************************************Get Stuff********************************************************/
/**
 * @brief DatabaseHandler::updateValues
 */
void DatabaseHandler::updateValues()
{
    m_networkReply = m_networkManager->get(QNetworkRequest(url));
    QEventLoop loop;
    connect(m_networkReply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    parseFlags(m_networkReply->readAll());
    m_networkReply->deleteLater();

}

void DatabaseHandler::clear()
{
    url = NULL;
    userid = nullptr;
    temperature = NULL;
    humidity = NULL;
    live_flag = NULL;
    swing_flag = NULL;
    notification_flag = NULL;
    signinErroremail = 0;
    signinErrorpassword = 0;
    signupError = 0;
}

void DatabaseHandler::sendLiveFlag( int value)
{
    if(value == 1 || value == 0)
    {
    QVariantMap newuser;
    newuser["live_flag"] = value;

    QJsonDocument jdoc = QJsonDocument::fromVariant(newuser);
    QNetworkRequest newUser(url);
    newUser.setHeader( QNetworkRequest::ContentTypeHeader , QString("application/json"));
    m_networkManagerWithPatch->patch(newUser, jdoc.toJson());

    }

}

void DatabaseHandler::setclearLiveFlag(int value)
{
    if(value == 1 || value == 0)
        live_flag = value;
    else
    {
        live_flag = 0;
    }
}

void DatabaseHandler::sendSwingFlag(int value)
{
    if(value == 1 || value == 0)
    {

    QVariantMap newuser;
    newuser["swing_flag"] = value;

    QJsonDocument jdoc = QJsonDocument::fromVariant(newuser);
    QNetworkRequest newUser(url);
    newUser.setHeader( QNetworkRequest::ContentTypeHeader , QString("application/json"));
    m_networkManagerWithPatch->patch(newUser, jdoc.toJson());

    }
}

void DatabaseHandler::setclearSwingFlag(int value)
{
    if(value == 1 || value == 0)
        swing_flag = value;
    else
    {
        swing_flag = 0;
    }
}

void DatabaseHandler::sendNotificationFlag(int value)
{
    QVariantMap newuser;
    newuser["notification_flag"] = value;

    QJsonDocument jdoc = QJsonDocument::fromVariant(newuser);
    QNetworkRequest newUser(url);
    newUser.setHeader( QNetworkRequest::ContentTypeHeader , QString("application/json"));
    m_networkManagerWithPatch->patch(newUser, jdoc.toJson());
}

void DatabaseHandler::setclearNotificationFlag(int value)
{
    if(value == 1 || value == 0)
        swing_flag = value;
    else
    {
        notification_flag = 0;
    }
}

float DatabaseHandler::getTemperature()
{
    return temperature;
}

float DatabaseHandler::getHumidity()
{
    return humidity;
}

int DatabaseHandler::getNotificationFlag()
{
    return notification_flag;
}

int DatabaseHandler::getSwingFlag()
{
    return swing_flag;
}

int DatabaseHandler::getLiveFlag()
{
    return live_flag;
}

void DatabaseHandler::parseFlags(const QByteArray &response)
{
    qDebug() << response;
    QJsonDocument jdoc = QJsonDocument::fromJson(response);

    temperature = jdoc.object().value("temperature").toInt();
    humidity = jdoc.object().value("humidity").toInt();
    live_flag = jdoc.object().value("live_flag").toInt();
    swing_flag = jdoc.object().value("swing_flag").toInt();
    notification_flag = jdoc.object().value("notification_flag").toInt();

}

DatabaseHandler::~DatabaseHandler()
{
    m_networkManager->deleteLater();
}
