#include "imapclient.h"
#include "QDebug"
#include <qregularexpression.h>
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

        // QString envelope_init = nextTag() + " UID FETCH " + QString::number(_uidnext - 1) + ":" + QString::number(_uidnext - 31) + " (ENVELOPE)\r\n";
        QString envelope_init = nextTag() + " UID FETCH " + QString::number(_uidnext - 1) + ":" + QString::number(_uidnext - 5) + " (ENVELOPE)\r\n";
        // QString envelope_init = nextTag() + " UID FETCH " + "44967" + " (ENVELOPE)\r\n";
        recieved_data = sendRequest(envelope_init);
        qDebug() << recieved_data << "\n===========================\n";

        initDataParser(recieved_data);

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

QString ImapClient::extractNextToken(QString &data, int &pos) {

    while (pos < data.length() && data[pos].isSpace()) pos++;

    if (pos >= data.length()) return "";

    if (pos < data.length() && (data[pos] == '(' && data[pos + 1] == '(' && data[pos + 2] == '\"')) pos += 2;

    if (pos < data.length() && (data[pos] == ')' && data[pos + 1] == ')' && data[pos + 2] == ' ')) pos += 2;

    if (data[pos] == '{') {
        int end = data.indexOf('}', pos);
        int size = data.mid(pos + 1, end - pos - 1).toInt();
        pos = data.indexOf('\n', end) + 1; // Przeskocz do danych za \n
        QString res = data.mid(pos, size);
        pos += size;
        return res;
    }

    if (data[pos] == '"') {
        int end = pos;
        do {
            end = data.indexOf('"', end + 1);
        } while (end > 0 && data[end-1] == '\\'); // Obsługa escaped quotes \"

        QString res = data.mid(pos + 1, end - pos - 1);
        pos = end + 1;
        return res;
    }

    int nextSpace = data.indexOf(' ', pos);
    int nextBracket = data.indexOf(')', pos);
    int end = (nextSpace < nextBracket && nextSpace != -1) ? nextSpace : nextBracket;

    QString res = data.mid(pos, end - pos);
    pos = end;
    return (res == "NIL") ? "" : res;
}


void ImapClient::processResponse(QString input) {
    int pos = 0;

    // 1. Pomijamy początek (np. "* 13453 FETCH")
    extractNextToken(input, pos); // Zwróci "*"
    extractNextToken(input, pos); // Zwróci "13453"
    extractNextToken(input, pos); // Zwróci "FETCH"

    // 2. Szukamy początku sekcji ENVELOPE
    if (input.mid(pos).contains("ENVELOPE", Qt::CaseInsensitive)) {
        pos = input.indexOf("ENVELOPE", pos) + 8; // Przeskakujemy słowo ENVELOPE

        // Teraz jesteśmy tuż przed nawiasem otwierającym sekcję ENVELOPE
        while (pos < input.length() && input[pos] != '(') pos++;
        pos++; // Wejdź do środka nawiasu

        QString date    = extractNextToken(input, pos);
        QString subject = extractNextToken(input, pos);

        QString from = extractNextToken(input, pos);
        from += " <";
        from += extractNextToken(input, pos); //NIL
        from += extractNextToken(input, pos);
        from += "@";
        from += extractNextToken(input, pos);
        from += ">";

        pos += 2; // Wejście z listy from do listy sender

        QString sender = extractNextToken(input, pos);
        sender += " <";
        sender += extractNextToken(input, pos);
        sender += extractNextToken(input, pos);
        sender += "@";
        sender += extractNextToken(input, pos);
        sender += ">";

        pos += 2;

        QString reply_to = extractNextToken(input, pos);
        reply_to += " <";
        reply_to += extractNextToken(input, pos);
        reply_to += extractNextToken(input, pos);
        reply_to += "@";
        reply_to += extractNextToken(input, pos);
        reply_to += ">";

        pos += 2;
        QString to = extractNextToken(input, pos);
        to += extractNextToken(input, pos);
        to += " <";
        to += extractNextToken(input, pos);
        to += "@";
        to += extractNextToken(input, pos);
        to += ">";
        // to.remove("NIL");


        qDebug() << "Data:" << decode(date);
        qDebug() << "Temat:" << decode(subject); // Twoja funkcja dekodująca UTF-8
        qDebug() << "Od:" << decode(from);
        qDebug() << "Sender:" << decode(sender);
        qDebug() << "Reply To:" << decode(reply_to);
        qDebug() << "To:" << decode(to);
        qDebug() << "=======================";
    }
}


QString ImapClient::decode(QString input)
{
    // Czyścimy białe znaki na końcach i dzielimy po spacjach/nowych liniach
    QStringList phrases = input.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    QString output;
    bool lastWasEncoded = false;

    for(int i = 0; i < phrases.size(); i++)
    {
        QString current = phrases.at(i);
        QStringList parts = current.split("?");

        // Sprawdzamy czy to format =?charset?encoding?data?=
        if(current.startsWith("=?") && current.endsWith("?=") && parts.size() >= 4)
        {
            QString encoding = parts.at(2).toUpper();
            QString subject = parts.at(3);
            QByteArray decodedBytes;

            if(encoding == "B") // Base64
            {
                decodedBytes = QByteArray::fromBase64(subject.toUtf8());
            }
            else if(encoding == "Q") // Quoted-Printable
            {
                subject.replace("=", "%");
                subject.replace("_", " ");
                decodedBytes = QByteArray::fromPercentEncoding(subject.toUtf8());
            }

            output += QString::fromUtf8(decodedBytes);
            lastWasEncoded = true;
            // Nie dodajemy spacji! RFC mówi, żeby ignorować białe znaki między zakodowanymi członami.
        }
        else
        {
            // Jeśli poprzedni fragment NIE był zakodowany, a ten jest zwykłym tekstem,
            // lub na odwrót - dodajemy spację (o ile output nie jest pusty).
            if (!output.isEmpty() && !lastWasEncoded) {
                output += " ";
            } else if (!output.isEmpty() && lastWasEncoded) {
                // Jeśli przechodzimy z zakodowanego na zwykły tekst, spacja powinna zostać
                output += " ";
            }

            output += current;
            lastWasEncoded = false;
        }
    }
    return output.trimmed();
}


std::vector<Email> ImapClient::initDataParser(QString input)
{
    QStringList parts = input.split("*");

    for(int i = 0; i < parts.size(); i++)
    {
        processResponse(parts.at(i));
    }

}

// =================== ACCESSORS ===================


QString ImapClient::getUrl() { return _url; }

QString ImapClient::getTag() { return _tag; }

int ImapClient::getPort() {return _port; }

QString ImapClient::getLogin() {return _login; }

QString ImapClient::getPassword() {return _password; }


