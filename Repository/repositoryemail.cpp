#include "repositoryemail.h"
#include <qregularexpression.h>
#include <qtconcurrentrun.h>

RepositoryEmail::RepositoryEmail(ImapClient *client, QObject *parent)
    : QObject(parent)
    , _client(client)
{}

