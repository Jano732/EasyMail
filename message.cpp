#include "message.h"
#include <qdebug.h>

Message::Message()
{

}

Message::~Message()
{

}

void Message::setRecipients(const QString &recipient) { _recipients = recipient; }
void Message::setSubject(const QString &subject) { _subject = subject; }
void Message::setBody(const QString &body) { _body = body; }

QString Message::getRecipients() const { return _recipients; }
QString Message::getSubject() const { return _subject; }
QString Message::getBody() const { return _body; }
