#include "LoginWidget.h"
#include "RegisterWidget.h"

#include "../service/AuthService.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFrame>
#include <QFont>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("VoltWay 管理后台登录");

    setFixedSize(460, 560);

    /*
     * =========================================================
     * 整体背景
     * =========================================================
     */
    setStyleSheet(R"(
        QWidget {
            background-color: #f8fbff;
            color: #3f424b;
            font-family: "Microsoft YaHei",
                         "PingFang SC",
                         "Segoe UI",
                         Arial,
                         sans-serif;
            font-size: 14px;
        }

        QLabel {
            color: #3f424b;
        }

        QLineEdit {
            background-color: #ffffff;
            border: 1px solid #e2eaf3;
            border-radius: 8px;
            padding: 0 14px;
            min-height: 46px;
            color: #3f424b;
            selection-background-color: #f17796;
        }

        QLineEdit:focus {
            border: 1px solid #f17796;
            background-color: #ffffff;
        }

        QLineEdit::placeholder {
            color: #a2a8b2;
        }

        QPushButton {
            border-radius: 8px;
            min-height: 44px;
            font-size: 14px;
            font-weight: 600;
        }

        QPushButton:hover {
            padding-top: 1px;
        }

        QPushButton:pressed {
            padding-top: 2px;
        }
    )");


    /*
     * =========================================================
     * 页面主布局
     * =========================================================
     */
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(
        32,
        28,
        32,
        28
    );

    mainLayout->setSpacing(0);


    /*
     * =========================================================
     * 顶部品牌
     * =========================================================
     */
    QLabel *logoLabel =
        new QLabel("VoltWay");

    logoLabel->setAlignment(Qt::AlignCenter);

    logoLabel->setStyleSheet(R"(
        QLabel {
            color: #d94f79;
            font-size: 30px;
            font-weight: 700;
            letter-spacing: 1px;
            background: transparent;
        }
    )");


    QLabel *titleLabel =
        new QLabel("充电运营管理后台");

    titleLabel->setAlignment(Qt::AlignCenter);

    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #3f424b;
            font-size: 21px;
            font-weight: 700;
            background: transparent;
            margin-top: 4px;
        }
    )");


    QLabel *subtitleLabel =
        new QLabel("管理员登录 / Administrator Login");

    subtitleLabel->setAlignment(Qt::AlignCenter);

    subtitleLabel->setStyleSheet(R"(
        QLabel {
            color: #777d88;
            font-size: 13px;
            background: transparent;
            margin-top: 6px;
            margin-bottom: 24px;
        }
    )");


    mainLayout->addWidget(logoLabel);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);


    /*
     * =========================================================
     * 登录卡片
     * =========================================================
     */
    QFrame *loginCard =
        new QFrame(this);

    loginCard->setObjectName("loginCard");

    loginCard->setStyleSheet(R"(
        QFrame#loginCard {
            background-color: #ffffff;
            border: 1px solid #e2eaf3;
            border-radius: 12px;
        }
    )");


    QVBoxLayout *cardLayout =
        new QVBoxLayout(loginCard);

    cardLayout->setContentsMargins(
        28,
        28,
        28,
        28
    );

    cardLayout->setSpacing(12);


    /*
     * =========================================================
     * 账号
     * =========================================================
     */
    QLabel *accountLabel =
        new QLabel("管理员账号");

    accountLabel->setStyleSheet(R"(
        QLabel {
            color: #3f424b;
            font-size: 13px;
            font-weight: 600;
            background: transparent;
        }
    )");


    accountEdit =
        new QLineEdit(loginCard);

    accountEdit->setPlaceholderText(
        "请输入管理员账号"
    );


    /*
     * =========================================================
     * 密码
     * =========================================================
     */
    QLabel *passwordLabel =
        new QLabel("登录密码");

    passwordLabel->setStyleSheet(R"(
        QLabel {
            color: #3f424b;
            font-size: 13px;
            font-weight: 600;
            background: transparent;
        }
    )");


    passwordEdit =
        new QLineEdit(loginCard);

    passwordEdit->setPlaceholderText(
        "请输入密码"
    );

    passwordEdit->setEchoMode(
        QLineEdit::Password
    );


    /*
     * =========================================================
     * 登录按钮
     * =========================================================
     */
    loginButton =
        new QPushButton(
            "登录 / Sign In",
            loginCard
        );

    loginButton->setCursor(
        Qt::PointingHandCursor
    );

    loginButton->setStyleSheet(R"(
        QPushButton {
            background-color: #f17796;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            min-height: 46px;
            font-size: 15px;
            font-weight: 700;
        }

        QPushButton:hover {
            background-color: #d94f79;
        }

        QPushButton:pressed {
            background-color: #c8426c;
        }
    )");


    /*
     * =========================================================
     * 注册按钮
     * =========================================================
     */
    registerButton =
        new QPushButton(
            "注册管理员账号",
            loginCard
        );

    registerButton->setCursor(
        Qt::PointingHandCursor
    );

    registerButton->setStyleSheet(R"(
        QPushButton {
            background-color: #ffffff;
            color: #2f83c5;
            border: 1px solid #cfe4f5;
            border-radius: 8px;
            min-height: 42px;
            font-size: 13px;
            font-weight: 600;
        }

        QPushButton:hover {
            background-color: #e8f5ff;
            border-color: #58a6df;
        }

        QPushButton:pressed {
            background-color: #dff0fc;
        }
    )");


    /*
     * =========================================================
     * 提示文字
     * =========================================================
     */
    tipLabel =
        new QLabel(loginCard);

    tipLabel->setAlignment(
        Qt::AlignCenter
    );

    tipLabel->setWordWrap(true);

    tipLabel->setStyleSheet(R"(
        QLabel {
            color: #777d88;
            font-size: 12px;
            background: transparent;
            padding-top: 4px;
        }
    )");

    tipLabel->setText(
        "请输入管理员账号和密码登录系统"
    );


    cardLayout->addWidget(accountLabel);
    cardLayout->addWidget(accountEdit);

    cardLayout->addSpacing(4);

    cardLayout->addWidget(passwordLabel);
    cardLayout->addWidget(passwordEdit);

    cardLayout->addSpacing(12);

    cardLayout->addWidget(loginButton);

    cardLayout->addSpacing(4);

    cardLayout->addWidget(registerButton);

    cardLayout->addSpacing(4);

    cardLayout->addWidget(tipLabel);


    mainLayout->addWidget(loginCard);


    /*
     * =========================================================
     * 底部版权/说明
     * =========================================================
     */
    QLabel *footerLabel =
        new QLabel(
            "VoltWay · 充电运营管理平台"
        );

    footerLabel->setAlignment(
        Qt::AlignCenter
    );

    footerLabel->setStyleSheet(R"(
        QLabel {
            color: #a2a8b2;
            font-size: 11px;
            background: transparent;
            margin-top: 18px;
        }
    )");

    mainLayout->addWidget(
        footerLabel
    );


    /*
     * =========================================================
     * 登录按钮
     * =========================================================
     */
    connect(
        loginButton,
        &QPushButton::clicked,
        this,
        &LoginWidget::onLoginClicked
    );


    /*
     * =========================================================
     * 回车登录
     * =========================================================
     */
    connect(
        passwordEdit,
        &QLineEdit::returnPressed,
        this,
        &LoginWidget::onLoginClicked
    );


    /*
     * =========================================================
     * 注册
     * =========================================================
     */
    connect(
        registerButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            RegisterWidget *w =
                new RegisterWidget();

            w->setAttribute(
                Qt::WA_DeleteOnClose
            );

            w->show();
        }
    );
}


/*
 * =============================================================
 * 登录
 * =============================================================
 */
void LoginWidget::onLoginClicked()
{
    QString account =
        accountEdit->text().trimmed();

    QString password =
        passwordEdit->text();


    /*
     * 空账号
     */
    if (account.isEmpty())
    {
        tipLabel->setText(
            "请输入管理员账号"
        );

        tipLabel->setStyleSheet(R"(
            QLabel {
                color: #c84658;
                font-size: 12px;
                background: transparent;
                padding-top: 4px;
            }
        )");

        accountEdit->setFocus();

        return;
    }


    /*
     * 空密码
     */
    if (password.isEmpty())
    {
        tipLabel->setText(
            "请输入登录密码"
        );

        tipLabel->setStyleSheet(R"(
            QLabel {
                color: #c84658;
                font-size: 12px;
                background: transparent;
                padding-top: 4px;
            }
        )");

        passwordEdit->setFocus();

        return;
    }


    /*
     * =========================================================
     * AuthService 验证
     * =========================================================
     */
    bool result =
        AuthService::instance()
        .login(
            account,
            password
        );


    if (result)
    {
        /*
         * 登录成功
         */
        QMessageBox msgBox(this);

        msgBox.setWindowTitle(
            "登录成功"
        );

        msgBox.setIcon(
            QMessageBox::Information
        );

        msgBox.setText(
            "欢迎进入 VoltWay 管理后台"
        );

        msgBox.setInformativeText(
            "当前登录账号：" + account
        );

        msgBox.setStandardButtons(
            QMessageBox::Ok
        );

        msgBox.setStyleSheet(R"(
            QMessageBox {
                background-color: #ffffff;
                color: #3f424b;
                font-family: "Microsoft YaHei";
            }

            QLabel {
                color: #3f424b;
                font-size: 14px;
                min-width: 280px;
            }

            QPushButton {
                background-color: #f17796;
                color: #ffffff;
                border: none;
                border-radius: 7px;
                min-width: 80px;
                min-height: 34px;
                padding: 4px 16px;
                font-weight: 600;
            }

            QPushButton:hover {
                background-color: #d94f79;
            }
        )");

        msgBox.exec();


        /*
         * =====================================================
         * 把真实账号传给 MainWindow
         * =====================================================
         */
        emit loginSuccess(account);

        close();
    }
    else
    {
        /*
         * 登录失败
         */
        QMessageBox msgBox(this);

        msgBox.setWindowTitle(
            "登录失败"
        );

        msgBox.setIcon(
            QMessageBox::Warning
        );

        msgBox.setText(
            "账号或密码错误"
        );

        msgBox.setInformativeText(
            "请检查管理员账号和密码后重新输入。"
        );

        msgBox.setStandardButtons(
            QMessageBox::Ok
        );

        msgBox.setStyleSheet(R"(
            QMessageBox {
                background-color: #ffffff;
                color: #3f424b;
                font-family: "Microsoft YaHei";
            }

            QLabel {
                color: #3f424b;
                font-size: 14px;
                min-width: 280px;
            }

            QPushButton {
                background-color: #ffffff;
                color: #c84658;
                border: 1px solid #f0c7cf;
                border-radius: 7px;
                min-width: 80px;
                min-height: 34px;
                padding: 4px 16px;
                font-weight: 600;
            }

            QPushButton:hover {
                background-color: #ffe6eb;
                border-color: #c84658;
            }
        )");

        msgBox.exec();


        tipLabel->setText(
            "账号或密码错误，请重新输入"
        );

        tipLabel->setStyleSheet(R"(
            QLabel {
                color: #c84658;
                font-size: 12px;
                background: transparent;
                padding-top: 4px;
            }
        )");

        passwordEdit->clear();
        passwordEdit->setFocus();
    }
}
