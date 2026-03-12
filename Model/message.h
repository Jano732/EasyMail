#ifndef MESSAGE_H
#define MESSAGE_H

#include <qobject.h>
class Message
{

    QString _recipients;
    QString _subject;
    QString _body;
    QString _attachements;

public:
    Message();
    ~Message();

    void setRecipients(const QString &recipient);
    void setSubject(const QString &subject);
    void setBody(const QString &body);

    QString getRecipients() const;
    QString getSubject() const;
    QString getBody() const;
};


#endif // MESSAGE_H
