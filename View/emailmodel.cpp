#include "emailmodel.h"
#include <qtpreprocessorsupport.h>

EmailModel::EmailModel(QObject *parent) : QAbstractListModel(parent) {}

int EmailModel::rowCount(const QModelIndex &parent) const {

    Q_UNUSED(parent);
    return _emails.size();
}

QVariant EmailModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() >= _emails.size()) return QVariant();

    const Email &email = _emails.at(index.row());

    switch(role){
        case SubjectRole: return email.getSubject();
        case FromRole: return email.getFrom();
        case DateRole: return email.getDate();
        case UidRole: return email.getUid();
        case isReadRole: return email.getIsRead();
        default: return QVariant();
    }
}

QHash<int, QByteArray> EmailModel::roleNames() const {
    return {
        {SubjectRole, "subject"},
        {FromRole, "from"},
        {DateRole, "date"},
        {UidRole, "uid"},
        {isReadRole, "isRead"}
    };
}

void EmailModel::setEmails(const std::vector<Email> &emails)
{
    beginResetModel();
    _emails = emails;
    endResetModel();
}

void EmailModel::setEmail(const Email& email)
{
    for(int i = 0; i < _emails.size(); i++)
    {
        if(_emails[i].getUid() == email.getUid())
        {
            _emails[i] = email;
            QModelIndex index = createIndex(i, 0);
            emit dataChanged(index, index);
            return;
        }
    }
}
