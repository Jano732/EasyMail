/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QWidget *widget;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_newMessage;
    QPushButton *pushButton_send;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout;
    QListWidget *listWidget;
    QListWidget *listWidget_2;
    QVBoxLayout *verticalLayout;
    QLineEdit *lineEdit_mailto;
    QSpacerItem *verticalSpacer_2;
    QPlainTextEdit *plainTextEdit_subject;
    QSpacerItem *verticalSpacer;
    QTextEdit *textEdit_body;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1036, 571);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        widget = new QWidget(centralwidget);
        widget->setObjectName("widget");

        horizontalLayout_2->addWidget(widget);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButton_newMessage = new QPushButton(centralwidget);
        pushButton_newMessage->setObjectName("pushButton_newMessage");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton_newMessage->sizePolicy().hasHeightForWidth());
        pushButton_newMessage->setSizePolicy(sizePolicy);
        pushButton_newMessage->setMaximumSize(QSize(16777215, 40));

        horizontalLayout_2->addWidget(pushButton_newMessage);

        pushButton_send = new QPushButton(centralwidget);
        pushButton_send->setObjectName("pushButton_send");
        pushButton_send->setMaximumSize(QSize(16777215, 40));

        horizontalLayout_2->addWidget(pushButton_send);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        horizontalLayout->setContentsMargins(0, -1, -1, -1);
        listWidget = new QListWidget(centralwidget);
        listWidget->setObjectName("listWidget");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Minimum);
        sizePolicy1.setHorizontalStretch(30);
        sizePolicy1.setVerticalStretch(200);
        sizePolicy1.setHeightForWidth(listWidget->sizePolicy().hasHeightForWidth());
        listWidget->setSizePolicy(sizePolicy1);
        listWidget->setMinimumSize(QSize(200, 0));
        listWidget->setMaximumSize(QSize(1000, 16777215));

        horizontalLayout->addWidget(listWidget);

        listWidget_2 = new QListWidget(centralwidget);
        listWidget_2->setObjectName("listWidget_2");
        listWidget_2->setEnabled(true);
        sizePolicy1.setHeightForWidth(listWidget_2->sizePolicy().hasHeightForWidth());
        listWidget_2->setSizePolicy(sizePolicy1);
        listWidget_2->setMinimumSize(QSize(200, 0));
        listWidget_2->setMaximumSize(QSize(1000, 16777147));

        horizontalLayout->addWidget(listWidget_2);


        horizontalLayout_3->addLayout(horizontalLayout);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        lineEdit_mailto = new QLineEdit(centralwidget);
        lineEdit_mailto->setObjectName("lineEdit_mailto");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lineEdit_mailto->sizePolicy().hasHeightForWidth());
        lineEdit_mailto->setSizePolicy(sizePolicy2);
        lineEdit_mailto->setEchoMode(QLineEdit::EchoMode::NoEcho);

        verticalLayout->addWidget(lineEdit_mailto);

        verticalSpacer_2 = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Maximum);

        verticalLayout->addItem(verticalSpacer_2);

        plainTextEdit_subject = new QPlainTextEdit(centralwidget);
        plainTextEdit_subject->setObjectName("plainTextEdit_subject");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(200);
        sizePolicy3.setHeightForWidth(plainTextEdit_subject->sizePolicy().hasHeightForWidth());
        plainTextEdit_subject->setSizePolicy(sizePolicy3);
        plainTextEdit_subject->setMaximumSize(QSize(16777215, 50));
        plainTextEdit_subject->setStyleSheet(QString::fromUtf8("#plainTextEdit_topic{\n"
"background-color: #72CC72;\n"
"}"));

        verticalLayout->addWidget(plainTextEdit_subject);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Maximum);

        verticalLayout->addItem(verticalSpacer);

        textEdit_body = new QTextEdit(centralwidget);
        textEdit_body->setObjectName("textEdit_body");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(textEdit_body->sizePolicy().hasHeightForWidth());
        textEdit_body->setSizePolicy(sizePolicy4);
        textEdit_body->setMinimumSize(QSize(600, 400));

        verticalLayout->addWidget(textEdit_body);


        horizontalLayout_3->addLayout(verticalLayout);


        verticalLayout_2->addLayout(horizontalLayout_3);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1036, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton_newMessage->setText(QCoreApplication::translate("MainWindow", "New message", nullptr));
        pushButton_send->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
        lineEdit_mailto->setInputMask(QString());
        lineEdit_mailto->setText(QString());
        lineEdit_mailto->setPlaceholderText(QCoreApplication::translate("MainWindow", "To:", nullptr));
        plainTextEdit_subject->setPlaceholderText(QCoreApplication::translate("MainWindow", "Topic: ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
