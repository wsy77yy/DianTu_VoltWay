#include "RegisterWidget.h"

#include "../service/AdminManageService.h"

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>



RegisterWidget::RegisterWidget(QWidget *parent)
    :
    QWidget(parent)
{


    setWindowTitle(
        "管理员注册申请"
    );


    resize(
        350,
        200
    );



    accountEdit =
        new QLineEdit(this);


    accountEdit->setPlaceholderText(
        "请输入申请账号"
    );



    passwordEdit =
        new QLineEdit(this);


    passwordEdit->setPlaceholderText(
        "请输入密码"
    );


    passwordEdit->setEchoMode(
        QLineEdit::Password
    );



    submitButton =
        new QPushButton(
            "提交申请",
            this
        );



    QVBoxLayout *layout =
        new QVBoxLayout(this);



    layout->addWidget(accountEdit);

    layout->addWidget(passwordEdit);

    layout->addWidget(submitButton);



    connect(
        submitButton,
        &QPushButton::clicked,
        this,
        &RegisterWidget::registerClicked
    );


}




void RegisterWidget::registerClicked()
{


    bool result =
        AdminManageService::instance()
        .registerAdmin(
            accountEdit->text(),
            passwordEdit->text(),
            "OperatorAdmin"
        );



    if(result)
    {

        QMessageBox::information(
            this,
            "申请成功",
            "等待超级管理员审批"
        );


        close();

    }


}