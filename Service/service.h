#pragma once
#include "View/attachmentmodel.h"
#ifndef SERVICE_H
#define SERVICE_H

#include "../Model/email.h"
#include "../Model/message.h"
#include "../Repository/repositoryemail.h"
#include "../View/emailmodel.h"

class Service : public QObject
{
    Q_OBJECT

    RepositoryEmail  *_repo;
    EmailModel       *_email_model;
    AttachmentModel *_attachment_model;
    std::vector<Email> _emails;
    std::vector<Message> _messages;

    QString _pendingHtml;
    QList<RepositoryEmail::Attachment> _pendingAttachments;
    bool _htmlReceived;
    bool _attachmentsReceived;

    void tryEmitBody();

public:

    explicit Service(RepositoryEmail*, EmailModel*, AttachmentModel*, QObject* parent = nullptr);

    void envelopeEmails();

    Q_INVOKABLE void requestBodyOfAnEmail(Email email);
    Q_INVOKABLE Email getEmailByUid(QString);
    Q_INVOKABLE void openAttachment(int index);

signals:

    void envelopedDataReady();
    void requestEnvelopedEmails();
    void requestBody(QString);
    void htmlReady(QString);
    void attachmentsReady(QList<RepositoryEmail::Attachment>);

public slots:

    void onEmailsEnvelope(std::vector<Email>&);
    void onHtmlReady(QString);
    void onAttachmentsReady(QList<RepositoryEmail::Attachment>);

};

#endif // SERVICE_H
