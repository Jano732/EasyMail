#pragma once
#ifndef REPOSITORYEMAIL_H
#define REPOSITORYEMAIL_H

#include "../Infrastructure/imapclient.h"
#include "../Model/email.h"
#include <QObject>


class RepositoryEmail : public QObject
{
    Q_OBJECT

    ImapClient *_client;


public:

    RepositoryEmail(ImapClient*, QObject* parent = nullptr);




public slots:


signals:


};



#endif // REPOSITORYEMAIL_H
