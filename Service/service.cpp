#include "service.h"
#include <qfuture.h>
#include <qtconcurrentrun.h>

// Service::Service(QString url, int port, QString address, QString password)
//     : _client(url, port, address, password)
// {
//     _client.connect();
//     _client.login();
//     _client.selectInbox("INBOX");
//     _emails = _client.envelope();
// }

Service::Service(RepositoryEmail *repo, EmailModel* emailmodel, QObject* parent)
    : QObject(parent)
    , _repo(repo)
    , _email_model(emailmodel)
{};

void Service::onEmailsEnvelope(std::vector<Email>& emails)
{
    _emails = emails;
    _email_model->setEmails(_emails);
    emit envelopedDataReady();
}

void Service::envelopeEmails()
{
    emit requestEnvelopedEmails();
}
