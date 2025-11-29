#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QDebug"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit_mailto->setEnabled(false);
    ui->plainTextEdit_subject->setEnabled(false);
    ui->textEdit_body->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


//INTEGRATION WITH UI


void MainWindow::on_pushButton_newMessage_clicked()
{
    ui->lineEdit_mailto->setEnabled(true);
    ui->plainTextEdit_subject->setEnabled(true);
    ui->textEdit_body->setEnabled(true);

    Message message;
    MainWindow::setMessage(message);
}

//ACCESSORS

void MainWindow::setMessage(Message &message) { MainWindow::_messages.push_back(message); }


//DEBUG FUNCS

void MainWindow::on_pushButton_send_clicked()
{
    QString recipients = ui->lineEdit_mailto->text();
    QString subject = ui->plainTextEdit_subject->toPlainText();
    QString body = ui->textEdit_body->toPlainText();

    if(!recipients.isEmpty()) MainWindow::_messages.back().setRecipients(recipients);
    if(!subject.isEmpty()) MainWindow::_messages.back().setSubject(subject);
    if(!body.isEmpty()) MainWindow::_messages.back().setBody(body);

    qDebug() << "Message created:" << _messages.back().getRecipients() << ", " << _messages.back().getSubject() << ", " << _messages.back().getBody();
}
