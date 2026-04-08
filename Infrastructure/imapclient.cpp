#pragma once
#include "imapclient.h"
#include "Infrastructure/tracerfactory.h"
#include "QDebug"
#include "vmime/net/folder.hpp"
#include "vmime/net/service.hpp"
#include "vmime/net/store.hpp"
#include "vmime/security/cert/X509Certificate.hpp"
#include "vmime/security/cert/defaultCertificateVerifier.hpp"
#include "vmime/utility/inputStreamAdapter.hpp"
#include <qregularexpression.h>
#include <qurl.h>
#include <fstream>
#include <QtConcurrent>


ImapClient::ImapClient(const QString url, const int port, const QString log, const QString password, QObject *parent)
    : QObject(parent)
    , _url(url)
    , _port(port)
    , _login(log)
    , _password(password)
{
    connect();
    // selectInbox("INBOX");
    // qDebug("FIN");
}


void ImapClient::connect()
{
    try {

        QString connection_string = "imaps://" + _login + ":" + _password + "@" + _url;
        vmime::utility::url url(connection_string.toStdString());

        _session = vmime::net::session::create();
        _store = _session->getStore(url);
        // _store->setTracerFactory(vmime::make_shared <tracerFactory>());
        verify();
        _store->connect();

        if(_store->isConnected())
        {
            qDebug() << "Connected!";
            selectDefaultFolder();
        }
        else qDebug() << "Not connected!";
    }
    catch(vmime::exception& e)
    {
        qDebug() << "Vmime exception - ImapClient::connect(): " << e.what();
    }

    catch (...)
    {
        qDebug() << "Unknown Exception - ImapClient::connect()";
    }

}

void ImapClient::verify()
{
    QString cert_path = "/Users/janponiatowski/Desktop/Projekty/INZYNIERKA/EasyMail/Resources/cacert.pem";

    std::ifstream certFile(cert_path.toStdString(), std::ios::in | std::ios::binary);
    if(!certFile)
    {
        qDebug() << "Error: cannot open cert file!";
        return;
    }

    std::string fileContent((std::istreambuf_iterator<char>(certFile)),
                            std::istreambuf_iterator<char>());

    std::vector<vmime::shared_ptr<vmime::security::cert::X509Certificate>> rootCAs;

    const std::string begin_marker = "-----BEGIN CERTIFICATE-----";
    const std::string end_marker = "-----END CERTIFICATE-----";

    size_t pos = 0;
    while(pos < fileContent.size())
    {
        size_t begin_pos = fileContent.find(begin_marker, pos);
        if(begin_pos == std::string::npos) break;

        size_t end_pos = fileContent.find(end_marker, begin_pos);
        if(end_pos == std::string::npos) break;

        end_pos += end_marker.size();
        std::string certBlock = fileContent.substr(begin_pos, end_pos - begin_pos);

        vmime::shared_ptr<vmime::security::cert::X509Certificate> cert =
            vmime::security::cert::X509Certificate::import(
                reinterpret_cast<const vmime::byte_t*>(certBlock.data()),
                certBlock.size()
                );

        if(cert) rootCAs.push_back(cert);
        pos = end_pos;
    }

    qDebug() << "Loaded" << rootCAs.size() << "root CA certificates";

    vmime::shared_ptr<vmime::security::cert::defaultCertificateVerifier> vrf =
        vmime::make_shared<vmime::security::cert::defaultCertificateVerifier>();

    vrf->setX509RootCAs(rootCAs);
    _store->setCertificateVerifier(vrf);
}

void ImapClient::selectDefaultFolder()
{
    _folder = _store->getDefaultFolder();
    _folder->open(vmime::net::folder::MODE_READ_WRITE);
}

vmime::shared_ptr<vmime::security::cert::X509Certificate> ImapClient::loadX509CertificateFromFile(const std::string& path)
{
    std::ifstream certFile;
    certFile.open(path.c_str(), std::ios::in | std::ios::binary);

    if(!certFile)
    {
        qDebug() << "Error with opening cert file!";
        return nullptr;
    }

    vmime::utility::inputStreamAdapter is(certFile);
    vmime::shared_ptr<vmime::security::cert::X509Certificate> cert;
    cert = vmime::security::cert::X509Certificate::import(is);

    return cert;
}


// =================== ACCESSORS ===================


QString ImapClient::getUrl() { return _url; }


QString ImapClient::getLogin() {return _login; }

vmime::shared_ptr<vmime::net::folder> ImapClient::getFolder() { return _folder; }


// std::vector<Email> ImapClient::getEmails() {return _emails; }

