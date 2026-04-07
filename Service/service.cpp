#include "service.h"
#include <qfuture.h>
#include <qtconcurrentrun.h>

// Service::Service(QString url, int port, QString address, QString password)
//     : _client(url, port, address, password)
// {
//     _client.connect();
//     _client.login();
//     _client.selectInbox("INBOX");
//     _emails = _client.envelope();
// }

Service::Service(RepositoryEmail *repo, EmailModel* emailmodel, QObject* parent)
    : QObject(parent)
    , _repo(repo)
    , _email_model(emailmodel)
{};

