#include "mailboxesmodel.h"

MailboxModel::MailboxModel(QObject *parent) : QAbstractListModel(parent) {}

int MailboxModel::rowCount(const QModelIndex &parent) const {

    Q_UNUSED(parent);
    return _mailboxes.size();
}

QVariant MailboxModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() >= _mailboxes.size()) return QVariant();

    const MailBox &mailbox = _mailboxes.at(index.row());

    switch(role){
    case NameRole: return mailbox.name;
    case ElementRole: return mailbox.elements;

    default: return QVariant();
    }
}

QHash<int, QByteArray> MailboxModel::roleNames() const {
    return {
        {NameRole, "name"},
        {ElementRole, "elements"},
    };
}

void MailboxModel::setMailBoxes(QList<MailBox> mailboxes)
{
    qDebug() << "wywołano setMailBoxes";
    if(!mailboxes.empty())
    {
        beginResetModel();
        _mailboxes = mailboxes;
        endResetModel();
    }
}
