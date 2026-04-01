#include "imapclient.h"
#include "QDebug"
#include <qregularexpression.h>
#include <qurl.h>

#include <QtConcurrent>


ImapClient::ImapClient(const QString url, const int port, const QString log, const QString password, QObject *parent)
    : QObject(parent)
    , _url(url)
    , _port(port)
    , _login(log)
    , _password(password)
{
    connect();
    login();
    selectInbox("INBOX");
    qDebug("FIN");
}


bool ImapClient::connect()
{
    _socket = new QSslSocket(this);
    _socket->connectToHostEncrypted(_url, _port);

    if(_socket->waitForEncrypted(3000)){

        qDebug() << "Server connected!";
        qDebug() << _socket->readAll() << "\n===========================\n";
        return true;
    }
    else return false;
}

void ImapClient::login()
{
    QString login_request = nextTag() + " LOGIN " + _login + " " + _password + "\r\n";
    QString response = sendRequest(login_request);
    // qDebug() << response;
}

void ImapClient::selectInbox(QString inbox)
{
    QString select_request = nextTag() + " SELECT " + inbox.toUpper() + "\r\n";
    QString response = sendRequest(select_request);
    uidnextFinder(response);
    // qDebug() << response;
}

void ImapClient::uidnextFinder(QString recieved_data)
{
    if(recieved_data.contains("[UIDNEXT"))
    {
        int start_index = recieved_data.indexOf("[UIDNEXT") + 9;
        int end_index = recieved_data.indexOf("]", start_index);
        QString uid = recieved_data.mid(start_index, end_index - start_index);
        _uidnext = uid.toInt();
        qDebug() << "Next uid:" << _uidnext;
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
    QString response;
    _socket->write(converted_req);
    _socket->waitForBytesWritten(3000);
    while(_socket->waitForReadyRead(3000))
    {
        response += _socket->readAll();
    }
    return response;
}



// =================== ACCESSORS ===================


QString ImapClient::getUrl() { return _url; }

QString ImapClient::getTag() { return _tag; }

int ImapClient::getPort() {return _port; }

QString ImapClient::getLogin() {return _login; }

int ImapClient::getUid() { return _uidnext; }

void ImapClient::setUid(int uid) { _uidnext = uid; }

// std::vector<Email> ImapClient::getEmails() {return _emails; }

