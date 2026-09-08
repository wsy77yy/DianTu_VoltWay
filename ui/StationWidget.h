#ifndef STATIONWIDGET_H
#define STATIONWIDGET_H


#include <QWidget>



class QLabel;
class QPushButton;
class QTableWidget;





class StationWidget : public QWidget
{

    Q_OBJECT


public:

    explicit StationWidget(
        QWidget *parent = nullptr
        );



signals:

    void backHome();

    // 电站数据发生变化
    void stationDataChanged();

private slots:


    void addStation();

    void toggleStation(
        int row
        );



private:


    void refreshTable();


    void updateStatistics();



private:


    /*
     * 顶部统计
     */


    QLabel *totalStationLabel;

    QLabel *normalStationLabel;

    QLabel *pauseStationLabel;

    QLabel *totalPileLabel;

    QLabel *faultPileLabel;



    /*
     * 按钮
     */


    QPushButton *backButton;

    QPushButton *addButton;



    /*
     * 表格
     */


    QTableWidget *table;


};



#endif