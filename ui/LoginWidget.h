#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);

signals:

    // 登录成功，并把实际登录账号传给 MainWindow
    void loginSuccess(const QString &account);

private slots:

    void onLoginClicked();

private:

    QLineEdit *accountEdit;
    QLineEdit *passwordEdit;

    QPushButton *loginButton;
    QPushButton *registerButton;

    QLabel *tipLabel;
};

#endif
