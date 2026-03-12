#ifndef REPOSITORYEMAIL_H
#define REPOSITORYEMAIL_H

#include "../Infrastructure/imapclient.h"
#include <QObject>

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

    ImapClient *_client;
    std::vector<Token> _bodyTokens;

public:
    RepositoryEmail(ImapClient*, QObject* parent = nullptr);

    // std::vector<Email> envelope();

    QString decode(QString&);
    QString processList(QString&);
    std::vector<Email> envelopeDataParser(QString &);
    QString extractNextTokenEnvelope(QString&, int&);
    Email processEnvelopeResponse(QString);
    void parseBodyPart(QString&, int&);
    void parseBodySinglePart(QString&, int&);
    void parseBodyMultiPart(QString&, int&);
    void tokenizeBody(QString&);

    friend QDebug operator<<(QDebug, const Token&);



public slots:

    void envelopeEmailsSlot();
    void fetchBodySlot(Email);

signals:

    void emailsReadySignal(std::vector<Email>);

};

inline QDebug operator<<(QDebug dbg, const RepositoryEmail::Token& t)
{
    dbg << t.typeName() << " : " << t._value;
    return dbg;
}

#endif // REPOSITORYEMAIL_H
