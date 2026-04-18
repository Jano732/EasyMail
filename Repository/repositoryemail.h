#pragma once
#ifndef REPOSITORYEMAIL_H
#define REPOSITORYEMAIL_H

#include "../Infrastructure/imapclient.h"
#include "../Model/email.h"
#include <QObject>
#include <QByteArray>
#include <QList>
#include <QMap>


class RepositoryEmail : public QObject
{
    Q_OBJECT

    struct BodyStructure{

        int ident;
        std::string type;
        std::string subtype;
        int child_index;
        size_t size;
        std::vector<int> path;
    };

    struct InlineResource {
        QString mimeType;
        QByteArray data;
    };

public:

    struct Attachment {
        QString filename;
        QString mimeType;
        QByteArray data;
    };

private:

    ImapClient *_client;
    std::vector<BodyStructure> _parts;
    std::vector<vmime::shared_ptr<vmime::net::message>> _messages;

    void explorePart(vmime::shared_ptr<const vmime::net::messagePart>, int, int, std::vector<int>);
    void analyzeMultiPartAlternative(vmime::shared_ptr<vmime::net::message>);
    void analyzeMultiPartMixed(vmime::shared_ptr<vmime::net::message>);
    void analyzeMultiPartRelated(vmime::shared_ptr<vmime::net::message>);

    vmime::shared_ptr<const vmime::net::messagePart> getPartByPath(vmime::shared_ptr<vmime::net::message>, const std::vector<int>&);
    std::string extractRawPart(vmime::shared_ptr<vmime::net::message>, const BodyStructure&);
    QString decodePartContent(const std::string&, const std::string&);
    QByteArray extractAttachment(vmime::shared_ptr<vmime::net::message>, const BodyStructure&);
    QString resolveAttachmentFilename(const std::string&, const BodyStructure&);
    QString extractCid(const std::string&);
    QString resolveCidReferences(const QString&, const QMap<QString, InlineResource>&);
    bool isDirectChildOf(const BodyStructure&, const BodyStructure&);
    std::string detectEncoding(const std::string&);

public:

    RepositoryEmail(ImapClient*, QObject* parent = nullptr);
    void envelope();
    void fetchBody(QString);
    void analyzeSinglePart(vmime::shared_ptr<vmime::net::message>, BodyStructure& bs);


public slots:

    void envelopeEmailsSlot();

signals:

    void emailsEnvelopedReady(std::vector<Email>& emails);
    void htmlReady(QString);
    void attachmentsReady(QList<RepositoryEmail::Attachment>);
    void resetAttachments();

};

#endif // REPOSITORYEMAIL_H
