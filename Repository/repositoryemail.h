#pragma once
#ifndef REPOSITORYEMAIL_H
#define REPOSITORYEMAIL_H

#include "../Infrastructure/imapclient.h"
#include "../Model/email.h"
#include <QObject>

struct ResolvedBody {

    QString main_content;
    enum class ContentType {HTML, PLAINTEXT, UNKNOWN} content_type = ContentType::UNKNOWN;
    QMap<QString, QByteArray> inline_images;

    struct Attachment
    {
        QString filename;
        QString mimeType;
        QByteArray data;
    };
    QList<Attachment> attachments;
};

class RepositoryEmail : public QObject
{
    Q_OBJECT

    enum class TokenType { LPAREN, RPAREN, STRING, ATOM};

    struct Token{

        Token(RepositoryEmail::TokenType type, QString element)
            :_type(type)
            ,_value(element)
        {}

        TokenType _type;
        QString _value;

        QString typeName() const {
            switch(_type){
            case TokenType::LPAREN: return "LPAREN";
            case TokenType::RPAREN: return "RPAREN";
            case TokenType::STRING: return "STRING";
            case TokenType::ATOM: return "ATOM";
            }
            return "?";
        };
    };

    struct Bodystructure{

        QString section;
        QString type;
        QString subtype;
        QList<QString> params;
        QString content_id;
        QString description;
        QString encoding;
        QString size;
        QString lines;
        QString md5;
        QList<QString> dispozition;
        QString language;
        QString location;
    };

    struct BodystructureMultipart {
        QString subtype;
        QString section;
        QString boundary;
    };



    ImapClient *_client;
    std::vector<Token> _bodyTokens;
    std::vector<Bodystructure> _tokenizedBodystructure;
    std::vector<BodystructureMultipart> _tokenizedBodystructureMultipart;
    std::vector<QString> _download_list;
    std::vector<QString> _download_list_atachments;
    std::vector<QString> _fetched_bodies;

public:
    RepositoryEmail(ImapClient*, QObject* parent = nullptr);

    // std::vector<Email> envelope();



    QString decode(QString&);
    QString processList(QString&);
    std::vector<Email> envelopeDataParser(QString &);
    QString extractNextTokenEnvelope(QString&, int&);
    Email processEnvelopeResponse(QString);

    void processBodyResponse(QString&);
    void bodyParsePart(int&, QString);
    void bodyParseSinglePart(int&, QString);
    void bodyParseMultiPart(int&, QString);
    void bodyTokenize(QString&);
    void bodyProcessNode(QString, BodystructureMultipart&);
    QString bodyResolveBestPart(QString, QString);
    Bodystructure analyzeStructure();
    void resolveBody();
    void skipList(int&);
    QByteArray extractRawBytes(const QString&);
    QString decodeBodyPart(const QString&, const QString&);


    friend QDebug operator<<(QDebug, const Token&);
    friend QDebug operator<<(QDebug, const Bodystructure&);


public slots:

    void envelopeEmailsSlot();
    void fetchBodySlot(Email);

signals:

    void emailsReadySignal(std::vector<Email>);
    void bodyReadySignal(ResolvedBody);

};

inline QDebug operator<<(QDebug dbg, const RepositoryEmail::Token& t)
{
    dbg << t.typeName() << " : " << t._value;
    return dbg;
}

inline QDebug operator<<(QDebug dbg, const RepositoryEmail::Bodystructure& b)
{
    dbg << "section: \t" << b.section << "\n";
    dbg << "type:\t\t" << b.type << "\n";
    dbg << "subtype:\t" << b.subtype << "\n";

    QString params;
    for(auto& elements : b.params)
    {
        params += elements;
        params += ", ";
    }

    dbg << "params:\t\t" << params << "\n";
    dbg << "content:\t" << b.content_id << "\n";
    dbg << "description:\t" << b.description << "\n";
    dbg << "encoding:\t" << b.encoding << "\n";
    dbg << "size:\t\t" << b.size << "\n";
    dbg << "lines:\t\t" << b.lines << "\n";
    dbg << "md5:\t\t" << b.md5 << "\n";

    QString dispozition;
    for(auto& elements : b.dispozition)
    {
        dispozition += elements;
        dispozition += ", ";
    }

    dbg << "dispozition:\t" << dispozition << "\n";
    dbg << "language:\t" << b.language << "\n";
    dbg << "location:\t" << b.location << "\n";

    return dbg;
}


// inline QDebug operator <<(QDebug dbg, const RepositoryEmail::)

#endif // REPOSITORYEMAIL_H
