#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

private slots:

    void on_pushButton_newMessage_clicked();

    void on_pushButton_send_clicked();

private:
    Ui::MainWindow *ui;
    std::vector<Message> _messages;
};
#endif // MAINWINDOW_H
