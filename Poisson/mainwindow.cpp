#include "mainwindow.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    view = new Q3DSurface;
    view->activeTheme()->setType(Q3DTheme::ThemeIsabelle);
    view->activeTheme()->setLabelBorderEnabled(false);
    view->setShadowQuality(Q3DSurface::ShadowQualityNone);
    view->activeTheme()->setLabelBackgroundEnabled(true);
    QWidget *container = QWidget::createWindowContainer(view);
    QSize screenSize = view->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.3));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    QWidget *controlsWidget = new QWidget();
    controlsWidget->setMaximumWidth(200);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlsWidget);

    xn = new QLineEdit(this);
    xn->setText("3");
    QLabel* label1 = new QLabel("Разбиений по x:", controlsWidget);
    yn = new QLineEdit(this);
    yn->setText("3");
    QLabel* label2 = new QLabel("Разбиений по y:", controlsWidget);
    eps = new QLineEdit(this);
    eps->setText("0.001");
    QLabel* label3 = new QLabel("Требуемая точность:", controlsWidget);
    maxsteps = new QLineEdit(this);
    maxsteps->setText("10000");
    QLabel* label4 = new QLabel("Макс. число шагов:", controlsWidget);

    QPushButton *graphButton = new QPushButton("График", this);
    QPushButton *tableButton = new QPushButton("Таблица", this);

    connect(graphButton, &QPushButton::clicked, this, &MainWindow::showGraph);
    connect(tableButton, &QPushButton::clicked, this, &MainWindow::showTable);


    slider = new QSlider(Qt::Horizontal);
    slider->setMinimum(0);
    slider->setMaximum(10);
    valueLabel = new QLabel("Шаг: 0");

    connect(slider, &QSlider::valueChanged, this, [=](int value) {
        valueLabel->setText("Шаг: " + QString::number(value));
        setT(value);
    });
    QLabel* label6 = new QLabel(" \n ", controlsWidget);
    controlLayout->addWidget(label1);
    controlLayout->addWidget(xn);
    controlLayout->addWidget(label2);
    controlLayout->addWidget(yn);
    controlLayout->addWidget(label3);
    controlLayout->addWidget(eps);
    controlLayout->addWidget(label4);
    controlLayout->addWidget(maxsteps);
    controlLayout->addWidget(graphButton);
    controlLayout->addWidget(tableButton);
    controlLayout->addWidget(valueLabel);
    controlLayout->addWidget(slider);
    controlLayout->addWidget(label6);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->addWidget(controlsWidget);
    mainLayout->addWidget(container);

    view->axisX()->setTitle(QStringLiteral("X"));
    view->axisY()->setTitle(QStringLiteral("Y"));
    view->axisZ()->setTitle(QStringLiteral("U"));
    view->axisX()->setTitleVisible(true);
    view->axisY()->setTitleVisible(true);
    view->axisZ()->setTitleVisible(true);
    slv = solver();
    a = 0.0;
    b = 1.0;
    c = 0.0;
    d = 1.0;
};

MainWindow::~MainWindow()
{
}




void MainWindow::showGraph() {
    int Xn = xn->text().toInt();
    int Yn = yn->text().toInt();
    int maxN = maxsteps->text().toInt();
    double epsilon = eps->text().toDouble();

    if (Xn != slv.N || Yn != slv.M) {
        slv.solve(Xn, Yn, a, b, c, d, epsilon, maxN, v, z);
    }
    else return;

    dataSeries = new QSurface3DSeries;
    dataSeries->setItemLabelFormat(QStringLiteral("(@xLabel @yLabel @zLabel)"));
    //dataSeries->setMesh(QAbstract3DSeries::MeshSphere);
    dataSeries->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);

    QSurfaceDataArray* data = new QSurfaceDataArray;
    for (int j = 0; j <= slv.M; j++) {
        QSurfaceDataRow* row = new QSurfaceDataRow;
        for (int i = 0; i <= slv.N; i++) {
            *row << QVector3D(slv.h * i, v[9][j][i], slv.k * j);
        }
        *data << row;
    }
    //dataSeries->setName(QString("Сфера r = " + QString::number(radius)));
    dataSeries->dataProxy()->resetArray(data);
    dataSeries->setItemLabelFormat("solution @xLabel @yLabel @zLabel");
    dataSeries->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    view->addSeries(dataSeries);

    dataSeries = new QSurface3DSeries;
    dataSeries->setItemLabelFormat(QStringLiteral("(@xLabel @yLabel @zLabel)"));
    //dataSeries->setMesh(QAbstract3DSeries::MeshSphere);
    dataSeries->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);

    data = new QSurfaceDataArray;
    for (int j = 0; j <= slv.M; j++) {
        QSurfaceDataRow* row = new QSurfaceDataRow;
        for (int i = 0; i <= slv.N; i++) {
            *row << QVector3D(slv.h * i, u_real::u(slv.h * i, slv.k * j), slv.k * j);
        }
        *data << row;
    }
    //dataSeries->setName(QString("Сфера r = " + QString::number(radius)));
    dataSeries->dataProxy()->resetArray(data);
    dataSeries->setItemLabelFormat("solution @xLabel @yLabel @zLabel");
    dataSeries->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    view->addSeries(dataSeries);

    view->axisX()->setRange(a, b);
    view->axisY()->setRange(0, 5);
    view->axisZ()->setRange(c, d);
}

void MainWindow::setT(int t){
    int Xn = xn->text().toInt();
    int Yn = yn->text().toInt();
    view->removeSeries(dataSeries);
    QSurfaceDataRow* row;
    QSurfaceDataArray* data;
    dataSeries = new QSurface3DSeries;
    data = new QSurfaceDataArray;

    dataSeries->setItemLabelFormat(QStringLiteral("(@xLabel @yLabel @zLabel)"));
    //dataSeries->setMesh(QAbstract3DSeries::MeshSphere);
    dataSeries->setDrawMode(QSurface3DSeries::DrawSurface);
    dataSeries->setFlatShadingEnabled(false);


    QSurfaceDataItem item;
    for (int i = 0; i <= slv.N; i++) {
        row = new QSurfaceDataRow;
        for (int j = 0; j <= slv.M; j++) {
            double x = slv.h * i;
            double y = slv.k * j;
            double z = v[t][i][j];
            item.setPosition(QVector3D(x, y, z));
            row->append(item);
        }
        data->append(row);
    }
    //dataSeries->setName(QString("Сфера r = " + QString::number(radius)));
    dataSeries->dataProxy()->resetArray(data);
    dataSeries->setItemLabelFormat("solution @xLabel @yLabel @zLabel");
    dataSeries->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    view->addSeries(dataSeries);

    dataSeries = new QSurface3DSeries;
    data = new QSurfaceDataArray;

    dataSeries->setItemLabelFormat(QStringLiteral("(@xLabel @yLabel @zLabel)"));
    //dataSeries->setMesh(QAbstract3DSeries::MeshSphere);
    dataSeries->setDrawMode(QSurface3DSeries::DrawSurface);
    dataSeries->setFlatShadingEnabled(false);


    for (int i = 0; i <= slv.N; i++) {
        row = new QSurfaceDataRow;
        for (int j = 0; j <= slv.M; j++) {
            double x = slv.h * i;
            double y = slv.k * j;
            double z = u_real::u(x, y);
            item.setPosition(QVector3D(x, y, z));
            row->append(item);
        }
        data->append(row);
    }
    //dataSeries->setName(QString("Сфера r = " + QString::number(radius)));
    dataSeries->dataProxy()->resetArray(data);
    dataSeries->setItemLabelFormat("solution @xLabel @yLabel @zLabel");
    dataSeries->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    view->addSeries(dataSeries);


    view->axisX()->setRange(a, b);
    view->axisY()->setRange(c, d);
    view->axisZ()->setRange(0, 5);
}

void MainWindow::showTable() {
    QTableWidget* tableWidget = new QTableWidget(4, 4);
    for(int j = 0; j < 4; j++){
        
        tableWidget->setHorizontalHeaderLabels({"X", "Y", "Z"});
        for (int i = 0; i < 4; ++i) {
            tableWidget->setItem(i, j, new QTableWidgetItem(QString::number(v[9][j][i])));
        }
        
    }
    tableWidget->setMinimumSize(400, 400);
    tableWidget->setWindowTitle("Таблица");
    tableWidget->show();
}
