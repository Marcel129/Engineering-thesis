#include "mainwindow.h"
#include "ui_mainwindow.h"

QString GCodeCommands0="Available commands:\n";
QString GCodeCommands1="G1 S[SPEED_M/S] A[ROTATION_ANGLE_DEG] - set linear vehicle speed and rotary angle of turning axis\n";
QString GCodeCommands2="G4 S[SPEED_M/S] - set max speed\n";
QString GCodeCommands3="G28 - base axes, same as G1 S0 A0\n";
QString GCodeCommands4="\n";
QString GCodeCommands5="Communication with the control box:\n";
QString GCodeCommands6="G10 L[LOGIC BATTERY LVL_%] M[MOTOR BATTERY VOLATAGE %] - set voltage values on the displays (in %) exp: G10 L50 M70 - logic battery is 50% charged, motor battery is 70%\n";
QString GCodeCommands7="G11 - set the bt communication diode on/off\n";
QString GCodeCommands8="G12 - blink the bt com diode\n";
QString GCodeCommands9="G13 - set the error diode on/off\n";
QString GCodeCommands10="G14 - blink the error diode\n";

QString GCodeCommands = GCodeCommands0 +
        GCodeCommands1 +
        GCodeCommands2 +
        GCodeCommands3 +
        GCodeCommands4 +
        GCodeCommands5 +
        GCodeCommands6 +
        GCodeCommands7 +
        GCodeCommands8 +
        GCodeCommands9 +
        GCodeCommands10;

int vMax = 30;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/icon.png"));

    this->discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    this->socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    //connect(this->discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this, SLOT(captureDeviceProperties(QBluetoothDeviceInfo)));

//    mCartThr = new cart(this);
//    mCartThr->start();
//    moveToThread(mCartThr);

    connect(this->socket, SIGNAL(connected()),this, SLOT(connectionEstablished()));
    connect(this->socket, SIGNAL(disconnected()),this, SLOT(connectionInterrupted()));
    connect(this->socket, SIGNAL(readyRead()),this, SLOT(socketReadyToRead()));
//    connect(this,&MainWindow::start,mCartThr,&cart::receivePrtData);
//    connect(this,&MainWindow::stop,mCartThr,&cart::timeToGoBack);
//    connect(this,&MainWindow::goSleep,mCartThr,&cart::mSleep);
//    connect(this,&MainWindow::wakeUp,mCartThr,&cart::mWakeUp);

//    connect(mCartThr,&cart::newMsg,this,&MainWindow::newMsgGetted);
//    connect(this,&MainWindow::sendMsg,mCartThr,&cart::getAndSendMsg);

//    emit start(&cartData);

    //orange: #ff732d, darkgrey: #252525
    ui->pushButtonCloseConnection->setStyleSheet("background-color: red");
    ui->pushButtonCloseConnection->setText("Disconnected");
    ui->pushButtonConnect->setStyleSheet("background-color: #252525");
    ui->pushButtonConnect->setText("Connect");

    mTimer = new QTimer;
    connect(mTimer, &QTimer::timeout, this, &MainWindow::dupaSlot);
    mTimer->start(100);


    this->ui->horizontalSlider_speed->setSliderPosition(vMax);
    this->ui->lcdNumber_speed->display(vMax);

}

MainWindow::~MainWindow()
{
    emit stop();
    mCartThr->quit();
    mCartThr->wait();

    delete mTimer;
    delete mCartThr;
    delete ui;
}

void MainWindow::addToLogs(QString a){
    this->ui->logsText->append(a);
}

void MainWindow::connectionEstablished() {
    this->addToLogs("Connection setted.");
    //orange: #ff732d, darkgrey: #252525
    ui->pushButtonCloseConnection->setStyleSheet("background-color: #252525");
    ui->pushButtonCloseConnection->setText("Disconnect");
    ui->pushButtonConnect->setStyleSheet("background-color: green");
    ui->pushButtonConnect->setText("Connected");

//    emit wakeUp();
}

void MainWindow::connectionInterrupted() {
    this->addToLogs("Connection interrupted");
    //orange: #ff732d, darkgrey: #252525
    ui->pushButtonCloseConnection->setStyleSheet("background-color: red");
    ui->pushButtonCloseConnection->setText("Disconnected");
    ui->pushButtonConnect->setStyleSheet("background-color: #252525");
    ui->pushButtonConnect->setText("Connect");

//    emit goSleep();
}

void MainWindow::newMsgGetted() {
    while(this->cartData.isAnyMgs()) {
         while(!cartData.lockData()){};
         this->addToLogs(this->cartData.getOldestMsg());
        cartData.unlockData();
    }
}


void MainWindow::keyPressEvent (QKeyEvent * event){

    if(event->key() == Qt::Key_W){
        //do przeodu
        cartData.setPressedStraightKey(true);
        if(cartData.getSpeed()<vMax){
           cartData.setSpeed(cartData.getSpeed()+5);
        }
    }
    if(event->key() == Qt::Key_D){
        cartData.setPressedTurnKey(true);
        if(cartData.getAngle() < MAX_ANGLE){
            cartData.setAngle(cartData.getAngle()+ 0.5);
        }
    }
    if(event->key() == Qt::Key_A){
        //        addToLogs("skrec w lewo");
        cartData.setPressedTurnKey(true);
        if(cartData.getAngle() > -MAX_ANGLE){
            cartData.setAngle(cartData.getAngle()- 0.5);
        }
    }
    if(event->key() == Qt::Key_S){
        //        addToLogs("jedz do tylu");
        cartData.setPressedStraightKey(true);
       if(cartData.getSpeed()>-vMax){
          cartData.setSpeed(cartData.getSpeed()-5);
       }
    }
    if(event->key() == Qt::Key_Q){
        //        addToLogs("jedz do tylu");
       cartData.setSpeed(0);
    }
}

void MainWindow::keyReleaseEvent (QKeyEvent * event){
    if(event->key() == Qt::Key_W || event->key() == Qt::Key_S){
        cartData.setPressedStraightKey(false);
    }
    if(event->key() == Qt::Key_A || event->key() == Qt::Key_D){
        cartData.setPressedTurnKey(false);
    }
}

void MainWindow::on_pushButtonConnect_clicked() {
    addToLogs("Connecting...");
//    emit wakeUp();
//    startSendingMsg();
//    connectionEstablished();
    QString deviceAddres = "00:19:12:10:01:A8";
    //HC-06 i adresie: 00:19:12:10:01:A8

    static const QString serviceUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB"));
//     while(!cartData.lockData()){};
    this->socket->connectToService(QBluetoothAddress(deviceAddres),QBluetoothUuid(serviceUuid),QIODevice::ReadWrite);
//    cartData.unlockData();
    this->addToLogs("Conncetion with: HC-06 , adress: " + deviceAddres + " started");

}

void MainWindow::on_pushButtonCloseConnection_clicked() {
    this->addToLogs( "Connection close");
//    emit goSleep();
//    stopSendingMsg();
//    connectionInterrupted();
//     while(!cartData.lockData()){};
    this->socket->disconnectFromService();
//    cartData.unlockData();
}

void MainWindow::on_pushButtonSendMessage_clicked(){

     while(!cartData.lockData()){};
    if(socket->state() != QBluetoothSocket::ConnectedState){
        addToLogs("Connection to the cart disabled!");
        return;
    }
     qDebug()<<"Napisz wysyłanie customowych wiadomości ziom";
    cartData.unlockData();

    QString message = "Empty message\n";
    message = this->ui->textEditCommand->toPlainText();
    message = message+ " "+ QString::number(crc16(message))+"\n";
    this->addToLogs("Sending message: " + message);

    while(!cartData.lockData()){};
    this->socket->write(message.toStdString().c_str());
    cartData.unlockData();
}

void MainWindow::on_pushButtonClear_clicked()
{
    this->ui->logsText->clear();
}


void MainWindow::on_horizontalSlider_speed_valueChanged(int value)
{
//    while(!cartData.lockData()){};
//    cartData.setSpeed(value);
    vMax = value;
//    cartData.unlockData();
    ui->lcdNumber_speed->display(value);
}


void MainWindow::on_horizontalSlider_angle_valueChanged(int value)
{
     while(!cartData.lockData()){};
   cartData.setAngle(value/5.0);
    cartData.unlockData();
    ui->lcdNumber_angle->display(cartData.getAngle());
}


void MainWindow::on_pushButtonConnect_Batteries_clicked()
{
    if(ui->pushButtonConnect_Batteries->text() == "Volts"){
        ui->pushButtonConnect_Batteries->setText("Percents");
    }
    else{
        ui->pushButtonConnect_Batteries->setText("Volts");
    }

}


void MainWindow::on_pushButton_PrintCommands_clicked()
{
    this->addToLogs(GCodeCommands);
}

