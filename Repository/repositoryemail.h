#pragma once
#ifndef REPOSITORYEMAIL_H
#define REPOSITORYEMAIL_H

#include "../Infrastructure/imapclient.h"
#include "../Model/email.h"
#include <QObject>


class RepositoryEmail : public QObject
{
    Q_OBJECT

    struct BodyStructure{

        int ident;
        std::string type;
        std::string subtype;
        int child_index;
        size_t size;
    };

    ImapClient *_client;
    std::vector<BodyStructure> _parts;
    std::vector<vmime::shared_ptr<vmime::net::message>> _messages;

    void explorePart(vmime::shared_ptr<const vmime::net::messagePart>, int);
    void analyzeMultiPartAlternative(vmime::shared_ptr<vmime::net::message>);

public:

    RepositoryEmail(ImapClient*, QObject* parent = nullptr);
    void envelope();
    void fetchBody(QString);

public slots:

    void envelopeEmailsSlot();

signals:

    void emailsEnvelopedReady(std::vector<Email>& emails);
    void htmlReady(QString);

};



#endif // REPOSITORYEMAIL_H
