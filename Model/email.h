#pragma once
#ifndef EMAIL_H
#define EMAIL_H

#include <qobject.h>

typedef struct UploadStatus
{
    int counter = 0;
} uploadStatus;

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

    // std::unique_ptr<Message> _message;
    // Message* _message;

public:
    // Email(QString, QString, QString, std::unique_ptr<Message>);
    Email();
    Email(QString, QString, QString, QString, QString, QString, QString, QString, QString, QString, QString);
    // Email(QString, QString, QString, Message*);

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

    // int sendEmail();
    // size_t payload_source(void*, size_t, size_t, void*);
    // typedef struct {int lines_read;} UploadStatus;

};

#endif // EMAIL_H
