#include "service.h"
#include "QtCore/qdir.h"
#include "QtCore/qurl.h"
#include <qfuture.h>
#include <qtconcurrentrun.h>
#include <QDesktopServices>

Service::Service(RepositoryEmail *repo, EmailModel* emailmodel, AttachmentModel* attmodel, QObject* parent)
    : QObject(parent)
    , _repo(repo)
    , _email_model(emailmodel)
    , _attachment_model(attmodel)
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
    qDebug() << "ATTACHMENTS COUNT: " << _pendingAttachments.size();
    tryEmitBody();
}

void Service::onAttachmentsReady(QList<RepositoryEmail::Attachment> attachments)
{
    _pendingAttachments = attachments;
    _attachment_model->setAttachments(_pendingAttachments);
}

void Service::tryEmitBody()
{
    if (_htmlReceived && !_attachmentsReceived)
    {
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


void Service::openAttachment(int index)
{
    const auto& att = _attachment_model->getAttachment(index);

    QString tempPath = QDir::tempPath() + "/" + att.filename;
    QFile file(tempPath);
    qDebug() << "tempPath: " << tempPath;

    if (file.open(QIODevice::WriteOnly))
    {
        file.write(att.data);
        file.close();
        QDesktopServices::openUrl(QUrl::fromLocalFile(tempPath));
    }
    else
    {
        qDebug() << "Nie można zapisać pliku tymczasowego:" << tempPath;
    }
}

Q_INVOKABLE bool Service::changeReadState(Email email)
{
    if(!email.getIsRead())
    {
        email.setRead(true);
        _email_model->setEmail(email);
    }
}
