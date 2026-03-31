#ifndef EMAILMODEL_H
#define EMAILMODEL_H

#include <QAbstractListModel>
#include "../Model/email.h"

class EmailModel : public QAbstractListModel
{
    Q_OBJECT

    std::vector<Email> _emails;

public:
    explicit EmailModel(QObject *parent = nullptr);

    enum Roles {
        SubjectRole = Qt::UserRole + 1,
        FromRole,
        DateRole,
        UidRole,
        BodyRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex&, int) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setEmails(const std::vector<Email> &);
};

#endif
