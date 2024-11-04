#ifndef CART_H
#define CART_H

#include <QThread>
#include <QMutex>
#include <QDebug>
#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QQueue>
#include <QMainWindow>

#define SENDING_MSG_PERIOD_MS 100

int crc16(QString s);

class dataStr{
    double angle, speed;
    bool pressedTurnKey, pressedStraightKey;
   mutable QMutex mtx;

    QQueue<QString> *messagesToRead, *messageToSend;
bool isNewMsg;
bool closeConnectionPls, openConnectionPls;
bool sendingMsg;

public:
    dataStr(){
        angle = 0;
        speed = 0;
        pressedTurnKey = false;
        pressedStraightKey = false;

        messagesToRead = new QQueue<QString>;
        messageToSend = new QQueue<QString>;
        isNewMsg = false;
        closeConnectionPls = false;
        openConnectionPls = false;
        sendingMsg = false;
    }
    ~dataStr(){
        mtx.unlock();
        delete messagesToRead;

    }
    void addToQueue(QString s){
        messagesToRead->push_back(s);
    }

    bool isAnyMgs(){
        return !messagesToRead->empty();
    }

    QString getOldestMsg(){
        if(messagesToRead->empty()){
            return "";
        }
        QString tmp = messagesToRead->head();
        messagesToRead->pop_front();
        return tmp;
    }

    bool lockData(){
       return mtx.tryLock();
    }
    void unlockData(){
        mtx.unlock();
    }
    double getAngle() const;
    void setAngle(double newAngle);
    double getSpeed() const;
    void setSpeed(double newSpeed);
//    QBluetoothDeviceDiscoveryAgent *getDiscoveryAgent() const;
//    void setDiscoveryAgent(QBluetoothDeviceDiscoveryAgent *newDiscoveryAgent);
//    QBluetoothSocket *getSocket() const;
//    void setSocket(QBluetoothSocket *newSocket);

    bool getPressedTurnKey() const;
    void setPressedTurnKey(bool newPressedTurnKey);
    bool getPressedStraightKey() const;
    void setPressedStraightKey(bool newPressedStraightKey);
    QQueue<QString> *getMessages() const;
    void setMessages(QQueue<QString> *newMessages);
    bool getIsNewMsg() const;
    void setIsNewMsg(bool newIsNewMsg);
    bool getCloseConnectionPls() const;
    void setCloseConnectionPls(bool newCloseConnectionPls);
    bool getOpenConnectionPls() const;
    void setOpenConnectionPls(bool newOpenConnectionPls);
    QQueue<QString> *getMessageToSend() const;
    void setMessageToSend(QQueue<QString> *newMessageToSend);
    bool getSendingMsg() const;
    void setSendingMsg(bool newSendingMsg);
};

class cart : public QThread
{
    Q_OBJECT

    dataStr *cartDataStr;
//    bool sendingMsg;
    bool isTimeToGoBack;

    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothSocket *socket;

public:
    cart(QWidget *parent);
    void run() override;

    void init();

     dataStr *getCartDataStr() ;
    void setCartDataStr(const dataStr &newDStr);
//    bool getSendingMsg() const;
//    void setSendingMsg(bool newSendingMsg);

    virtual ~cart(){
        delete discoveryAgent;
        delete socket;
    };
    QBluetoothDeviceDiscoveryAgent *getDiscoveryAgent() const;
    void setDiscoveryAgent(QBluetoothDeviceDiscoveryAgent *newDiscoveryAgent);
    QBluetoothSocket *getSocket() const;
    void setSocket(QBluetoothSocket *newSocket);

    void mWakeUp(){
//        sendingMsg = true;
//        QString deviceAddres = "00:19:12:10:01:A8";
//        //HC-06 i adresie: 00:19:12:10:01:A8

//        static const QString serviceUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB"));


//    this->socket->connectToService(QBluetoothAddress(deviceAddres),QBluetoothUuid(serviceUuid),QIODevice::ReadWrite);

}

signals:
    void newMsg();

public slots:
    void timeToGoBack(){
        isTimeToGoBack = true;
    }

    void mSleep(){
//        sendingMsg = false;
        while(!cartDataStr->lockData()){};
       this->socket->disconnectFromService();
       cartDataStr->unlockData();
    }


    void receivePrtData(dataStr * a){
        cartDataStr = a;
    }

    void getAndSendMsg(QString ms){
        while(!cartDataStr->lockData()){};
 qDebug()<<ms;
       cartDataStr->unlockData();
    }

    void socketReadyToRead();
};

#endif // CART_H
