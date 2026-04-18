#pragma once
#ifndef EMAIL_H
#define EMAIL_H

#include <qobject.h>



class Email
{

    QString _uid;
    QString _date;
    QString _subject;
    QString _from;
    QString _sender;
    QString _reply_to;
    QString _to;
    QString _cc;
    QString _bcc;
    QString _in_reply_to;
    QString _message_id;
    bool _isRead;

    // std::unique_ptr<Message> _message;
    // Message* _message;

public:
    // Email(QString, QString, QString, std::unique_ptr<Message>);
    Email();
    Email(QString, QString, QString, QString, QString, QString, QString, QString, QString, QString, QString, bool);

    QString getUid() const;
    QString getDate() const;
    QString getSubject() const;
    QString getFrom() const;
    QString getSender();
    QString getReplyTo();
    QString getTo();
    QString getCc();
    QString getBcc();
    QString getInreplyTo();
    QString getMessageId();



};

#endif // EMAIL_H
