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

class TrustAllVerifier : public vmime::security::cert::certificateVerifier
{
public:
    void verify(const vmime::shared_ptr<vmime::security::cert::certificateChain>& chain,
                const vmime::string& hostname) override {}
};

void ImapClient::connect()
{
    try {

        QString connection_string = "imaps://" + _login + ":" + _password + "@" + _url;
        vmime::utility::url url(connection_string.toStdString());

        qDebug() << "Url: " << connection_string;

        _session = vmime::net::session::create();
        _store = _session->getStore(url);
        // _store->setTracerFactory(vmime::make_shared <tracerFactory>());
        // verify();

        _store->setCertificateVerifier(vmime::make_shared<TrustAllVerifier>());

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
    // QString cert_path = "/Users/janponiatowski/Desktop/Projekty/INZYNIERKA/EasyMail/Resources/cacert.pem";
    // QString cert_path = "D:\\Projekty\\INZYNIERKA\EasyMail\\Resources\\cacert.pem";
    QString cert_path = QCoreApplication::applicationDirPath() + "/Resources/cacert.pem";
    qDebug() << "Szukam cert w:" << QCoreApplication::applicationDirPath();

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
    vrf->setX509TrustedCerts(rootCAs);
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


void ImapClient::markAsRead(const QString& uid)
{

    auto messages = getMessageByUid(uid);

    if(messages.empty())
        return;

    messages.front()->setFlags(vmime::net::message::FLAG_SEEN, vmime::net::message::FLAG_MODE_ADD);
}


std::vector<vmime::shared_ptr<vmime::net::message>> ImapClient::getMessageByUid(QString uid)
{
    vmime::string casted_uid = uid.toStdString();
    std::vector<vmime::shared_ptr<vmime::net::message>> messages = _folder->getMessages(vmime::net::messageSet::byUID(casted_uid));
    return messages;
}


QList<QString> ImapClient::fetchMailBoxes()
{
    QList<QString> mail_boxes;

    try {
        vmime::shared_ptr<vmime::net::folder> root = _store->getRootFolder();
        auto folders = root->getFolders(true);
        for(auto& f : folders)
        {
            QString name = QString::fromStdString(f->getName().getBuffer());
            mail_boxes.append(name);
            qDebug() << name;
        }
    }
    catch (vmime::exception& e)
    {
        qDebug() << e.what();
    }

    return mail_boxes;
}


// =================== ACCESSORS ===================


QString ImapClient::getUrl() { return _url; }


QString ImapClient::getLogin() {return _login; }

vmime::shared_ptr<vmime::net::folder> ImapClient::getFolder() { return _folder; }


// std::vector<Email> ImapClient::getEmails() {return _emails; }

