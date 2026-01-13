#include "imapclient.h"
#include "QDebug"
#include <winsock2.h>


ImapClient::ImapClient(const QString url, const int port, const QString login, const QString password, QObject *parent)
    : QObject(parent)
    , _url(url)
    , _port(port)
    , _login(login)
    , _password(password)
{
    bool res = connect();
}

bool ImapClient::connect()
{

    _socket = new QSslSocket(this);
    _socket->connectToHostEncrypted(_url, _port);

    if(_socket->waitForEncrypted(3000)){

        qDebug() << "Server connected!";
        QString login_request = nextTag() + " LOGIN " + _login + " " + _password + "\r\n";
        QString select_request = nextTag() + " SELECT INBOX\r\n";
        QString recieved_data;

        _socket->waitForReadyRead(3000);
        qDebug() << _socket->readAll();

        sendRequest(login_request);
        recieved_data = sendRequest(select_request);
        uidnextFinder(recieved_data);

        QString envelope_init = nextTag() + " UID FETCH " + QString::number(_uidnext - 31) + ":" + QString::number(_uidnext - 1) + " (BODY[HEADER.FIELDS (DATE SUBJECT FROM)])\r\n";
        qDebug() << envelope_init;
        recieved_data = sendRequest(envelope_init);
        qDebug() << recieved_data;

        parser(recieved_data);

        return true;
    }
    else{
        qDebug() << "Not connected!";
        return false;
    }
}

QString ImapClient::nextTag()
{
    _tagNum++;
    _tag = "A00" + QString::number(_tagNum);
    return _tag;
}

QString ImapClient::sendRequest(QString request)
{
    QByteArray converted_req = request.toUtf8();
    _socket->write(converted_req);
    _socket->waitForBytesWritten(3000);
    _socket->waitForReadyRead(3000);
    return _socket->readAll();
}

void ImapClient::uidnextFinder(QString recieved_data)
{
    if(recieved_data.contains("[UIDNEXT"))
    {
        int start_index = recieved_data.indexOf("[UIDNEXT") + 9;
        int end_index = recieved_data.indexOf("]", start_index);
        QString uid = recieved_data.mid(start_index, end_index - start_index);
        _uidnext = uid.toInt();
    }
}

Email ImapClient::parser(QString response)
{

}

// =================== ACCESSORS ===================


QString ImapClient::getUrl() { return _url; }

QString ImapClient::getTag() { return _tag; }

int ImapClient::getPort() {return _port; }

QString ImapClient::getLogin() {return _login; }

QString ImapClient::getPassword() {return _password; }


