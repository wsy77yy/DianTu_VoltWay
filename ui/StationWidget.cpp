#include "StationWidget.h"


#include "../manager/StationManager.h"
#include "../dialog/StationAddDialog.h"

#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QGroupBox>

#include <QMessageBox>



StationWidget::StationWidget(
    QWidget *parent
    )
    :
    QWidget(parent)
{

    setWindowTitle(
        "充电站管理"
        );


    resize(
        1000,
        650
        );



    /*
     * ======================
     * 顶部统计区域
     * ======================
     */


    totalStationLabel =
        new QLabel(
            "电站总数:0"
            );


    normalStationLabel =
        new QLabel(
            "正常运行:0"
            );


    pauseStationLabel =
        new QLabel(
            "暂停使用:0"
            );


    totalPileLabel =
        new QLabel(
            "电桩数量:0"
            );


    faultPileLabel =
        new QLabel(
            "故障电桩:0"
            );



    QHBoxLayout *statisticsLayout =
        new QHBoxLayout();



    statisticsLayout->addWidget(
        totalStationLabel
        );


    statisticsLayout->addWidget(
        normalStationLabel
        );


    statisticsLayout->addWidget(
        pauseStationLabel
        );


    statisticsLayout->addWidget(
        totalPileLabel
        );


    statisticsLayout->addWidget(
        faultPileLabel
        );



    /*
     * ======================
     * 操作按钮
     * ======================
     */



    backButton =
        new QPushButton(
            "返回首页"
            );



    addButton =
        new QPushButton(
            "新增电站"
            );



    QHBoxLayout *buttonLayout =
        new QHBoxLayout();



    buttonLayout->addWidget(
        backButton
        );


    buttonLayout->addWidget(
        addButton
        );


    buttonLayout->addStretch();



    /*
     * ======================
     * 表格
     * ======================
     */


    table =
        new QTableWidget(
            this
            );



    table->setColumnCount(
        8
        );



    table->setHorizontalHeaderLabels(
        {
            "编号",
            "电站名称",
            "地址",
            "状态",
            "电桩总数",
            "可用电桩",
            "故障电桩",
            "操作"
        }
        );



    table->horizontalHeader()
        ->setStretchLastSection(
            true
            );



    table->setEditTriggers(
        QAbstractItemView::NoEditTriggers
        );



    /*
     * ======================
     * 主布局
     * ======================
     */



    QVBoxLayout *mainLayout =
        new QVBoxLayout(
            this
            );



    mainLayout->addLayout(
        statisticsLayout
        );


    mainLayout->addLayout(
        buttonLayout
        );
    qDebug() << "refreshTable table:" << static_cast<void *>(table);


    mainLayout->addWidget(
        table
        );



    /*
     * ======================
     * 信号
     * ======================
     */



    connect(
        backButton,
        &QPushButton::clicked,
        this,
        &StationWidget::backHome
        );



    connect(
        addButton,
        &QPushButton::clicked,
        this,
        &StationWidget::addStation
        );



    /*
     * 最后加载数据
     *
     * 注意：
     * table已经创建完成
     * 不会再出现522行崩溃
     */


    refreshTable();


    updateStatistics();

}

void StationWidget::refreshTable()
{


    table->clearContents();



    QList<ManagedStation> stations =
        StationManager::instance()
            .getStations();



    table->setRowCount(
        stations.size()
        );



    for(int row=0;
         row<stations.size();
         row++)
    {


        ManagedStation station =
            stations[row];



        /*
         * 编号
         */

        table->setItem(
            row,
            0,
            new QTableWidgetItem(
                station.code
                )
            );



        /*
         * 名称
         */

        table->setItem(
            row,
            1,
            new QTableWidgetItem(
                station.name
                )
            );



        /*
         * 地址
         */

        table->setItem(
            row,
            2,
            new QTableWidgetItem(
                station.address
                )
            );



        /*
         * 状态
         */

        table->setItem(
            row,
            3,
            new QTableWidgetItem(
                station.status
                )
            );



        /*
         * 总电桩
         */

        table->setItem(
            row,
            4,
            new QTableWidgetItem(
                QString::number(
                    station.total
                    )
                )
            );



        /*
         * 可用电桩
         */

        table->setItem(
            row,
            5,
            new QTableWidgetItem(
                QString::number(
                    station.available
                    )
                )
            );



        /*
         * 故障
         */

        table->setItem(
            row,
            6,
            new QTableWidgetItem(
                QString::number(
                    station.fault
                    )
                )
            );



        /*
         * 操作区域
         */

        QWidget *operationWidget =
            new QWidget();



        QPushButton *statusButton =
            new QPushButton();



        QPushButton *deleteButton =
            new QPushButton(
                "删除"
                );



        if(station.status=="启用")
        {
            statusButton->setText("暂停");
        }
        else if (station.status == "暂停")
        {
            statusButton->setText("启用");
        }
        else
        {
            statusButton->setText("不可用");
            statusButton->setEnabled(false);
        }



        QHBoxLayout *layout =
            new QHBoxLayout(
                operationWidget
                );



        layout->addWidget(
            statusButton
            );


        layout->addWidget(
            deleteButton
            );



        layout->setContentsMargins(
            0,
            0,
            0,
            0
            );



        operationWidget->setLayout(
            layout
            );



        table->setCellWidget(
            row,
            7,
            operationWidget
            );



        /*
         * 启用/暂停
         */


        connect(
            statusButton,
            &QPushButton::clicked,
            this,
            [=]()
            {

                toggleStation(row);

            });



        /*
         * 删除
         */


        connect(
            deleteButton,
            &QPushButton::clicked,
            this,
            [=]()
            {


                QMessageBox::StandardButton result;


                result =
                    QMessageBox::question(
                        this,
                        "删除电站",
                        "确定删除该电站吗?"
                        );



                if(result ==
                    QMessageBox::Yes)
                {


                    StationManager::instance()
                    .removeStation(
                        row
                        );



                    refreshTable();


                    updateStatistics();

                    // 通知 Dashboard
                    emit stationDataChanged();

                }


            });



    }


}




void StationWidget::toggleStation(
    int row
    )
{


    QList<ManagedStation> stations =
        StationManager::instance()
            .getStations();



    if(row<0 ||
        row>=stations.size())
    {
        return;
    }



    /*
     * 状态切换
     *
     * 统一：
     *
     * 启用
     * 暂停
     *
     */


    if(stations[row].status=="启用")
    {

        stations[row].status=
            "暂停";

    }
    else
    {

        stations[row].status=
            "启用";

    }



    /*
     * 不再使用：
     *
     * clearStations()
     *
     * 改为更新指定数据
     */


    StationManager::instance()
        .updateStation(
            row,
            stations[row]
            );



    refreshTable();

    updateStatistics();

    // 通知 Dashboard
    emit stationDataChanged();
}





void StationWidget::updateStatistics()
{


    QList<ManagedStation> stations =
        StationManager::instance()
            .getStations();



    int totalStation =
        stations.size();



    int normal=0;


    int pause=0;


    int piles=0;


    int fault=0;



    for(const auto &station:std::as_const(stations))
    {


        if(station.status=="正常")
        {
            normal++;
        }
        else
        {
            pause++;
        }



        piles += station.total;


        fault += station.fault;


    }



    totalStationLabel->setText(
        QString(
            "电站总数:%1"
            )
            .arg(
                totalStation
                )
        );



    normalStationLabel->setText(
        QString(
            "正常运行:%1"
            )
            .arg(
                normal
                )
        );



    pauseStationLabel->setText(
        QString(
            "暂停使用:%1"
            )
            .arg(
                pause
                )
        );



    totalPileLabel->setText(
        QString(
            "电桩数量:%1"
            )
            .arg(
                piles
                )
        );



    faultPileLabel->setText(
        QString(
            "故障电桩:%1"
            )
            .arg(
                fault
                )
        );

}

void StationWidget::addStation()
{


    StationAddDialog dialog(
        this
        );



    if(dialog.exec()
        != QDialog::Accepted)
    {
        return;
    }



    /*
     * 从Dialog获取数据
     */


    ManagedStation station;



    station.code =
        dialog.getCode();



    station.name =
        dialog.getName();



    station.address =
        dialog.getAddress();



    station.status =
        "启用";



    station.total =
        dialog.getTotalPiles();



    station.available =
        station.total;



    station.fault =
        0;



    StationManager::instance()
        .addStation(
            station
            );


    refreshTable();

    updateStatistics();

    // 通知 Dashboard
    emit stationDataChanged();

}


