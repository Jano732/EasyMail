#ifndef IMAPCLIENT_H
#define IMAPCLIENT_H

#include "email.h"
#include <QSslSocket>

class ImapClient : public QObject
{
    QSslSocket *_socket;
    QString _tag;
    int _tagNum = 0;
    const QString _url;
    const int _port;
    const QString _login;
    const QString _password;
    std::vector<Email*> emails;
    int _uidnext;

public:

    explicit ImapClient(const QString, const int, const QString, const QString, QObject *parent = 0);
    bool connect();
    void choseInbox();
    QString nextTag();
    QString sendRequest(QString);
    void uidnextFinder(QString);
    QString encode(QString);
    std::vector<Email> basicDataParser(QString);
    QStringList subjectParser(QString);


    // ===== ACCESSORS =====

    QString getTag();
    QString getUrl();
    int getPort();
    QString getLogin();
    QString getPassword();

};

#endif // IMAPCLIENT_H
