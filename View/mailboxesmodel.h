
#ifndef MAILBOXESMODEL_H
#define MAILBOXESMODEL_H

#include <QAbstractListModel>
#include "../Model/email.h"

struct MailBox{

    QString name;
    int elements;
};

class MailboxModel : public QAbstractListModel
{
    Q_OBJECT

    std::vector<MailBox> _mailboxes;

public:
    explicit MailboxModel(QObject *parent = nullptr);

    enum Roles {
        NameRole = Qt::UserRole + 1,
        ElementRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex&, int) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setMailBoxes(std::vector<MailBox>);

};

#endif // MAILBOXESMODEL_H
