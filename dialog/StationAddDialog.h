#ifndef STATIONADDDIALOG_H
#define STATIONADDDIALOG_H


#include <QDialog>


class QLineEdit;
class QSpinBox;


class StationAddDialog : public QDialog
{

    Q_OBJECT


public:

    explicit StationAddDialog(QWidget *parent=nullptr);



    QString getCode() const;

    QString getName() const;

    QString getAddress() const;

    int getTotalPiles() const;



private:


    QLineEdit *codeEdit;

    QLineEdit *nameEdit;

    QLineEdit *addressEdit;

    QSpinBox *pileSpin;



};


#endif