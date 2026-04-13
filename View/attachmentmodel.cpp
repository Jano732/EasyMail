#include "attachmentmodel.h"

AttachmentModel::AttachmentModel(QObject* parent)
    : QAbstractListModel(parent)
{}

void AttachmentModel::setAttachments(const QList<RepositoryEmail::Attachment>& list)
{
    beginResetModel();
    _attachments = list;
    endResetModel();
}

int AttachmentModel::rowCount(const QModelIndex&) const
{
    return _attachments.size();
}

QVariant AttachmentModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= _attachments.size())
        return {};

    const auto& att = _attachments[index.row()];

    if (role == FilenameRole) return att.filename;
    if (role == MimeTypeRole) return att.mimeType;
    if (role == DataRole)     return QString("data:%1;base64,%2")
            .arg(att.mimeType)
            .arg(QString(att.data.toBase64()));
    return {};
}

QHash<int, QByteArray> AttachmentModel::roleNames() const
{
    return {
        { FilenameRole, "filename" },
        { MimeTypeRole, "mimeType" },
        { DataRole,     "data"     }
    };
}

RepositoryEmail::Attachment AttachmentModel::getAttachment(int index) const
{
    if (index >= 0 && index < _attachments.size())
        return _attachments[index];
    return {};
}
