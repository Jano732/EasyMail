#pragma once
#ifndef REPOSITORYEMAIL_H
#define REPOSITORYEMAIL_H

#include "../Infrastructure/imapclient.h"
#include "../Model/email.h"
#include <QObject>


class RepositoryEmail : public QObject
{
    Q_OBJECT

    ImapClient *_client;
    std::vector<vmime::shared_ptr<vmime::net::message>> _messages;


public:

    RepositoryEmail(ImapClient*, QObject* parent = nullptr);

    void envelope();


public slots:

    void envelopeEmailsSlot();

signals:

    void emailsEnvelopedReady(std::vector<Email>& emails);

};



#endif // REPOSITORYEMAIL_H
