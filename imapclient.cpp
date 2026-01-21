#include "imapclient.h"
#include "QDebug"
#include <qurl.h>
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
        qDebug() << _socket->readAll() << "\n===========================\n";

        sendRequest(login_request);
        recieved_data = sendRequest(select_request);
        qDebug() << recieved_data << "\n===========================\n";
        uidnextFinder(recieved_data);
        qDebug() << "uidnext = " << _uidnext << "\n===========================\n";

        QString envelope_init = nextTag() + " UID FETCH " + QString::number(_uidnext - 1) + ":" + QString::number(_uidnext - 31) + " (BODY[HEADER.FIELDS (DATE SUBJECT FROM)])\r\n";
        recieved_data = sendRequest(envelope_init);
        qDebug() << recieved_data << "\n===========================\n";

        basicDataParser(recieved_data);

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
    QString response;
    _socket->write(converted_req);
    _socket->waitForBytesWritten(3000);
    while(_socket->waitForReadyRead(3000))
    {
        response += _socket->readAll();
    }
    return response;
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


QStringList ImapClient::subjectParser(QString input)
{
    QStringList recieved_mail_data;

    if(input.contains("FETCH"))
    {
        int start_index, end_index;

        start_index = input.indexOf("UID") + 4;
        end_index = input.indexOf(" ", start_index);
        QString uid = input.mid(start_index, end_index - start_index);
        qDebug() << uid;
        recieved_mail_data.append(uid);

        start_index = input.indexOf("Date: ") + 6;
        end_index = input.indexOf("\r\n", start_index);
        QString date = input.mid(start_index, end_index - start_index);
        qDebug() << date;
        recieved_mail_data.append(date);

        start_index = input.indexOf("From: ") + 6;
        end_index = input.indexOf("<", start_index);
        QString name = input.mid(start_index, end_index - start_index);
        name = encode(name);
        qDebug() << name;
        recieved_mail_data.append(name);

        start_index = input.indexOf("<") + 1;
        end_index = input.indexOf(">", start_index);
        QString address = input.mid(start_index, end_index - start_index);
        qDebug() << address;
        recieved_mail_data.append(address);

        start_index = input.indexOf("Subject: ") + 9;
        end_index = input.indexOf("*", start_index);
        QString subject = input.mid(start_index, end_index - start_index);
        subject = encode(subject);
        qDebug() << subject;
        recieved_mail_data.append(subject);
        recieved_mail_data.append(QString::number(end_index));

    }
    return recieved_mail_data;
}


QString ImapClient::encode(QString input)
{
    QStringList parts = input.split("?");

    if(parts.size() >= 4)
    {
        // qDebug() << parts.at(0) << ", " << parts.at(1) << ", " << parts.at(2) << ", " << parts.at(3);
        QString encoding = parts.at(2);
        QString subject = parts.at(3);
        // qDebug() << "separated subject: " << subject;

        if(encoding == 'B') //Base64
        {
            QString output = QByteArray::fromBase64(subject.toUtf8());
            return output;
        }
        else if(encoding == 'Q') //Quoted Printed
        {
            subject.replace("=", "%");
            subject.replace("_", " ");
            QString output = QByteArray::fromPercentEncoding(subject.toUtf8());
            return output;
        }
    }
    return input;
}

std::vector<Email> ImapClient::basicDataParser(QString input)
{
    QStringList part = input.split("*");
    QStringList res;
    std::vector<Email> emails;
    qDebug() << part.size() << ", " << part.at(2);
    res = subjectParser(part.at(2));
    // qDebug() << "==========";
    // qDebug() << res.at(0);
    // qDebug() << res.at(1);
    // qDebug() << res.at(2);
    // qDebug() << res.at(3);
    // qDebug() << res.at(4);

    // for(int i = 1; i < 30; i++)
    // {
    //     res = subjectParser(part.at(i));
    //     qDebug() << i;
    //     Email email(res.at(0), res.at(1), res.at(2), res.at(3), res.at(4));
    //     emails.push_back(email);
    // }
    // qDebug() << emails.size();
    return emails;
}



// =================== ACCESSORS ===================


QString ImapClient::getUrl() { return _url; }

QString ImapClient::getTag() { return _tag; }

int ImapClient::getPort() {return _port; }

QString ImapClient::getLogin() {return _login; }

QString ImapClient::getPassword() {return _password; }


