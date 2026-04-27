#pragma once
#ifndef MAILBOXESMODEL_H
#define MAILBOXESMODEL_H

#include <QAbstractListModel>
#include "Infrastructure/imapclient.h"


class MailboxModel : public QAbstractListModel
{
    Q_OBJECT

    QList<MailBox> _mailboxes;

public:
    explicit MailboxModel(QObject *parent = nullptr);

    enum Roles {
        NameRole = Qt::UserRole + 1,
        ElementRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex&, int) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setMailBoxes(QList<MailBox>);

};

#endif // MAILBOXESMODEL_H
