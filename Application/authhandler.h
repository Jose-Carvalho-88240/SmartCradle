#ifndef AUTHHANDLER_H
#define AUTHHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include "QNetworkAcessManagerWithPatch.h"


class DatabaseHandler : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseHandler(QObject *parent = nullptr);
    ~DatabaseHandler();
    void setAPIKey( const QString & apikey);
    void signUserUp( const QString & emailAddress , const QString & password );
    void signUserIn( const QString & emailAddress , const QString & password );
    QString getUserid();
    bool signupError;
    bool signinErroremail;
    bool signinErrorpassword;


public slots:
    void networkReplyReadyReadSignUp();
    void networkReplyReadyReadSignIn();

signals:

private:
    void performPostSignUp(const QString & url, const QJsonDocument & payload);
    void performPostSignIn(const QString & url, const QJsonDocument & payload);
    void parseSignUpResponse(const QByteArray &response);
    void parseSignInResponse(const QByteArray &response);
    void setDatabase();
    QString m_apiKey;
    QString userid;
    QNetworkAccessManager * m_networkAccessManager;
    QNetworkAccessManagerWithPatch * m_networkAccessManagerWithPatch;
    QNetworkReply * m_networkReply;

};

#endif // AUTHHANDLER_H
