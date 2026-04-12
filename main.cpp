#pragma once
#include "Service/service.h"
#include "Repository/repositoryemail.h"
#include "View/emailmodel.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include <qdiriterator.h>
#include <QtWebEngineQuick/QtWebEngineQuick>
#include <vmime/vmime.hpp>

int main(int argc, char *argv[])
{
    QtWebEngineQuick::initialize();
    QApplication a(argc, argv);

    auto *client = new ImapClient("imap.poczta.onet.pl", 993, "poniatowski@op.pl", "XRAB-ELTQ-ECU1-T0M5");
    // auto *client = new ImapClient("imap.gmail.com", 993, "jan.poniatowski2003@gmail.com", "tzirxihayzdtxozy");
    auto *repository = new RepositoryEmail(client);
    auto *emailModel = new EmailModel();
    auto *service = new Service(repository, emailModel);

    QObject::connect(repository, &RepositoryEmail::emailsEnvelopedReady, service, &Service::onEmailsEnvelope);
    QObject::connect(service, &Service::requestEnvelopedEmails,repository, &RepositoryEmail::envelopeEmailsSlot);
    QObject::connect(service, &Service::requestBody, repository, &RepositoryEmail::fetchBody);
    QObject::connect(repository, &RepositoryEmail::htmlReady, service, &Service::onHtmlReady);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("emailModel", emailModel);
    engine.rootContext()->setContextProperty("service", service);
    engine.load(QUrl(QStringLiteral("qrc:/EmailClient/main.qml")));

    service->envelopeEmails();

    if (engine.rootObjects().isEmpty())
        return -1;

    return a.exec();
}
