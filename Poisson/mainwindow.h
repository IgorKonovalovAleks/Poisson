#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QtDataVisualization>
#include <QStyleFactory>
#include <iostream>
#include <vector>
#include <cmath>
#include "solver.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Q3DSurface *view;
    QLineEdit *xn;
    QLineEdit *yn;
    QLineEdit *eps;
    QLineEdit *maxsteps;
    QSlider *slider;
    QLabel *valueLabel;
    std::vector<QSurface3DSeries> iterations;
    QSurface3DSeries *dataSeries;
    solver slv;
    std::vector<std::vector<std::vector<double>>> v;
    std::vector<std::vector<std::vector<double>>> z;
    double a, b, c, d;

    MainWindow(QWidget* parent = nullptr);

    ~MainWindow();

    bool activePlot{false};

    void showGraph();

    void showTable();
    void setT(int t);
};
