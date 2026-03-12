#ifndef IMAPCLIENT_H
#define IMAPCLIENT_H

#include "../Model/email.h"
#include <QSslSocket>

class ImapClient : public QObject
{
    QSslSocket *_socket;

    int _tagNum = 0;
    int _uidnext = 0;
    QString _tag;
    const QString _url;
    const int _port;
    const QString _login;
    const QString _password;

public:

    explicit ImapClient(const QString, const int, const QString, const QString, QObject *parent = 0);

    bool connect();
    void login();
    void selectInbox(QString);
    QString nextTag();
    QString sendRequest(QString);
    void uidnextFinder(QString);

    // ===== ACCESSORS =====

    QString getTag();
    QString getUrl();
    int getPort();
    QString getLogin();
    int getUid();
    void setUid(int);

};

#endif // IMAPCLIENT_H
