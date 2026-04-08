#pragma once
#include "Infrastructure/tracer.h"
#include "vmime/net/folder.hpp"
#include "vmime/net/session.hpp"
#include "vmime/security/cert/X509Certificate.hpp"
#ifndef IMAPCLIENT_H
#define IMAPCLIENT_H
#include <QSslSocket>

class ImapClient : public QObject
{
    const QString _url;
    const int _port;
    const QString _login;
    const QString _password;
    vmime::shared_ptr<vmime::net::session> _session;
    vmime::shared_ptr<vmime::net::store> _store;
    vmime::shared_ptr<vmime::net::folder> _folder;

    vmime::shared_ptr<vmime::security::cert::X509Certificate> loadX509CertificateFromFile(const std::string&);

public:

    explicit ImapClient(const QString, const int, const QString, const QString, QObject *parent = 0);

    void connect();
    void verify();
    void selectDefaultFolder();

    // ===== ACCESSORS =====

    QString getUrl();
    QString getLogin();
    int getUid();
    void setUid(int);

    vmime::shared_ptr<vmime::net::folder> getFolder();

};

#endif // IMAPCLIENT_H
