#pragma once
#include "Service/service.h"
#include "Model/email.h"
#include "Repository/repositoryemail.h"
#include "View/emailmodel.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include <qdiriterator.h>
#include <QtWebEngineQuick/QtWebEngineQuick>
#include <vmime/vmime.hpp>

int main(int argc, char *argv[])
{
    QtWebEngineQuick::initialize();
    QApplication a(argc, argv);

    auto *client = new ImapClient("imap.poczta.onet.pl", 993, "poniatowski@op.pl", "F5I3-O0YZ-SXQK-CPKU");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/EmailClient/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return a.exec();
}
