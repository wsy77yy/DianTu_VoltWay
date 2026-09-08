#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>

class QLineEdit;
class QPushButton;

class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget *parent = nullptr);

private slots:
    void registerClicked();

private:
    QLineEdit *accountEdit;
    QLineEdit *passwordEdit;
    QPushButton *submitButton;
};

#endif