#pragma once
#ifndef SERVICE_H
#define SERVICE_H

#include "../Model/email.h"
#include "../Model/message.h"
#include "../Repository/repositoryemail.h"
#include "../View/emailmodel.h"

class Service : public QObject
{
    Q_OBJECT

    RepositoryEmail *_repo;
    EmailModel *_email_model;
    std::vector<Email> _emails;
    std::vector<Message> _messages;

public:

    explicit Service(RepositoryEmail*, EmailModel*,  QObject* parent = nullptr);

    void envelopeEmails();

signals:

    void envelopedDataReady();
    void requestEnvelopedEmails();

public slots:

    void onEmailsEnvelope(std::vector<Email>&);

};

#endif // SERVICE_H
