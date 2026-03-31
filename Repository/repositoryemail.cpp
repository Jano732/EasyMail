#include "repositoryemail.h"
#include <qregularexpression.h>
#include <qtconcurrentrun.h>

RepositoryEmail::RepositoryEmail(ImapClient *client, QObject *parent)
    : QObject(parent)
    , _client(client)
{}

void RepositoryEmail::envelopeEmailsSlot()
{
    QString envelope_request = _client->nextTag() + " UID FETCH " + QString::number(_client->getUid() - 51) + ":" + QString::number(_client->getUid()) + " (ENVELOPE)\r\n";
    QString response = _client->sendRequest(envelope_request);
    std::vector<Email> recieved_emails = envelopeDataParser(response);
    emit emailsReadySignal(recieved_emails);
}


void RepositoryEmail::fetchBodySlot(Email email)
{
    _bodyTokens.clear();
    _tokenizedBodystructure.clear();
    _tokenizedBodystructureMultipart.clear();
    _download_list.clear();
    _download_list_atachments.clear();

    QString fetch_bodystructure_request = _client->nextTag() + " UID FETCH " + email.getUid() + " (BODYSTRUCTURE)\r\n";
    QString response = _client->sendRequest(fetch_bodystructure_request);
    qDebug() << response;
    processBodyResponse(response);
    resolveBody();

    ResolvedBody result;

    for(auto& section : _download_list)
    {
        auto it = std::find_if(_tokenizedBodystructure.begin(), _tokenizedBodystructure.end(),
                               [&section](const Bodystructure bs)
                               {return section == bs.section;}
                               );

        if(it == _tokenizedBodystructure.end()) continue;

        QString fetchReq = _client->nextTag() + " UID FETCH " + email.getUid() + " (BODY[" + section + "])\r\n";
        // qDebug() << section;
        QString rawResponse = _client->sendRequest(fetchReq);

        if(it->type.toLower() == "image")
        {
            QString cid = it->content_id;
            cid.remove('<').remove('>');
            if(!cid.isEmpty())
                result.inline_images[cid] = extractRawBytes(rawResponse);
            continue;
        }

        QString decoded = decodeBodyPart(rawResponse, it->encoding);

        if(it->subtype.toLower() == "html" && result.main_content.isEmpty())
        {
            result.main_content = decoded;
            // qDebug() << result.main_content;
            result.content_type = ResolvedBody::ContentType::HTML;
        }
        else if(it->subtype.toLower() == "plain" && result.main_content.isEmpty())
        {
            result.main_content = decoded;
            result.content_type = ResolvedBody::ContentType::PLAINTEXT;
        }
    }

    for(auto& section : _download_list_atachments)
    {
        auto it = std::find_if(_tokenizedBodystructure.begin(), _tokenizedBodystructure.end(),
                               [&section](const Bodystructure bs)
                               {return bs.section == section;}
                               );

        if(it == _tokenizedBodystructure.end()) continue;

        QString fetchReq = _client->nextTag() + " UID FETCH " + email.getUid() + " (BODY[" + section + "])\r\n";
        QString rawResponse = _client->sendRequest(fetchReq);

        ResolvedBody::Attachment att;
        att.mimeType = it->type.toLower() + '/' + it->subtype.toLower();
        att.data = extractRawBytes(rawResponse);

        for(int i = 0; i + 1 < it->params.size(); i += 2){
            if(it->params[i].toLower() == "name"){
                att.filename = it->params[i + 1];
                break;
            }
        }

        if(att.filename.isEmpty())
        {
            for(int i = 0; i + 1 < it->dispozition.size(); i++)
            {
                if(it->dispozition[i].toLower() == "filename"){
                    att.filename = it->dispozition[i+1];
                    break;
                }
            }
        }
        if(att.filename.isEmpty())
            att.filename = "attachment_" + section;

        result.attachments.append(att);
    }

    if(result.content_type == ResolvedBody::ContentType::HTML)
    {
        for (auto it = result.inline_images.begin(); it != result.inline_images.end();)
        {
            QString dataUri = "data:image/png;base64," +
                              QString::fromLatin1(it.value().toBase64());
            result.main_content.replace("cid:" + it.key(), dataUri);
        }
    }
    emit bodyReadySignal(result);
}

QByteArray RepositoryEmail::extractRawBytes(const QString& response)
{
    int dataStart = response.indexOf('{');
    if (dataStart == -1) return {};
    int dataEnd = response.indexOf('}', dataStart);
    int size = response.mid(dataStart + 1, dataEnd - dataStart - 1).toInt();
    int contentStart = response.indexOf('\n', dataEnd) + 1;
    QString raw = response.mid(contentStart, size);
    raw.remove('\r').remove('\n');
    return QByteArray::fromBase64(raw.toUtf8());
}

QString RepositoryEmail::bodyResolveBestPart(QString parent_section, QString subtype)
{
    if(subtype.toLower() == "alternative")
    {
        Bodystructure* best = nullptr;
        for(auto& bs : _tokenizedBodystructure)
        {
            if(bs.section.startsWith(parent_section + '.'))
            {
                if(bs.subtype.toLower() == "html") return bs.section;
                if(bs.subtype.toLower() == "plain") best = &bs;
            }
        }
        return best ? best->section : "";
    }
    else if(subtype.toLower() == "mixed"){

        for(auto& bs: _tokenizedBodystructure)
        {
            if(bs.section.startsWith(parent_section + '.'))
            {
                return bs.section;
            }
        }
    }
    return "";
}

void RepositoryEmail::bodyProcessNode(QString section, BodystructureMultipart& multipart)
{
    if(multipart.subtype.toLower() == "alternative")
    {
        QString bestSection = bodyResolveBestPart(section, "alternative");
        if(!bestSection.isEmpty())
        {
            _download_list.push_back(bestSection);
        }
    }
    else if(multipart.subtype.toLower() == "mixed")
    {
        for(auto& bs : _tokenizedBodystructure)
        {
            if(!bs.section.startsWith(section.isEmpty() ? "" : section + ".")) continue;
            QString relative = section.isEmpty() ? bs.section : bs.section.mid(section.length() + 1);
            // qDebug() << relative;
            if(relative.contains('.')) continue;

            bool isAttachment = !bs.dispozition.isEmpty() && bs.dispozition.first().toLower() == "attachment";
            if(isAttachment) _download_list_atachments.push_back(bs.section);
            else _download_list.push_back(bs.section);
        }
    }
    else if(multipart.subtype.toLower() == "related")
    {
        bool first = true;
        for(auto& bs : _tokenizedBodystructure)
        {
            if(!bs.section.startsWith(section.isEmpty() ? "" : section + "."))
                continue;
            QString relative = section.isEmpty() ? bs.section : bs.section.mid(section.length() + 1);
            // qDebug() << relative;

            _download_list.push_back(bs.section);
            first = false;
        }
    }
}

void RepositoryEmail::resolveBody()
{
    _download_list.clear();

    auto sorted = _tokenizedBodystructureMultipart;
    std::sort(sorted.begin(), sorted.end(), [](const BodystructureMultipart& a, const BodystructureMultipart& b){
        return a.section.length() > b.section.length();
    });

    for(auto& bsm : sorted)
    {
        bodyProcessNode(bsm.section, bsm);
        // qDebug() << bsm.section;
    }

    if(_download_list.empty() && !_tokenizedBodystructure.empty()) _download_list.push_back(_tokenizedBodystructure.front().section);
}


QString RepositoryEmail::decodeBodyPart(const QString& response, const QString& encoding)
{
    QString content;

    int dataStart = response.indexOf('{');
    if(dataStart != -1)
    {
        // Istniejąca ścieżka: literal {n}\r\n...dane...
        int dataEnd = response.indexOf('}', dataStart);
        int size = response.mid(dataStart + 1, dataEnd - dataStart - 1).toInt();
        int contentStart = response.indexOf('\n', dataEnd) + 1;
        content = response.mid(contentStart, size);
    }
    else
    {
        // Singlepart bez literału — serwer zwrócił quoted string lub inline
        // Szukamy: BODY[...] "treść" lub BODY[...] treść\r\n
        QRegularExpression bodyRe(R"(BODY\[[^\]]*\]\s+\"((?:[^\"\\]|\\.)*)\")");
        QRegularExpressionMatch match = bodyRe.match(response);
        if(match.hasMatch())
        {
            content = match.captured(1);
        }
        else
        {
            // Ostatnia deska ratunku: weź wszystko po pierwszym \r\n\r\n (po nagłówku FETCH)
            int headerEnd = response.indexOf("\r\n\r\n");
            if(headerEnd != -1)
                content = response.mid(headerEnd + 4).trimmed();
            else
                content = response; // fallback
        }
    }

    // Dekodowanie — ta część bez zmian
    if(encoding.toLower() == "base64")
    {
        content.remove('\r');
        content.remove('\n');
        return QString::fromUtf8(QByteArray::fromBase64(content.toUtf8()));
    }
    else if(encoding.toLower() == "quoted-printable")
    {
        content.replace('_', ' ');
        QByteArray bytes = content.toLatin1();
        bytes.replace('%', "%25");
        bytes.replace('=', "%");
        content = QByteArray::fromPercentEncoding(bytes);
    }
    return content;
}

QString RepositoryEmail::extractNextTokenEnvelope(QString &data, int &pos)
{

    while (pos < data.length() && data[pos].isSpace()) pos++;

    if (pos >= data.length()) return "";

    if (data[pos] == '(' && data[pos + 1] == '('){

        pos += 2;
        int end = pos;
        do{
            end = data.indexOf("))", end + 1);
        } while(end > 0 && (data[end - 1] == ')' && data[end] == ')' && data[end + 1] == ')'));
        QString res = data.mid(pos, end - pos);
        pos = end + 1;
        return res;
    }

    if (data[pos] == '{') {
        int end = data.indexOf('}', pos);
        int size = data.mid(pos + 1, end - pos - 1).toInt();
        pos = data.indexOf('\n', end) + 1;
        QString res = data.mid(pos, size);
        pos += size;
        return res;
    }

    if (data[pos] == '"') {
        int end = pos;
        do {
            end = data.indexOf('"', end + 1);
        } while (end > 0 && data[end-1] == '\\');

        QString res = data.mid(pos + 1, end - pos - 1);
        pos = end + 1;
        return res;
    }

    int nextSpace = data.indexOf(' ', pos);
    int nextBracket = data.indexOf(')', pos);
    int end = (nextSpace < nextBracket && nextSpace != -1) ? nextSpace : nextBracket;

    QString res = data.mid(pos, end - pos);
    pos = end;
    return (res == "NIL") ? "" : res;
}

Email RepositoryEmail::processEnvelopeResponse(QString input)
{
    int pos = 0;

    extractNextTokenEnvelope(input, pos);
    extractNextTokenEnvelope(input, pos);
    extractNextTokenEnvelope(input, pos);
    extractNextTokenEnvelope(input, pos);
    QString uid = extractNextTokenEnvelope(input, pos);

    if (input.mid(pos).contains("ENVELOPE", Qt::CaseInsensitive)) {
        pos = input.indexOf("ENVELOPE", pos) + 8;

        while (pos < input.length() && input[pos] != '(') pos++;
        pos++;

        QString date =      extractNextTokenEnvelope(input, pos);
        QString subject =   extractNextTokenEnvelope(input, pos);
        QString from =      extractNextTokenEnvelope(input, pos);
        from = processList(from);

        pos += 2; // Wejście z listy from do listy sender

        QString sender = extractNextTokenEnvelope(input, pos);
        sender = processList(sender);

        pos += 2; // Wejście z listy sender do listy reply to

        QString reply_to = extractNextTokenEnvelope(input, pos);
        reply_to = processList(reply_to);

        pos += 2; // Wejście z listy reply_tp do listy to

        QString to = extractNextTokenEnvelope(input, pos);
        to = processList(to);

        pos += 2;  // Wejście z listy to do listy cc

        QString cc = extractNextTokenEnvelope(input, pos);
        cc = processList(cc);

        pos += 1;

        QString bcc = extractNextTokenEnvelope(input, pos);
        bcc = processList(bcc);

        pos += 1;

        QString in_reply_to = extractNextTokenEnvelope(input, pos);
        in_reply_to = processList(in_reply_to);

        pos += 1;

        QString message_id = extractNextTokenEnvelope(input, pos);

        // qDebug() << "UID:" << uid;
        // qDebug() << "Data:" << decode(date);
        // qDebug() << "Temat:" << decode(subject);
        // qDebug() << "Od:" << decode(from);
        // qDebug() << "Sender:" << decode(sender);
        // qDebug() << "Reply To:" << decode(reply_to);
        // qDebug() << "To:" << decode(to);
        // qDebug() << "Cc:" << decode(cc);
        // qDebug() << "Bcc:" << decode(bcc);
        // qDebug() << "In Reply To:" << decode(in_reply_to);
        // qDebug() << "Message ID:" << message_id;
        // qDebug() << "=======================";

        Email email(uid, decode(date), decode(subject), decode(from), decode(sender), decode(reply_to), decode(to), decode(cc), decode(bcc), decode(in_reply_to), message_id);

        return email;
    }
    return Email();
}

QString RepositoryEmail::processList(QString &input)
{
    if(input != ""){

        QStringList formattedAddresses;
        QRegularExpression re("(?:\"([^\"]*)\"|NIL)\\s+(?:\"([^\"]*)\"|NIL)\\s+\"([^\"]*)\"\\s+\"([^\"]*)\"");

        QRegularExpressionMatchIterator it = re.globalMatch(input);

        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();

            QString displayName = match.captured(1);
            QString user = match.captured(3);
            QString domain = match.captured(4);

            if (displayName.isEmpty() || displayName == "NIL") {
                formattedAddresses << QString("%1@%2").arg(user, domain);
            } else {
                //Name <email@domain.pl>
                formattedAddresses << QString("%1 <%2@%3>").arg(displayName, user, domain);
            }
        }
        return formattedAddresses.join(", ");
    }
    else return "";
}

QString RepositoryEmail::decode(QString &input)
{
    QString output;
    int lastPos = 0;
    QRegularExpression re(R"(=\?([^?]+)\?([qQbB])\?([^?]+)\?=(?:\s+(?==\?))?)");
    QRegularExpressionMatchIterator i = re.globalMatch(input);

    while(i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        output += input.mid(lastPos, match.capturedStart() - lastPos);

        QString charset = match.captured(1); // utf-8
        QString type = match.captured(2).toUpper(); // Q || B
        QString encoded = match.captured(3);

        if(type == 'B') output += QByteArray::fromBase64(encoded.toUtf8());
        else if (type == 'Q')
        {
            // encoded.replace('=','%');
            encoded.replace('_',' ');
            QByteArray bytes = encoded.toLatin1();
            bytes.replace('%', "%25");
            bytes.replace('=', "%");
            output += QByteArray::fromPercentEncoding(bytes);
        }

        lastPos = match.capturedEnd();
    }

    output += input.mid(lastPos);
    return output.trimmed();
}

std::vector<Email> RepositoryEmail::envelopeDataParser(QString &input)
{
    std::vector<Email> emails;
    QRegularExpression fetchStart(R"(\* \d+ FETCH)");
    QRegularExpressionMatchIterator it = fetchStart.globalMatch(input);

    QList<int> positions;
    while (it.hasNext()) positions << it.next().capturedStart();
    positions << input.length();

    for (int i = 0; i < positions.size() - 1; ++i) {
        QString singleFetch = input.mid(positions[i], positions[i+1] - positions[i]);
        // qDebug() << i << "\n" << singleFetch << "\n=======================";
        emails.push_back(processEnvelopeResponse(singleFetch));
    }

    return emails;
}


void RepositoryEmail::processBodyResponse(QString& input)
{
    QString current_tag = _client->getTag();
    _bodyTokens.clear();
    bodyTokenize(input);
    int pos = 0;
    bodyParsePart(pos, "");
}

void RepositoryEmail::bodyParsePart(int& pos, QString section = "")
{
    // qDebug() << "POS: " << _bodyTokens.at(pos) << ", " << _bodyTokens.at(pos + 1);
    if(_bodyTokens.at(pos)._type == RepositoryEmail::TokenType::LPAREN)
    {
        if(_bodyTokens.at(pos+1)._type == RepositoryEmail::TokenType::LPAREN) bodyParseMultiPart(pos, section);
        else if(_bodyTokens.at(pos+1)._type == RepositoryEmail::TokenType::STRING) bodyParseSinglePart(pos, section);
    }
}


void RepositoryEmail::bodyParseSinglePart(int& pos, QString section)
{
    Bodystructure bs{};
    bs.section = section;
    bs.section = section.isEmpty() ? "1" : section;

    if(pos < _bodyTokens.size())
    {
        pos++;
            bs.type = _bodyTokens.at(pos)._value;
            pos++;
            bs.subtype = _bodyTokens.at(pos)._value;
            pos++;

            if(_bodyTokens.at(pos)._type == RepositoryEmail::TokenType::LPAREN)
            {
                pos++;
                do
                {
                    bs.params.append(_bodyTokens.at(pos)._value);
                    pos++;
                } while(_bodyTokens.at(pos)._type != RepositoryEmail::TokenType::RPAREN);
                pos++;
            }
            else
            {
                bs.params.append(_bodyTokens.at(pos)._value);
                pos++;
            }

            bs.content_id = _bodyTokens.at(pos)._value;
            pos++;
            bs.description = _bodyTokens.at(pos)._value;
            pos++;
            bs.encoding = _bodyTokens.at(pos)._value;
            pos++;
            bs.size = _bodyTokens.at(pos)._value;
            pos++;

            if(bs.type.toLower() == "text")
            {
                bs.lines = _bodyTokens.at(pos)._value;
                pos++;
            }

            bs.md5 = _bodyTokens.at(pos)._value;
            pos++;

            if(_bodyTokens.at(pos)._type == RepositoryEmail::TokenType::LPAREN)
            {
                int depth = 0;
                do
                {
                    if(_bodyTokens.at(pos)._type == RepositoryEmail::TokenType::LPAREN) depth++;
                    else if (_bodyTokens.at(pos)._type == RepositoryEmail::TokenType::RPAREN) depth--;
                    if(depth > 0 && _bodyTokens.at(pos)._type != RepositoryEmail::TokenType::LPAREN && _bodyTokens.at(pos)._type != RepositoryEmail::TokenType::RPAREN) bs.dispozition.append(_bodyTokens.at(pos)._value);
                    pos++;
                } while(depth > 0);
            }
            else
            {
                bs.dispozition.append(_bodyTokens.at(pos)._value);
                pos++;
            }

            bs.language = _bodyTokens.at(pos)._value;
            pos++;
            bs.location = _bodyTokens.at(pos)._value;
            pos += 2;

            // qDebug() << "LAST SIGN: " << _bodyTokens.at(pos);
            // qDebug() << bs;
            _tokenizedBodystructure.push_back(bs);
    }
}


void RepositoryEmail::bodyParseMultiPart(int& pos, QString section)
{
    pos++;
    int childIndex = 1;

    while(_bodyTokens.at(pos)._type == RepositoryEmail::TokenType::LPAREN)
    {
        QString childSection = section.isEmpty() ? QString::number(childIndex) : section + "." + QString::number(childIndex);
        bodyParsePart(pos, childSection);
        childIndex++;
    }

    if(pos < _bodyTokens.size() && _bodyTokens.at(pos)._type == RepositoryEmail::TokenType::STRING)
    {
        BodystructureMultipart bsm;
        bsm.section = section;
        bsm.subtype = _bodyTokens.at(pos)._value;
        pos++;
        // qDebug() << "bsm.section: " << bsm.section;
        // qDebug() << "bsm.subtype: " << bsm.subtype;
        pos += 2;
        bsm.boundary = _bodyTokens.at(pos)._value;
        // qDebug() << "bsm.boundary: " << bsm.boundary;
        // qDebug() << "=============";
        pos += 2;

        _tokenizedBodystructureMultipart.push_back(bsm);

        while(_bodyTokens.at(pos)._type == RepositoryEmail::TokenType::ATOM) pos++;


        // qDebug() << _bodyTokens.at(pos - 1)._value << ", " << _bodyTokens.at(pos)._value << ", " << _bodyTokens.at(pos + 1)._value;
    }


    if(pos < _bodyTokens.size() && _bodyTokens.at(pos)._type == RepositoryEmail::TokenType::RPAREN) pos++;

}

void RepositoryEmail::bodyTokenize(QString& data)
{
    int pos = 0;
    if(data.contains("BODYSTRUCTURE ("))
    {
        pos = data.indexOf("BODYSTRUCTURE (");
        pos += 14;
    }

    QString current_tag = _client->getTag();

    while((data[pos] != current_tag[0]) || (data[pos + 1] != current_tag[1]) || (data[pos+2] != current_tag[2]) || (data[pos+3] != current_tag[3]))
    {
        if(data[pos] == '(')
        {
            Token token(RepositoryEmail::TokenType::LPAREN, data[pos]);
            _bodyTokens.push_back(token);
            pos++;
        }
        else if(data[pos] == ')')
        {
            Token token(RepositoryEmail::TokenType::RPAREN, data[pos]);
            _bodyTokens.push_back(token);
            pos++;
        }
        else if(data[pos] == '"')
        {
            QString string;
            pos++;
            do{
                string += data[pos];
                pos++;
            } while(data[pos] != '"');

            pos++;;
            Token token(RepositoryEmail::TokenType::STRING, string);
            _bodyTokens.push_back(token);
        }
        else if(data[pos] == 'N' || data[pos].isDigit())
        {
            QString string;
            do{
                string += data[pos];
                pos++;
            } while(!data[pos].isSpace() && data[pos] != ')');

            Token token(RepositoryEmail::TokenType::ATOM, string);
            _bodyTokens.push_back(token);
        }
        else if(data[pos].isSpace()) pos++;
    }

    // qDebug() << "Liczba tokenów: " << _bodyTokens.size();

    // for(auto token : _bodyTokens)
    // {
    //     qDebug() << token << "\n";
    // }

    // qDebug() << "==========================";
}

void RepositoryEmail::skipList(int& pos) {
    int level = 0;
    qDebug() << _bodyTokens.at(pos)._value;
    do {
        if (_bodyTokens.at(pos)._type == RepositoryEmail::TokenType::LPAREN) level++;
        if (_bodyTokens.at(pos)._type == RepositoryEmail::TokenType::RPAREN) level--;
        pos++;
        qDebug() << "level: " << level;
    } while (level > 0 && pos < _bodyTokens.size());
}

