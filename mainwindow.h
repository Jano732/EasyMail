#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "email.h"
#include "imapclient.h"
#include "message.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setMessage(Message &message);
    void initEmailList();

private slots:

    void on_pushButton_newMessage_clicked();

    void on_pushButton_send_clicked();

private:
    Ui::MainWindow *ui;
    ImapClient client{"imap.poczta.onet.pl", 993, "poniatowski@op.pl", "G320-R5K3-O1DX-4CWH"};
    // ImapClient client{"imap.gmail.com", 993, "jan.poniatowski2003@gmail.com", "pxtluultxeaixepe"};
    // std::vector<Message> _messages;
    std::vector<std::unique_ptr<Message>> _messages;
    std::vector<Email*> emails;

};
#endif // MAINWINDOW_H

