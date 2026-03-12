#ifndef SERVICE_H
#define SERVICE_H

#include "../Model/email.h"
#include "../Infrastructure/imapclient.h"
#include "../Repository/repositoryemail.h"
#include "../View/emailmodel.h"

class Service : public QObject
{
    Q_OBJECT

    RepositoryEmail *_repo;
    EmailModel *_email_model;
    std::vector<Email> _emails;

public:

    explicit Service(RepositoryEmail*, EmailModel*,  QObject* parent = nullptr);

    void envelopeEmails();
    std::vector<Email> getEmails();
    EmailModel* getEmailModel();
    Q_INVOKABLE void requestBodyByUid(Email);
    Q_INVOKABLE Email getEmailByUid(QString);

signals:

    void requestEnvelopedEmails();
    void envelopedDataReady();
    void requestBody(Email);

public slots:

    void onEmailsFetched(const std::vector<Email>);

};

#endif // SERVICE_H
