#pragma once
#ifndef ATTACHMENTMODEL_H
#define ATTACHMENTMODEL_H

#include <QAbstractListModel>
#include "../Repository/repositoryemail.h"

class AttachmentModel : public QAbstractListModel
{
    Q_OBJECT

    QList<RepositoryEmail::Attachment> _attachments;

public:

    enum Roles { FilenameRole = Qt::UserRole + 1, MimeTypeRole, DataRole };

    explicit AttachmentModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setAttachments(const QList<RepositoryEmail::Attachment>& list);
    RepositoryEmail::Attachment getAttachment(int index) const;
};

#endif // ATTACHMENTMODEL_H
