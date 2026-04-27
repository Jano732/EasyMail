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

    auto *client = new ImapClient("imap.poczta.onet.pl", 993, "poniatowski@op.pl", "password");

    auto *repository = new RepositoryEmail(client);
    auto *emailModel = new EmailModel();
    auto *attachmentModel = new AttachmentModel();
    auto *mailboxModel = new MailboxModel();
    auto *service = new Service(repository, emailModel, attachmentModel, mailboxModel);

    QObject::connect(repository, &RepositoryEmail::emailsEnvelopedReady, service, &Service::onEmailsEnvelope);
    QObject::connect(service, &Service::requestEnvelopedEmails,repository, &RepositoryEmail::envelopeEmailsSlot);
    QObject::connect(service, &Service::requestBody, repository, &RepositoryEmail::fetchBody);
    QObject::connect(repository, &RepositoryEmail::htmlReady, service, &Service::onHtmlReady);
    QObject::connect(repository, &RepositoryEmail::attachmentsReady,service, &Service::onAttachmentsReady);
    QObject::connect(repository, &RepositoryEmail::mailboxesLoaded, service, &Service::onFetchedMailboxes);
    QObject::connect(service, &Service::changeMailbox_signal, client, &ImapClient::changeMailbox);
    QObject::connect(client, &ImapClient::mailbox_changed, repository, &RepositoryEmail::envelopeEmailsSlot);


    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("emailModel", emailModel);
    engine.rootContext()->setContextProperty("service", service);
    engine.rootContext()->setContextProperty("mailboxModel", mailboxModel);
    engine.rootContext()->setContextProperty("attachmentModel", attachmentModel);
    engine.load(QUrl(QStringLiteral("qrc:/EmailClient/main.qml")));

    service->envelopeEmails();
    repository->loadMailboxesSlot();

    if (engine.rootObjects().isEmpty())
        return -1;

    return a.exec();
}
