#include "StationAddDialog.h"


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>



StationAddDialog::StationAddDialog(QWidget *parent)
    :
    QDialog(parent)
{

    setWindowTitle(
        "新增充电站"
    );


    resize(
        350,
        300
    );



    QVBoxLayout *layout =
            new QVBoxLayout(this);



    codeEdit =
            new QLineEdit;


    nameEdit =
            new QLineEdit;


    addressEdit =
            new QLineEdit;


    pileSpin =
            new QSpinBox;


    pileSpin->setRange(
        1,
        500
    );



    layout->addWidget(
        new QLabel("电站编号")
    );

    layout->addWidget(
        codeEdit
    );



    layout->addWidget(
        new QLabel("电站名称")
    );

    layout->addWidget(
        nameEdit
    );



    layout->addWidget(
        new QLabel("地址")
    );

    layout->addWidget(
        addressEdit
    );



    layout->addWidget(
        new QLabel("电桩数量")
    );

    layout->addWidget(
        pileSpin
    );



    QPushButton *okBtn =
            new QPushButton(
                "确定"
            );


    QPushButton *cancelBtn =
            new QPushButton(
                "取消"
            );



    QHBoxLayout *btnLayout =
            new QHBoxLayout;


    btnLayout->addWidget(okBtn);

    btnLayout->addWidget(cancelBtn);



    layout->addLayout(
        btnLayout
    );



    connect(
        okBtn,
        &QPushButton::clicked,
        this,
        &QDialog::accept
    );


    connect(
        cancelBtn,
        &QPushButton::clicked,
        this,
        &QDialog::reject
    );


}



QString StationAddDialog::getCode()const
{

    return codeEdit->text();

}


QString StationAddDialog::getName() const
{

    return nameEdit->text();

}


QString StationAddDialog::getAddress() const
{

    return addressEdit->text();

}


int StationAddDialog::getTotalPiles() const
{

    return pileSpin->value();

}