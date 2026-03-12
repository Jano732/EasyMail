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

void Service::envelopeEmails()
{
    emit requestEnvelopedEmails();
    qDebug() << "Wysłano sygnał requestEnvelopedEmails";
}

void Service::onEmailsFetched(const std::vector<Email> emails)
{
    _emails = emails;
    _email_model->setEmails(emails);
    qDebug() << "Dane odebrane asynchronicznie";
    emit envelopedDataReady();
}

Q_INVOKABLE void Service::requestBodyByUid(Email email)
{
    qDebug() << email.getUid();
    emit requestBody(email);
}


Q_INVOKABLE Email Service::getEmailByUid(QString uid)
{

    if(!_emails.empty())
    {
        for(auto email : _emails) if(email.getUid() == uid) return email;
    }

    return Email();
}

std::vector<Email> Service::getEmails() {return _emails; }

EmailModel* Service::getEmailModel() { return _email_model; }
