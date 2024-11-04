#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QBluetoothDeviceDiscoveryAgent>
#include <string>
#include <iostream>
#include <QBluetoothSocket>
#include <QKeyEvent>
#include "ui_mainwindow.h"
#include <QPalette>
#include "GCodeCommands.h"
#include "cart.h"
#include <QAbstractButton>
#include <QTimer>

#define MAX_ANGLE 8
#define SPEED_ERR 5
#define ANGLE_ERR 0.5

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addToLogs(QString a);

    void stopSendingMsg(){
        while(!this->cartData.lockData()){};
        this->cartData.setCloseConnectionPls(true);
        this->cartData.setSendingMsg(false);
        this->cartData.unlockData();
    }
    void startSendingMsg(){
        while(!this->cartData.lockData()){};
        this->cartData.setOpenConnectionPls(true);
        this->cartData.setSendingMsg(true);
        this->cartData.unlockData();
    }

private:
    Ui::MainWindow *ui;

    void keyPressEvent (QKeyEvent * event);
    void keyReleaseEvent (QKeyEvent * event);
    cart *mCartThr;
    dataStr cartData;

    QTimer *mTimer;

    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothSocket *socket;

public slots:

    void on_pushButtonConnect_clicked();

    void on_pushButtonCloseConnection_clicked();
    void on_pushButtonSendMessage_clicked();

    void connectionEstablished();
    void connectionInterrupted();
    void newMsgGetted();

    void on_pushButtonClear_clicked();
    void on_horizontalSlider_speed_valueChanged(int value);
    void on_horizontalSlider_angle_valueChanged(int value);
    void on_pushButtonConnect_Batteries_clicked();
    void on_pushButton_PrintCommands_clicked();

    void dupaSlot(){

        QString msg = "";
        if(!cartData.getPressedStraightKey()){
                cartData.setSpeed(0);
        }

        if(!cartData.getPressedTurnKey()){
                cartData.setAngle(0);
        }

        if(socket->state() == QBluetoothSocket::ConnectedState){

            msg = "G1 ";

            msg+="L"+QString::number(cartData.getSpeed()*10,'d',2)+" ";
            msg +="A"+QString::number(cartData.getAngle()*35,'d',2);
            msg += " " + QString::number(crc16(msg)) +"\n";
//                qDebug() <<"message: " << msg;
            socket->write(msg.toStdString().c_str());
            qDebug()<<msg;

        }
    }
    void socketReadyToRead(){
        while(this->socket->canReadLine()) {
            QString line = this->socket->readLine();
            //qDebug() << line;

            QString terminator = "\n";
            int pos = line.lastIndexOf(terminator);
            QString sss = line.left(pos);
            //      qDebug() << line.left(pos);
//            while(!this->cartDataStr->lockData()){}
            if(sss != "OK"){
                addToLogs(line.left(pos));
            }

//            this->cartDataStr->addToQueue(line.left(pos));
//            this->cartDataStr->unlockData();


            //       this->addToLogs(line);
        }
    }

signals:
    void start(dataStr *);
    void stop();
    void goSleep();
    void wakeUp();

    void sendMsg(QString  s);
};
#endif // MAINWINDOW_H
