#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtAV>
#include <QtAVWidgets>
#include "notificationclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace QtAV;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTimer *timer;

private slots:
    void on_SignInMainScreenButton_clicked();

    void on_CreateAccMainScreenButton_clicked();

    void on_GoBackLogInScreenButton_clicked();

    void on_GoBackCreateAccScreenButton_clicked();

    void on_CreateAccCreateAccScreenButton_clicked();

    void on_SignInLogInScreenButton_clicked();


    void on_SignOut_Button_clicked();

    void on_Livestream_Button_clicked();

    void on_Swing_Button_clicked();

public slots:
    void updateAtInterval();

    void updateDisplayedValues();

signals:
    void updateDisplay();

private:
    Ui::MainWindow *ui;
    NotificationClient *notificationClient;
    QtAV::AVPlayer *player;
    QtAV::VideoOutput *v;
};
#endif // MAINWINDOW_H
