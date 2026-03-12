#include "email.h"
#include "curl/curl.h"
#include <_mingw_mac.h>
#include <qdebug.h>

// Email::Email(QString username, QString password, QString url, std::unique_ptr<Message> message):
//     _username(username),
//     _password(password),
//     _smtp_url(url),
//     _message(std::move(message)) {}

// Email::Email(QString username, QString password, QString url, Message* message):
//     _username(username),
//     _password(password),
//     _smtp_url(url),
//     _message(message) {qDebug() << _username << ", " << _password << ", " << _smtp_url << ", " << _message->getSubject();}

Email::Email(QString uid, QString date, QString subject, QString from, QString sender, QString reply_to, QString to, QString cc,  QString bcc,  QString in_reply_to, QString message_id):
    _uid(uid),
    _date(date),
    _subject(subject),
    _from(from),
    _sender(sender),
    _reply_to(reply_to),
    _to(to),
    _cc(cc),
    _bcc(bcc),
    _in_reply_to(in_reply_to),
    _message_id(message_id) {}

Email::Email() {}

QString Email::getUid() const {return _uid; }

QString Email::getDate() const { return _date; }

QString Email::getSubject() const { return _subject; }

QString Email::getFrom() const { return _from; }

QString Email::getSender() {return _sender; }

QString Email::getReplyTo() { return _reply_to; }

QString Email::getTo() {return _to; }

QString Email::getCc() {return _cc;}

QString Email::getBcc() { return _bcc; }

QString Email::getInreplyTo() {return _in_reply_to; }

QString Email::getMessageId() {return _message_id; }


// int Email::sendEmail()
// {
//     CURLcode res = CURLE_OK;
//     struct curl_slist* recipients = NULL;
//     UploadStatus upload_ctx = { 0 };
//     const char* from = _username.toUtf8().constData();
//     const char* to = "michniaktomek@gmail.com";
//     CURL* curl = curl_easy_init();

//     if (curl)
//     {
//         curl_easy_setopt(curl, CURLOPT_USERNAME, from);
//         curl_easy_setopt(curl, CURLOPT_PASSWORD, "PLGT-CATL-KDVE-OTRN");
//         curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.poczta.onet.pl:465");
//         // start with normal connection, and upgrade to TLS using STARTTLS
//         curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
//         curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//         curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
//         curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from);
//         recipients = curl_slist_append(recipients, to);
//         curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
//         //curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
//         curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
//         curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
//         // useful for debugging encryped traffic
//         curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

//         // send the message
//         res = curl_easy_perform(curl);
//         if (res != CURLE_OK) fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
//         curl_slist_free_all(recipients);
//         curl_easy_cleanup(curl);
//     }

//     return (int)res;
// }

// size_t Email::payload_source(void* ptr, size_t size, size_t nmemb, void* userp)
// {
//     if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) return 0;

//     UploadStatus* upload = (UploadStatus*)userp;

//     const char* data;// = payload[upload->lines_read];

//     if (data)
//     {
//         size_t len = strlen(data);
//         memcpy(ptr, data, len);
//         upload->lines_read++;
//         return len;
//     }
//     return 0;
// }
