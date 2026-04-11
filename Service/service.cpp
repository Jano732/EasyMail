#include "service.h"
#include <qfuture.h>
#include <qtconcurrentrun.h>

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

Q_INVOKABLE void Service::requestBodyOfAnEmail(Email email)
{
    emit requestBody(email.getUid());
}

Q_INVOKABLE Email Service::getEmailByUid(QString uid)
{
    for(auto& email : _emails)
    {
        if(email.getUid() == uid) return email;
    }
}

void Service::onHtmlReady(QString s)
{
    emit htmlReady(s);
}
