#ifndef EMAIL_H
#define EMAIL_H

#include "message.h"

typedef struct UploadStatus
{
    int counter = 0;
} uploadStatus;

class Email
{

    QString _uid;
    QString _date;
    QString _name;
    QString _address;
    QString _subject;
    QStringList _to;
    QStringList _cc;
    QStringList _bcc;
    QString _in_reply_to;
    QString _message_id;


    // std::unique_ptr<Message> _message;
    // Message* _message;


public:
    // Email(QString, QString, QString, std::unique_ptr<Message>);
    Email();
    Email(QString, QString, QString, QString, QString);
    // Email(QString, QString, QString, Message*);

    QString getSubject();
    // int sendEmail();
    // size_t payload_source(void*, size_t, size_t, void*);
    // typedef struct {int lines_read;} UploadStatus;

};

#endif // EMAIL_H
