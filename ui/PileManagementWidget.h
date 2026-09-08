#ifndef PILEMANAGEMENTWIDGET_H
#define PILEMANAGEMENTWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QList>

class PileManagementWidget : public QWidget
{
public:
    explicit PileManagementWidget(QWidget *parent = nullptr);

private:
    void setupUi();
    void loadStations();
    void loadPiles();
    void refreshTable();

    void restartSelectedPile();
    void shutdownSelectedPile();
    void selectAllPiles();
    void clearAllSelections();

    QList<QString> selectedPileCodes() const;
    QString selectedPileCode() const;

private:
    QLineEdit *searchEdit = nullptr;
    QComboBox *stationCombo = nullptr;
    QComboBox *statusCombo = nullptr;

    QPushButton *searchButton = nullptr;
    QPushButton *resetButton = nullptr;
    QPushButton *restartButton = nullptr;
    QPushButton *shutdownButton = nullptr;

    QTableWidget *table = nullptr;
    QLabel *summaryLabel = nullptr;
};

#endif