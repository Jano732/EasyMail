#include "repositoryemail.h"
#include "vmime/addressList.hpp"
#include "vmime/mailbox.hpp"
#include "vmime/text.hpp"
#include <qregularexpression.h>
#include <qtconcurrentrun.h>

RepositoryEmail::RepositoryEmail(ImapClient *client, QObject *parent)
    : QObject(parent)
    , _client(client)
{}


void RepositoryEmail::envelopeEmailsSlot()
{
    std::vector<Email> envelopedEmails;

    vmime::shared_ptr<vmime::net::folder> folder = _client->getFolder();
    int count = folder->getMessageCount();

    // Pobieramy ostatnie 30 wiadomości
    _messages = folder->getMessages(vmime::net::messageSet::byNumber(std::max(1, count - 35), count));

    // Kluczowe: pobranie atrybutów ENVELOPE (nagłówki) i FLAGS (np. przeczytana/nieprzeczytana)
    folder->fetchMessages(_messages, vmime::net::fetchAttributes::FLAGS | vmime::net::fetchAttributes::ENVELOPE);

    for (unsigned int i = 0; i < _messages.size(); ++i)
    {
        vmime::shared_ptr<vmime::net::message> msg = _messages[i];
        vmime::shared_ptr<const vmime::header> header = msg->getHeader();

        auto textToQs = [](vmime::shared_ptr<const vmime::headerField> field) -> QString {
            if (!field || !field->getValue()) return QString("");
            try {
                auto textVal = vmime::dynamicCast<const vmime::text>(field->getValue());
                if (textVal) {
                    return QString::fromStdString(textVal->getConvertedText(vmime::charsets::UTF_8));
                }
            } catch (...) {}
            return QString("");
        };

        auto addrToQs = [](vmime::shared_ptr<const vmime::headerField> field) -> QString {
            if (!field || !field->getValue()) return QString("");

            try {
                auto addrList = vmime::dynamicCast<const vmime::addressList>(field->getValue());
                if (addrList) {
                    QStringList formattedList;
                    for (size_t j = 0; j < addrList->getAddressCount(); ++j) {
                        auto addr = addrList->getAddressAt(j);
                        auto mailbox = vmime::dynamicCast<const vmime::mailbox>(addr);

                        if (mailbox) {
                            // Pobieramy nazwę jako obiekt vmime::text
                            const vmime::text& nameText = mailbox->getName();
                            // getConvertedText automatycznie zajmuje się dekodowaniem RFC 2047
                            QString decodedName = QString::fromStdString(nameText.getConvertedText(vmime::charsets::UTF_8));

                            QString email = QString::fromStdString(mailbox->getEmail().generate());

                            if (!decodedName.isEmpty()) {
                                formattedList << QString("%1 <%2>").arg(decodedName, email);
                            } else {
                                formattedList << email;
                            }
                        } else {
                            formattedList << QString::fromStdString(addr->generate());
                        }
                    }
                    return formattedList.join(", ");
                }
            } catch (...) {}

            // Poprawiony fallback: Tworzymy obiekt text z surowego stringa, co wymusza parsing
            try {
                vmime::text t;
                t.parse(field->getValue()->generate());
                return QString::fromStdString(t.getConvertedText(vmime::charsets::UTF_8));
            } catch (...) {
                return QString::fromStdString(field->getValue()->generate());
            }
        };

        auto rawToQs = [](vmime::shared_ptr<const vmime::headerField> field) -> QString {
            if (!field || !field->getValue()) return QString("");
            return QString::fromStdString(field->getValue()->generate());
        };

        QString uid = QString::fromStdString(msg->getUID());
        QString subject = textToQs(header->Subject());

        QString from = addrToQs(header->From());
        QString sender = addrToQs(header->Sender());
        QString replyTo = addrToQs(header->ReplyTo());
        QString to = addrToQs(header->To());
        QString cc = addrToQs(header->Cc());
        QString bcc = addrToQs(header->Bcc());

        QString messageId = rawToQs(header->MessageId());
        QString inReplyTo = rawToQs(header->InReplyTo());

        QString convertedDate = "";
        if (header->Date() && header->Date()->getValue()) {
            auto dateVal = vmime::dynamicCast<const vmime::datetime>(header->Date()->getValue());
            if (dateVal) {
                convertedDate = QString::fromStdString(dateVal->generate());
            }
        }

        // qDebug() << "UID:" << uid;
        // qDebug() << "Subject:" << subject;
        // qDebug() << "From:" << from;
        // qDebug() << "To:" << to;
        // qDebug() << "Date:" << convertedDate;
        // qDebug() << "Message-ID:" << messageId;
        // qDebug() << "------------------------------------";

        Email email(uid, convertedDate, subject, from, sender, replyTo, to, cc, bcc, inReplyTo, messageId);
        envelopedEmails.push_back(email);
    }

    emit emailsEnvelopedReady(envelopedEmails);
}
