#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "databasehandler.h"
#include <QThread>
#include <QDebug>

DatabaseHandler db;
int live_flag=0;
int swing_flag=0;
int notification_flag = 0;
using namespace QtAV;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    timer = new QTimer(this);
    QtAV::Widgets::registerRenderers();
    notificationClient = new NotificationClient(this);
    connect(timer, &QTimer::timeout ,this ,&MainWindow::updateAtInterval);
    connect(this , &MainWindow::updateDisplay, this, &MainWindow::updateDisplayedValues);
    ui->setupUi(this);
    player = new QtAV::AVPlayer(this);
    v = new QtAV::VideoOutput(this);
    player->setRenderer(v);
    //v->widget()->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
    //v->widget()->setMaximumSize(330,470);
    v->widget()->setMinimumSize(338,470);
    ui->livestream->addWidget(v->widget());
    ui->livestream->setAlignment(Qt::AlignCenter);
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_SignInMainScreenButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_CreateAccMainScreenButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void MainWindow::on_GoBackLogInScreenButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_GoBackCreateAccScreenButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->CreateAcc_email->clear();
    ui->CreateAcc_password->clear();
    ui->CreateAcc_passwordRepeat->clear();
    ui->CreateAcc_label->clear();
}


void MainWindow::on_CreateAccCreateAccScreenButton_clicked()
{
    QString password = ui->CreateAcc_password->text();
    if(password != ui->CreateAcc_passwordRepeat->text())
    {
        QPalette sample_palette;
        sample_palette.setColor(QPalette::WindowText, Qt::red);
        ui->CreateAcc_label->setAutoFillBackground(true);
        ui->CreateAcc_label->setPalette(sample_palette);
        ui->CreateAcc_label->setText("Passwords do not match");
        ui->CreateAcc_password->clear();
        ui->CreateAcc_passwordRepeat->clear();
        return;
    }

    if(ui->CreateAcc_passwordRepeat->text().size() < 6)
    {
        QPalette sample_palette;
        sample_palette.setColor(QPalette::WindowText, Qt::red);
        ui->CreateAcc_label->setAutoFillBackground(true);
        ui->CreateAcc_label->setPalette(sample_palette);
        ui->CreateAcc_label->setText("Passwords must have at least 6 characters");
        ui->CreateAcc_password->clear();
        ui->CreateAcc_passwordRepeat->clear();
        return;
    }
    db.signUserUp(ui->CreateAcc_email->text(), ui->CreateAcc_password->text());
    if(db.signupError)
    {
        db.signupError = false;
        QPalette sample_palette;
        sample_palette.setColor(QPalette::WindowText, Qt::red);
        ui->CreateAcc_label->setAutoFillBackground(true);
        ui->CreateAcc_label->setPalette(sample_palette);
        ui->CreateAcc_label->setText("Email already in use or incorrect form");
        ui->CreateAcc_email->clear();
        ui->CreateAcc_password->clear();
        ui->CreateAcc_passwordRepeat->clear();
        return;
    }
    else
    {
        QPalette sample_palette;
        sample_palette.setColor(QPalette::WindowText, QColor::fromRgb(50,205,50));
        ui->CreateAcc_label->setAutoFillBackground(true);
        ui->CreateAcc_label->setPalette(sample_palette);
        ui->CreateAcc_label->setText("Account created successfully");
        ui->CreateAcc_email->clear();
        ui->CreateAcc_password->clear();
        ui->CreateAcc_passwordRepeat->clear();
    }
}

void MainWindow::on_SignInLogInScreenButton_clicked()
{
    db.signUserIn(ui->LogIn_email->text(),ui->LogIn_password->text());
    if(db.signinErroremail)
    {
        QPalette sample_palette;
        sample_palette.setColor(QPalette::WindowText,Qt::red);
        ui->LogInScreen_label->setAutoFillBackground(true);
        ui->LogInScreen_label->setPalette(sample_palette);
        ui->LogInScreen_label->setText("Email not found");
        ui->LogIn_password->clear();
        ui->LogIn_email->clear();
    }
    else if(db.signinErrorpassword)
    {
        QPalette sample_palette;
        sample_palette.setColor(QPalette::WindowText,Qt::red);
        ui->LogInScreen_label->setAutoFillBackground(true);
        ui->LogInScreen_label->setPalette(sample_palette);
        ui->LogInScreen_label->setText("Wrong Password");
        ui->LogIn_password->clear();
        ui->LogIn_email->clear();
    }
    else
    {
        timer->setInterval(10000);
        timer->start();
        ui->LogIn_password->clear();
        ui->LogIn_email->clear();
        ui->LogInScreen_label->clear();
        ui->stackedWidget->setCurrentIndex(3); //go to main screen
        //write the values to the labels
        ui->temp_label->setText(QString::number(db.getTemperature() , 'f', 2));
        ui->hum_label->setText(QString::number(db.getHumidity() , 'f', 2));
        live_flag = 0;
        swing_flag = 0;
        db.sendLiveFlag(live_flag);
        db.sendSwingFlag(swing_flag);
    }
}

void MainWindow::updateAtInterval()
{
    db.updateValues();
    notification_flag = db.getNotificationFlag();
    if(notification_flag == 1)
    {
        notification_flag = 0;
        notificationClient->setNotification("Please confirm by watching the stream.");
        db.sendNotificationFlag(notification_flag);
        db.setclearNotificationFlag(0);

    }
    emit updateDisplay();
}

void MainWindow::updateDisplayedValues()
{
    ui->temp_label->setText(QString::number(db.getTemperature() , 'f', 2));
    ui->hum_label->setText(QString::number(db.getHumidity() , 'f', 2));
}

void MainWindow::on_SignOut_Button_clicked()
{
    timer->stop();
    live_flag = 0;
    swing_flag = 0;
    db.sendLiveFlag(live_flag);
    db.sendSwingFlag(swing_flag);
    db.clear();
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_Livestream_Button_clicked() //TO DO -> VIDEO WIDGET pos 3,0
{
    if(live_flag == 0)
    {
        live_flag=1;
        ui->Livestream_Button->setStyleSheet("background-color: rgb(50, 205, 50);color: white;border-radius:14px;font: bold; padding: 4px;");
        db.sendLiveFlag(live_flag);
        db.setclearLiveFlag(live_flag);
        player->play("rtmp://192.168.43.100/live");
    }
    else
    {
        live_flag=0;
       ui->Livestream_Button->setStyleSheet("background-color: rgb(52, 84, 136);color: white;border-radius:14px;font: bold; padding: 4px;");
       player->stop();
       db.sendLiveFlag(live_flag);
       db.setclearLiveFlag(live_flag);
    }
}

void MainWindow::on_Swing_Button_clicked()
{
    if(swing_flag == 0)
    {
        swing_flag = 1;
        ui->Swing_Button->setStyleSheet("background-color: rgb(50, 205, 50);color: white;border-radius:14px;font: bold; padding: 4px;");
        db.sendSwingFlag(swing_flag);
        db.setclearSwingFlag(swing_flag);
    }
    else
    {
       swing_flag = 0;
       ui->Swing_Button->setStyleSheet("background-color: rgb(52, 84, 136);color: white;border-radius:14px;font: bold; padding: 4px;");
       db.sendSwingFlag(swing_flag);
       db.setclearSwingFlag(swing_flag);
    }
}





