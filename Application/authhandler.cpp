#include "authhandler.h"
#include <QDebug>
#include <QVariantMap>
#include <QNetworkRequest>
#include <QJsonObject>

#include "mainwindow.h"

DatabaseHandler::DatabaseHandler(QObject *parent)
    : QObject{parent}
{
    m_networkAccessManager = new QNetworkAccessManager(this);
    m_networkAccessManagerWithPatch = new QNetworkAccessManagerWithPatch(this);
    /* API Key was hidden, but must be defined using setAPIKey(<API Key>)*/
    signinErroremail = false;
    signinErrorpassword = false;
    signupError = false;
}

DatabaseHandler::~DatabaseHandler()
{
    m_networkAccessManager->deleteLater();
}

void DatabaseHandler::setAPIKey(const QString &apikey)
{
    m_apiKey = apikey;
}

void DatabaseHandler::signUserUp(const QString &emailAddress, const QString &password)
{
    QString signUserEndPoint = "https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=" + m_apiKey;

    QVariantMap payload;
    payload["email"] = emailAddress;
    payload["password"] = password;
    payload["returnSecureToken"] = true;

    QJsonDocument jdoc = QJsonDocument::fromVariant(payload);
    performPostSignUp(signUserEndPoint, jdoc);
}

void DatabaseHandler::signUserIn(const QString &emailAddress, const QString &password)
{
    QString signInEndPoint = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=" + m_apiKey;

    QVariantMap payload;
    payload["email"] = emailAddress;
    payload["password"] = password;
    payload["returnSecureToken"] = true;

    QJsonDocument jdoc = QJsonDocument::fromVariant(payload);
    performPostSignIn(signInEndPoint, jdoc);
}

QString DatabaseHandler::getUserid()
{
    return userid;
}

void DatabaseHandler::networkReplyReadyReadSignUp()
{
    //QByteArray aux = m_networkReply->readAll();
    parseSignUpResponse(m_networkReply->readAll());
    if(!signupError)
    {
      setDatabase();
    }
    m_networkReply->deleteLater();
}

void DatabaseHandler::networkReplyReadyReadSignIn()
{
    //QByteArray aux = m_networkReply->readAll();
    parseSignInResponse(m_networkReply->readAll());
    m_networkReply->deleteLater();
}

void DatabaseHandler::performPostSignUp(const QString &url, const QJsonDocument &payload)
{
    QNetworkRequest newRequest((QUrl(url)));
    newRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    m_networkReply = m_networkAccessManager->post(newRequest, payload.toJson());
    connect( m_networkReply, &QNetworkReply::readyRead, this, &DatabaseHandler::networkReplyReadyReadSignUp);
}

void DatabaseHandler::performPostSignIn(const QString &url, const QJsonDocument &payload)
{
    QNetworkRequest newRequest((QUrl(url)));
    newRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    m_networkReply = m_networkAccessManager->post(newRequest, payload.toJson());
    connect( m_networkReply, &QNetworkReply::readyRead, this, &DatabaseHandler::networkReplyReadyReadSignIn);
}


void DatabaseHandler::parseSignUpResponse(const QByteArray &response)
{
    QJsonDocument json = QJsonDocument::fromJson(response);

    if(json.object().contains("error"))
        signupError = true;
    else
      {
        signupError = false;
        userid = json.object().value("localId").toString();
        qDebug() << userid;
      }
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
        //qDebug() << userid;
    }
    signinErroremail = false;
    signinErrorpassword = false;
}

void DatabaseHandler::setDatabase()
{
    QUrl url = QUrl("https://projeto-gamma.firebaseio.com/users/" + userid + "/parameters.json");
    m_networkReply = m_networkAccessManager->get(QNetworkRequest(url));

    QVariantMap newuser;
    m_networkReply = m_networkAccessManager->get(QNetworkRequest(url));
    newuser["temperature"] = 0;
    newuser["humidity"] = 0;
    newuser["live_flag"] = 0;
    newuser["notification_flag"] = 0;
    newuser["swing_flag"] = 0;

    QJsonDocument jdoc = QJsonDocument::fromVariant(newuser);
    QNetworkRequest newUser(url);
    newUser.setHeader( QNetworkRequest::ContentTypeHeader , QString("application/json"));
    m_networkAccessManagerWithPatch->patch(newUser, jdoc.toJson());
}

