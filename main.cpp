#include "Service/service.h"
#include "Repository/repositoryemail.h"
#include "View/emailmodel.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include <qdiriterator.h>
#include <QtWebEngineQuick/QtWebEngineQuick>

int main(int argc, char *argv[])
{
    QtWebEngineQuick::initialize();

    QApplication a(argc, argv);
    qRegisterMetaType<std::vector<Email>>("std::vector<Email>");

    auto *client = new ImapClient("imap address", 993, "email address", "password");

    auto *repository = new RepositoryEmail(client);

    QThread *repoThread = new QThread();
    client->moveToThread(repoThread);
    repository->moveToThread(repoThread);
    repoThread->start();

    auto *emailModel = new EmailModel();
    auto *service = new Service(repository, emailModel);


    QObject::connect(service, &Service::requestEnvelopedEmails, repository, &RepositoryEmail::envelopeEmailsSlot);
    QObject::connect(service, &Service::requestBody, repository, &RepositoryEmail::fetchBodySlot);
    QObject::connect(repository, &RepositoryEmail::emailsReadySignal, service, &Service::onEmailsFetched);

    QObject::connect(repoThread, &QThread::finished, repository, &QObject::deleteLater);
    QObject::connect(repoThread, &QThread::finished, repoThread, &QObject::deleteLater);


    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("emailModel", emailModel);
    engine.rootContext()->setContextProperty("service", service);
    engine.load(QUrl(QStringLiteral("qrc:/EmailClient/main.qml")));


    service->envelopeEmails();

    int result = a.exec();

    repoThread->quit();
    repoThread->wait();

    return result;
}
