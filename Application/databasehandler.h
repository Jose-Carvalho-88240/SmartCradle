#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include "QNetworkAcessManagerWithPatch.h"

class DatabaseHandler : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseHandler(QObject *parent = nullptr);

    void updateValues();

    void clear();

    void sendLiveFlag(int value);
    void setclearLiveFlag( int value);

    void sendSwingFlag(int value);
    void setclearSwingFlag( int value);

    void sendNotificationFlag(int value);
    void setclearNotificationFlag( int value);

    float getTemperature();
    float getHumidity();
    int getNotificationFlag();
    int getSwingFlag();
    int getLiveFlag();

    void setAPIKey( const QString & apikey);
    void signUserUp( const QString & emailAddress , const QString & password );
    void signUserIn( const QString & emailAddress , const QString & password );
    QString getUserid();
    bool signupError;
    bool signinErroremail;
    bool signinErrorpassword;

    ~DatabaseHandler();

private:
    void networkReplyReadyReadSignUp();
    void networkReplyReadyReadSignIn();
    QNetworkAccessManager * m_networkManager;
    QNetworkAccessManagerWithPatch * m_networkManagerWithPatch;
    QNetworkReply * m_networkReply;
    QUrl url;
    void setQUrl(const QString & userid);
    void performPostSignUp(const QString & url, const QJsonDocument & payload);
    void performPostSignIn(const QString & url, const QJsonDocument & payload);
    void parseSignUpResponse(const QByteArray &response);
    void parseSignInResponse(const QByteArray &response);
    void setDatabase();
    QString m_apiKey;
    QString userid;

    void parseFlags(const QByteArray &response);
    float temperature;
    float humidity;
    int live_flag;
    int swing_flag;
    int notification_flag;
};

#endif // DATABASEHANDLER_H
