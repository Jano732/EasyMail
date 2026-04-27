#pragma once
#include "vmime/net/folder.hpp"
#include "vmime/net/session.hpp"
#include "vmime/security/cert/X509Certificate.hpp"
#ifndef IMAPCLIENT_H
#define IMAPCLIENT_H
#include <QSslSocket>

struct MailBox{

    QString name;
    int elements;
};

class ImapClient : public QObject
{

    Q_OBJECT

    const QString _url;
    const int _port;
    const QString _login;
    const QString _password;
    vmime::shared_ptr<vmime::net::session> _session;
    vmime::shared_ptr<vmime::net::store> _store;
    vmime::shared_ptr<vmime::net::folder> _folder;
    QList<MailBox> _mailboxes;

    vmime::shared_ptr<vmime::security::cert::X509Certificate> loadX509CertificateFromFile(const std::string&);

public:

    explicit ImapClient(const QString, const int, const QString, const QString, QObject *parent = 0);

    void connect();
    void verify();
    void selectDefaultFolder();
    void markAsRead(const QString&);
    std::vector<vmime::shared_ptr<vmime::net::message>> getMessageByUid(QString);
    void fetchMailBoxes();


    // ===== ACCESSORS =====

    QString getUrl();
    QString getLogin();
    int getUid();
    void setUid(int);
    QList<MailBox> getMailboxes();

    vmime::shared_ptr<vmime::net::folder> getFolder();

signals:

    void fetched_mailboxes_signal(QList<MailBox>);
    void mailbox_changed();

public slots:

    void changeMailbox(QString);

};

#endif // IMAPCLIENT_H
