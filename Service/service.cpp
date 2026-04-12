#include "service.h"
#include <qfuture.h>
#include <qtconcurrentrun.h>

Service::Service(RepositoryEmail *repo, EmailModel* emailmodel, QObject* parent)
    : QObject(parent)
    , _repo(repo)
    , _email_model(emailmodel)
    , _htmlReceived(false)
    , _attachmentsReceived(false)
{}

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
    _pendingHtml = QString();
    _pendingAttachments.clear();
    _htmlReceived = false;
    _attachmentsReceived = false;

    emit requestBody(email.getUid());
}

Q_INVOKABLE Email Service::getEmailByUid(QString uid)
{
    for (auto& email : _emails)
    {
        if (email.getUid() == uid) return email;
    }
    return Email();
}

void Service::onHtmlReady(QString html)
{
    _pendingHtml = html;
    _htmlReceived = true;
    tryEmitBody();
}

void Service::onAttachmentsReady(QList<RepositoryEmail::Attachment> attachments)
{
    _pendingAttachments = attachments;
    _attachmentsReceived = true;
    tryEmitBody();
}

void Service::tryEmitBody()
{
    // multipart/alternative i plain emitują tylko htmlReady bez załączników —
    // w takim razie attachmentsReady nigdy nie przyjdzie, więc emitujemy od razu
    if (_htmlReceived && !_attachmentsReceived)
    {
        // Dajemy chwilę — jeśli repo nie wyemituje attachmentsReady,
        // znaczy że ich nie ma (alternative / plain)
        // Używamy flagi: repo zawsze emituje attachmentsReady gdy są załączniki,
        // więc jeśli html przyszedł pierwszy — czekamy jeszcze jedną rundę event loop
        QMetaObject::invokeMethod(this, [this]() {
            if (!_attachmentsReceived)
            {
                emit htmlReady(_pendingHtml);
                _htmlReceived = false;
            }
        }, Qt::QueuedConnection);
        return;
    }

    if (_htmlReceived && _attachmentsReceived)
    {
        emit htmlReady(_pendingHtml);
        emit attachmentsReady(_pendingAttachments);
        _htmlReceived = false;
        _attachmentsReceived = false;
    }
}
