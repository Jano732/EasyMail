#include "repositoryemail.h"
#include "vmime/addressList.hpp"
#include "vmime/mailbox.hpp"
#include "vmime/text.hpp"
#include "vmime/utility/inputStreamStringAdapter.hpp"
#include "vmime/utility/outputStreamAdapter.hpp"
#include "vmime/utility/encoder/encoderFactory.hpp"
#include <qregularexpression.h>
#include <qtconcurrentrun.h>
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
        bool isRead = false;
        int flags = msg->getFlags();
        isRead = ((flags & vmime::net::message::FLAG_SEEN) != 0);
        qDebug() << flags;

        Email email(uid.trimmed(), convertedDate.trimmed(), subject.trimmed(), from.trimmed(), sender.trimmed(), replyTo.trimmed(), to.trimmed(), cc.trimmed(), bcc.trimmed(), inReplyTo.trimmed(), messageId.trimmed(), isRead);
        envelopedEmails.push_back(email);
    }

    emit emailsEnvelopedReady(envelopedEmails);
}


void RepositoryEmail::fetchBody(QString uid)
{
    _parts.clear();
    auto folder = _client->getFolder();
    auto messages = _client->getMessageByUid(uid);
    _client->markAsRead(uid);

    vmime::shared_ptr<vmime::net::message> msg = messages.back();

    folder->fetchMessage(msg, vmime::net::fetchAttributes::STRUCTURE);

    vmime::shared_ptr<const vmime::net::messageStructure> structPtr = msg->getStructure();
    explorePart(structPtr->getPartAt(0), 0, 0, {});

    BodyStructure root = _parts.front();

    if (root.type == vmime::mediaTypes::MULTIPART)
    {
        if (root.subtype == vmime::mediaTypes::MULTIPART_ALTERNATIVE)
            analyzeMultiPartAlternative(msg);
        else if (root.subtype == vmime::mediaTypes::MULTIPART_MIXED)
            analyzeMultiPartMixed(msg);
        else if (root.subtype == vmime::mediaTypes::MULTIPART_RELATED)
            analyzeMultiPartRelated(msg);
    }
    else
    {
        analyzeSinglePart(msg, root);
    }
}


void RepositoryEmail::explorePart(vmime::shared_ptr<const vmime::net::messagePart> part, int level, int index, std::vector<int> parentPath)
{
    BodyStructure bs;
    vmime::mediaType mt = part->getType();

    bs.type = mt.getType();
    bs.subtype = mt.getSubType();
    bs.size = part->getSize();
    bs.ident = level;
    bs.child_index = index;
    bs.path = parentPath;
    bs.path.push_back(index);

    qDebug() << QString::fromStdString(bs.type)<< ", " << QString::fromStdString(bs.subtype) << ", " << bs.size << ", level:" << bs.ident;

    _parts.push_back(bs);

    for (int i = 0; i < part->getPartCount(); ++i)
        explorePart(part->getPartAt(i), level + 1, i, bs.path);
}


vmime::shared_ptr<const vmime::net::messagePart> RepositoryEmail::getPartByPath(vmime::shared_ptr<vmime::net::message> msg,const std::vector<int>& path)
{
    auto current = msg->getStructure()->getPartAt(0);
    for (size_t i = 1; i < path.size(); ++i)
        current = current->getPartAt(path[i]);
    return current;
}


std::string RepositoryEmail::extractRawPart(vmime::shared_ptr<vmime::net::message> msg, const BodyStructure& bs)
{
    auto part = getPartByPath(msg, bs.path);
    std::stringstream ss;
    vmime::utility::outputStreamAdapter out(ss);
    msg->extractPart(part, out);
    return ss.str();
}


QString RepositoryEmail::decodePartContent(const std::string& raw, const std::string& encoding)
{
    size_t bodyStart = raw.find("\r\n\r\n");
    if (bodyStart == std::string::npos)
        bodyStart = raw.find("\n\n");
    if (bodyStart == std::string::npos)
        return QString::fromUtf8(raw.c_str());

    std::string body = raw.substr(bodyStart + 4);

    if (body.find_first_not_of(" \r\n\t") == std::string::npos)
        return QString::fromUtf8(raw.c_str());

    try {
        auto encoder = vmime::utility::encoder::encoderFactory::getInstance()->getEncoderByName(encoding)->create();
        vmime::utility::inputStreamStringAdapter in(body);
        std::ostringstream decoded;
        vmime::utility::outputStreamAdapter outDecoded(decoded);
        encoder->decode(in, outDecoded);
        return QString::fromUtf8(decoded.str().c_str());
    } catch (...) {
        return QString::fromUtf8(body.c_str());
    }
}


QByteArray RepositoryEmail::extractAttachment(vmime::shared_ptr<vmime::net::message> msg, const BodyStructure& bs)
{
    std::string raw = extractRawPart(msg, bs);

    size_t bodyStart = raw.find("\r\n\r\n");
    if (bodyStart == std::string::npos)
        bodyStart = raw.find("\n\n");
    if (bodyStart == std::string::npos)
        return QByteArray(raw.c_str(), raw.size());

    std::string body = raw.substr(bodyStart + 4);

    try {
        auto encoder = vmime::utility::encoder::encoderFactory::getInstance()->getEncoderByName("base64")->create();
        vmime::utility::inputStreamStringAdapter in(body);
        std::ostringstream decoded;
        vmime::utility::outputStreamAdapter outDecoded(decoded);
        encoder->decode(in, outDecoded);
        const std::string& s = decoded.str();
        return QByteArray(s.c_str(), s.size());
    } catch (...) {
        return QByteArray(body.c_str(), body.size());
    }
}


QString RepositoryEmail::resolveAttachmentFilename(const std::string& raw, const BodyStructure& bs)
{
    QString qraw = QString::fromStdString(raw);

    QRegularExpression reDisp(R"(filename[*]?=["']?([^"'\r\n;]+)["']?)", QRegularExpression::CaseInsensitiveOption);
    auto match = reDisp.match(qraw);
    if (match.hasMatch())
        return match.captured(1).trimmed();

    QRegularExpression reName(R"(name=["']?([^"'\r\n;]+)["']?)", QRegularExpression::CaseInsensitiveOption);
    match = reName.match(qraw);
    if (match.hasMatch())
        return match.captured(1).trimmed();

    return QString("attachment_%1_%2").arg(QString::fromStdString(bs.type)).arg(QString::fromStdString(bs.subtype));
}


QString RepositoryEmail::extractCid(const std::string& raw)
{
    QRegularExpression re(R"(Content-ID:\s*<([^>]+)>)", QRegularExpression::CaseInsensitiveOption);
    auto match = re.match(QString::fromStdString(raw));
    if (match.hasMatch())
        return match.captured(1).trimmed();
    return QString();
}


QString RepositoryEmail::resolveCidReferences(const QString& html, const QMap<QString, InlineResource>& resources)
{
    QString result = html;
    for (auto it = resources.begin(); it != resources.end(); ++it)
    {
        QString dataUri = QString("data:%1;base64,%2").arg(it->mimeType).arg(QString(it->data.toBase64()));
        result.replace("cid:" + it.key(), dataUri);
    }
    return result;
}


bool RepositoryEmail::isDirectChildOf(const BodyStructure& child, const BodyStructure& parent)
{
    if (child.path.size() != parent.path.size() + 1)
        return false;
    return std::equal(parent.path.begin(), parent.path.end(), child.path.begin());
}


void RepositoryEmail::analyzeMultiPartAlternative(vmime::shared_ptr<vmime::net::message> msg)
{
    BodyStructure root = _parts.front();

    for (auto& p : _parts)
    {
        if (!isDirectChildOf(p, root))
            continue;

        if (p.type == vmime::mediaTypes::MULTIPART && p.subtype == vmime::mediaTypes::MULTIPART_MIXED)
        {
            analyzeMultiPartMixed(msg);
            return;
        }

        if (p.subtype == vmime::mediaTypes::TEXT_HTML)
        {
            try {
                std::string raw = extractRawPart(msg, p);
                // qDebug() << "RAW \n\n" << raw;
                QString html = decodePartContent(raw, detectEncoding(raw));
                qDebug() << "HTML \n\n" << html;
                emit htmlReady(html);
            } catch(vmime::exception& e){
                std::cerr << e.what();
            }
            return;
        }
    }
}


void RepositoryEmail::analyzeMultiPartMixed(vmime::shared_ptr<vmime::net::message> msg)
{
    QString htmlContent;
    QString plainContent;
    QList<Attachment> attachments;

    for (auto& p : _parts)
    {
        if (p.type == vmime::mediaTypes::MULTIPART)
            continue;

        if (p.subtype == vmime::mediaTypes::TEXT_HTML)
        {
            std::string raw = extractRawPart(msg, p);

            qDebug() << "RAW: " << raw << "\n\n ======== = = = = = ========\n\n";

            htmlContent = decodePartContent(raw, detectEncoding(raw));
            qDebug() << "HTML: " << htmlContent;
            continue;
        }

        if (p.subtype == vmime::mediaTypes::TEXT_PLAIN)
        {
            std::string raw = extractRawPart(msg, p);
            plainContent = decodePartContent(raw, detectEncoding(raw));
            continue;
        }

        Attachment att;
        att.mimeType = QString::fromStdString(p.type) + "/" + QString::fromStdString(p.subtype);
        att.data = extractAttachment(msg, p);
        std::string raw = extractRawPart(msg, p);
        att.filename = resolveAttachmentFilename(raw, p);
        attachments.append(att);

        qDebug() << "Attachment:" << att.filename << "| type:" << att.mimeType << "| size:" << att.data.size() << "B";
    }

    if (!htmlContent.isEmpty())
        emit htmlReady(htmlContent);
    else if (!plainContent.isEmpty())
        emit htmlReady("<pre>" + plainContent + "</pre>");

    if (!attachments.isEmpty())
        emit attachmentsReady(attachments);
}


void RepositoryEmail::analyzeMultiPartRelated(vmime::shared_ptr<vmime::net::message> msg)
{
    BodyStructure root = _parts.front();
    QMap<QString, InlineResource> inlineResources;
    QString htmlContent;
    bool firstChild = true;

    for (auto& p : _parts)
    {
        if (!isDirectChildOf(p, root))
            continue;

        if (firstChild)
        {
            std::string raw = extractRawPart(msg, p);
            htmlContent = decodePartContent(raw, detectEncoding(raw));
            firstChild = false;
            continue;
        }

        std::string raw = extractRawPart(msg, p);
        QString cid = extractCid(raw);
        qDebug() << "CID: " << cid;

        if (!cid.isEmpty())
        {
            InlineResource res;
            res.mimeType = QString::fromStdString(p.type) + "/" + QString::fromStdString(p.subtype);
            res.data = extractAttachment(msg, p);

            qDebug() << res.mimeType;

            inlineResources[cid] = res;
        }
    }

    if (!htmlContent.isEmpty() && !inlineResources.isEmpty())
        htmlContent = resolveCidReferences(htmlContent, inlineResources);

    if (!htmlContent.isEmpty())
        emit htmlReady(htmlContent);
}


void RepositoryEmail::analyzeSinglePart(vmime::shared_ptr<vmime::net::message> msg, BodyStructure& bs)
{
    std::string raw = extractRawPart(msg, bs);
    QString qraw = QString::fromStdString(raw);
    size_t bodyStart = std::string::npos;
    size_t searchPos = 0;

    while (searchPos < raw.size())
    {
        size_t found = raw.find("\r\n\r\n", searchPos);
        if (found == std::string::npos) break;

        size_t afterSep = found + 4;
        if (afterSep < raw.size() && raw[afterSep] != ' ' && raw[afterSep] != '\t')
        {
            bodyStart = found;
            break;
        }
        searchPos = found + 1;
    }

    if (bodyStart == std::string::npos)
    {
        emit htmlReady(QString::fromUtf8(raw.c_str()));
        return;
    }

    std::string body = raw.substr(bodyStart + 4);

    std::string encoding = "7bit";
    QRegularExpression reEnc(R"(Content-Transfer-Encoding:\s*(\S+))", QRegularExpression::CaseInsensitiveOption);
    auto match = reEnc.match(qraw);
    if (match.hasMatch())
        encoding = match.captured(1).trimmed().toLower().toStdString();

    QString content;

    if (encoding == "quoted-printable")
    {
        try {
            auto encoder = vmime::utility::encoder::encoderFactory::getInstance()->getEncoderByName("quoted-printable")->create();
            vmime::utility::inputStreamStringAdapter in(body);
            std::ostringstream decoded;
            vmime::utility::outputStreamAdapter outDecoded(decoded);
            encoder->decode(in, outDecoded);
            content = QString::fromUtf8(decoded.str().c_str());
        } catch (...) {
            content = QString::fromUtf8(body.c_str());
        }
    }
    else if (encoding == "base64")
    {
        try {
            auto encoder = vmime::utility::encoder::encoderFactory::getInstance()->getEncoderByName("base64")->create();
            vmime::utility::inputStreamStringAdapter in(body);
            std::ostringstream decoded;
            vmime::utility::outputStreamAdapter outDecoded(decoded);
            encoder->decode(in, outDecoded);
            content = QString::fromUtf8(decoded.str().c_str());
        } catch (...) {
            content = QString::fromUtf8(body.c_str());
        }
    }
    else
    {
        content = QString::fromUtf8(body.c_str());
    }

    if (bs.subtype == vmime::mediaTypes::TEXT_HTML)
        emit htmlReady(content);
    else if (bs.subtype == vmime::mediaTypes::TEXT_PLAIN)
        emit htmlReady("<pre>" + content + "</pre>");
}

std::string RepositoryEmail::detectEncoding(const std::string& raw)
{
    QRegularExpression reEnc(R"(Content-Transfer-Encoding:\s*(\S+))", QRegularExpression::CaseInsensitiveOption);
    auto match = reEnc.match(QString::fromStdString(raw));
    if (match.hasMatch())
        return match.captured(1).trimmed().toLower().toStdString();
    return "7bit";
}
