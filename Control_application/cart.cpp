#include "cart.h"


int crc16(QString s){

    QByteArray ba = s.toLocal8Bit();
    char *ptr = ba.data();
    int count = s.length();

    int  crc;
    char i;
    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}

dataStr *cart::getCartDataStr()
{
    return cartDataStr;
}

//bool cart::getSendingMsg() const
//{
//    return sendingMsg;
//}

//void cart::setSendingMsg(bool newSendingMsg)
//{
//    sendingMsg = newSendingMsg;
//}

QBluetoothDeviceDiscoveryAgent *cart::getDiscoveryAgent() const
{
    return discoveryAgent;
}

void cart::setDiscoveryAgent(QBluetoothDeviceDiscoveryAgent *newDiscoveryAgent)
{
    discoveryAgent = newDiscoveryAgent;
}

QBluetoothSocket *cart::getSocket() const
{
    return socket;
}

void cart::setSocket(QBluetoothSocket *newSocket)
{
    socket = newSocket;
}

cart::cart(QWidget *parent){
//   cartDataStr = nullptr;
//   sendingMsg = false;
//   isTimeToGoBack = false;

       this->discoveryAgent = nullptr;
       this->socket = nullptr;
     cartDataStr = nullptr;

   this->discoveryAgent = new QBluetoothDeviceDiscoveryAgent;
   this->socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

     QString deviceAddres = "00:19:12:10:01:A8";
     //HC-06 i adresie: 00:19:12:10:01:A8

     static const QString serviceUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB"));


 this->socket->connectToService(QBluetoothAddress(deviceAddres),QBluetoothUuid(serviceUuid),QIODevice::ReadWrite);

}

void cart::run()
{
    init();
        QString msg = "";
        while(!isTimeToGoBack){

                if(!cartDataStr->getPressedStraightKey()){
                        cartDataStr->setSpeed(0);
                }

                if(!cartDataStr->getPressedTurnKey()){
                        cartDataStr->setAngle(0);
                }

                if(cartDataStr->getSendingMsg()){
                    msg = "G1 ";
                     while(!cartDataStr->lockData()){};

                    msg+="L"+QString::number(cartDataStr->getSpeed()*10,'d',2)+" ";
                    msg +="A"+QString::number(cartDataStr->getAngle()*35,'d',2);
                    msg += " " + QString::number(crc16(msg)) +"\n";
    //                qDebug() <<"message: " << msg;
                    socket->write(msg.toStdString().c_str());
                    cartDataStr->unlockData();

                }

//            if(this->cartDataStr->getCloseConnectionPls()){
//                this->mSleep();
//                cartDataStr->setCloseConnectionPls(false);
//            }
//            if(this->cartDataStr->getOpenConnectionPls()){
//                this->mWakeUp();
//                cartDataStr->setOpenConnectionPls(false);
//            }
            msleep(SENDING_MSG_PERIOD_MS);
        }

}

void cart::init()
{

//   sendingMsg = false;
   isTimeToGoBack = false;

   this->discoveryAgent = new QBluetoothDeviceDiscoveryAgent;
   this->socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

}

//QBluetoothDeviceDiscoveryAgent *dataStr::getDiscoveryAgent() const
//{
//    return discoveryAgent;
//}

//void dataStr::setDiscoveryAgent(QBluetoothDeviceDiscoveryAgent *newDiscoveryAgent)
//{
//    discoveryAgent = newDiscoveryAgent;
//}

//QBluetoothSocket *dataStr::getSocket() const
//{
//    return socket;
//}

//void dataStr::setSocket(QBluetoothSocket *newSocket)
//{
//    socket = newSocket;
//}

bool dataStr::getPressedTurnKey() const
{
    return pressedTurnKey;
}

void dataStr::setPressedTurnKey(bool newPressedTurnKey)
{
    pressedTurnKey = newPressedTurnKey;
}

bool dataStr::getPressedStraightKey() const
{
    return pressedStraightKey;
}

void dataStr::setPressedStraightKey(bool newPressedStraightKey)
{
    pressedStraightKey = newPressedStraightKey;
}

QQueue<QString> *dataStr::getMessages() const
{
    return messagesToRead;
}

void dataStr::setMessages(QQueue<QString> *newMessages)
{
    messagesToRead = newMessages;
}

bool dataStr::getIsNewMsg() const
{
    return isNewMsg;
}

void dataStr::setIsNewMsg(bool newIsNewMsg)
{
    isNewMsg = newIsNewMsg;
}

bool dataStr::getCloseConnectionPls() const
{
    return closeConnectionPls;
}

void dataStr::setCloseConnectionPls(bool newCloseConnectionPls)
{
    closeConnectionPls = newCloseConnectionPls;
}

bool dataStr::getOpenConnectionPls() const
{
    return openConnectionPls;
}

void dataStr::setOpenConnectionPls(bool newOpenConnectionPls)
{
    openConnectionPls = newOpenConnectionPls;
}

QQueue<QString> *dataStr::getMessageToSend() const
{
    return messageToSend;
}

void dataStr::setMessageToSend(QQueue<QString> *newMessageToSend)
{
    messageToSend = newMessageToSend;
}

bool dataStr::getSendingMsg() const
{
    return sendingMsg;
}

void dataStr::setSendingMsg(bool newSendingMsg)
{
    sendingMsg = newSendingMsg;
}

double dataStr::getAngle() const
{
    return angle;
}

void dataStr::setAngle(double newAngle)
{
    angle = newAngle;
}

double dataStr::getSpeed() const
{
    return speed;
}

void dataStr::setSpeed(double newSpeed)
{
    speed = newSpeed;
}

void cart::socketReadyToRead() {
    while(this->socket->canReadLine()) {
        QString line = this->socket->readLine();
        //qDebug() << line;

        QString terminator = "\n";
        int pos = line.lastIndexOf(terminator);
        //      qDebug() << line.left(pos);
        while(!this->cartDataStr->lockData()){}
        this->cartDataStr->addToQueue(line.left(pos));
        this->cartDataStr->unlockData();

        emit newMsg();

        //       this->addToLogs(line);
    }
}
