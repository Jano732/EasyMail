#include "repositoryemail.h"
#include "vmime/addressList.hpp"
#include "vmime/mailbox.hpp"
#include "vmime/text.hpp"
#include "vmime/utility/inputStreamStringAdapter.hpp"
#include "vmime/utility/outputStreamAdapter.hpp"
#include <qregularexpression.h>
#include <qtconcurrentrun.h>
#include "vmime/utility/encoder/encoderFactory.hpp"
#ifdef _WIN32
#include <winsock2.h>
#endif



RepositoryEmail::RepositoryEmail(ImapClient *client, QObject *parent)
    : QObject(parent)
    , _client(client)
{}


void RepositoryEmail::envelopeEmailsSlot()
{
    std::vector<Email> envelopedEmails;

    vmime::shared_ptr<vmime::net::folder> folder = _client->getFolder();
    int count = folder->getMessageCount();

    _messages = folder->getMessages(vmime::net::messageSet::byNumber(std::max(1, count - 35), count));

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
                            const vmime::text& nameText = mailbox->getName();
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

        Email email(uid.trimmed(), convertedDate.trimmed(), subject.trimmed(), from.trimmed(), sender.trimmed(), replyTo.trimmed(), to.trimmed(), cc.trimmed(), bcc.trimmed(), inReplyTo.trimmed(), messageId.trimmed());
        envelopedEmails.push_back(email);
    }

    emit emailsEnvelopedReady(envelopedEmails);
}


void RepositoryEmail::fetchBody(QString uid)
{
    _parts.clear();
    auto folder = _client->getFolder();
    vmime::string casted_uid = uid.toStdString();
    std::vector<vmime::shared_ptr<vmime::net::message>> messages = folder->getMessages(vmime::net::messageSet::byUID(casted_uid));
    vmime::shared_ptr<vmime::net::message> msg = messages.back();
    vmime::utility::outputStreamAdapter out(std::cout);

    folder->fetchMessage(msg, vmime::net::fetchAttributes::STRUCTURE);

    vmime::shared_ptr<const vmime::net::messageStructure> structPtr = msg->getStructure();

    explorePart(structPtr->getPartAt(0), 0, 0);

    // qDebug() << _parts.size();
    // for(auto& p : _parts) qDebug() << p.type << ", " << p.subtype << ", " << p.ident;

    BodyStructure root = _parts.front();


    if(root.type == vmime::mediaTypes::MULTIPART)
    {

        if(root.subtype == vmime::mediaTypes::MULTIPART_ALTERNATIVE)
        {
            analyzeMultiPartAlternative(msg);
        }
        else if(root.subtype == vmime::mediaTypes::MULTIPART_MIXED)
        {
            qDebug() << "multipart/mixed";
        }
        else if(root.subtype == vmime::mediaTypes::MULTIPART_RELATED)
        {
            qDebug() << "multiupart/related";
        }
    }

}

void RepositoryEmail::explorePart(vmime::shared_ptr<const vmime::net::messagePart> part, int level = 0, int index = 0)
{
    BodyStructure bs;
    vmime::mediaType mt = part->getType();

    bs.type = mt.getType();
    bs.subtype = mt.getSubType();
    bs.size = part->getSize();
    bs.ident = level;
    bs.child_index = index;

    qDebug() << bs.type << ", " << bs.subtype << ", " << bs.size << ", " << bs.ident;

    _parts.push_back(bs);

    for(int i = 0; i < part->getPartCount(); ++i)
    {
        explorePart(part->getPartAt(i), level + 1, i);
    }
}


void RepositoryEmail::analyzeMultiPartAlternative(vmime::shared_ptr<vmime::net::message> msg)
{

    for(auto& p : _parts)
    {
        if(p.subtype == vmime::mediaTypes::TEXT_HTML)
        {
            try{
                auto valid_part = msg->getStructure()->getPartAt(0)->getPartAt(p.child_index);
                std::stringstream ss;
                vmime::utility::outputStreamAdapter out(ss);
                msg->extractPart(valid_part, out);

                std::string raw = ss.str();
                size_t bodyStart = raw.find("\r\n\r\n");
                if(bodyStart == std::string::npos)
                    bodyStart = raw.find("\n\n");

                std::string body = raw.substr(bodyStart + 4);

                auto registeredEncoder = vmime::utility::encoder::encoderFactory::getInstance()->getEncoderByName("quoted-printable");
                auto encoder = registeredEncoder->create();

                vmime::utility::inputStreamStringAdapter in(body);
                std::ostringstream decoded;
                vmime::utility::outputStreamAdapter outDecoded(decoded);
                encoder->decode(in, outDecoded);

                QString html = QString::fromUtf8(decoded.str().c_str());
                emit htmlReady(html);
            } catch(vmime::exception& e){
                std::cerr << e.what();
            }
        }
    }

}
