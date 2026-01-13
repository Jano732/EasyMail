#ifndef EMAIL_H
#define EMAIL_H

#include "message.h"

typedef struct UploadStatus
{
    int counter = 0;
} uploadStatus;

class Email
{

    QString _username;
    QString _password;
    QString _smtp_url;
    std::unique_ptr<Message> _message;
    // Message* _message;


public:
    Email(QString, QString, QString, std::unique_ptr<Message>);
    // Email(QString, QString, QString, Message*);
    ~Email();

    int sendEmail();
    size_t payload_source(void*, size_t, size_t, void*);
    typedef struct {int lines_read;} UploadStatus;

};

#endif // EMAIL_H
