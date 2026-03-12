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
        default: return QVariant();
    }
}

QHash<int, QByteArray> EmailModel::roleNames() const {
    return {
        {SubjectRole, "subject"},
        {FromRole, "from"},
        {DateRole, "date"},
        {UidRole, "uid"}
    };
}

void EmailModel::setEmails(const std::vector<Email> &emails)
{
    beginResetModel();
    _emails = emails;
    endResetModel();
}
