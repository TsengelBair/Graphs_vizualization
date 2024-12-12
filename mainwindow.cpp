#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QScreen>
#include <QMessageBox> // убрать
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      curVertexNum(0)
{
    ui->setupUi(this);

    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);

    // Получаем размер экрана
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    // Устанавливаем фиксированные границы сцены, чтобы она не изменялась
    scene->setSceneRect(0, 0, screenWidth, screenHeight);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleCickOnVertex(Vertex* vertex)
{
    static Vertex* firstVertex = nullptr;

    if (!firstVertex) {
        // Если первая вершина не выбрана, запоминаем ее
        firstVertex = vertex;
        vertex->setPen(QPen(Qt::red, 3));
    } else {
        // Если выбрана вторая вершина, создаем диалоговое окно
        vertex->setPen(QPen(Qt::red, 3));

        QDialog* edgeDialog = new QDialog(this);
        edgeDialog->setWindowTitle("Добавить ребро");

        QVBoxLayout* layout = new QVBoxLayout(edgeDialog);
        QLabel* label = new QLabel("Введите вес ребра:");
        QLineEdit* le = new QLineEdit();
        QPushButton* createEdgeBtn = new QPushButton("Добавить ребро");
        QPushButton* cancelBtn = new QPushButton("Отмена");

        layout->addWidget(label);
        layout->addWidget(le);
        layout->addWidget(createEdgeBtn);
        layout->addWidget(cancelBtn);
        edgeDialog->setLayout(layout);

        connect(edgeDialog, &QDialog::rejected, [=]() {
            firstVertex->setPen(QPen(Qt::black, 2));
            vertex->setPen(QPen(Qt::black, 2));
            firstVertex = nullptr;
        });

        // Добавление ребра при нажатии кнопки
        connect(createEdgeBtn, &QPushButton::clicked, [=]() {
            qDebug() << "Введенное значение веса ребра:" << le->text();
            qDebug() << "Ребро добавлено между вершинами:"
                     << vertices.indexOf(firstVertex) << "и"
                     << vertices.indexOf(vertex);

            firstVertex->setPen(QPen(Qt::black, 2));
            vertex->setPen(QPen(Qt::black, 2));
            firstVertex = nullptr;

            edgeDialog->accept();
        });

        connect(cancelBtn, &QPushButton::clicked, edgeDialog, &QDialog::reject);
        edgeDialog->exec();
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction* addVertexAction = new QAction("Добавить вершину", this);
    QAction* cancelAction = new QAction("Отмена", this); // без коннекта, т.к. при нажатии автоматически закроется

    menu.addAction(addVertexAction);
    connect(addVertexAction, &QAction::triggered, this, &MainWindow::slotAddVertex);

    menu.addAction(cancelAction);

    // Отображаем меню в позиции курсора
    menu.exec(event->globalPos());

}

void MainWindow::slotAddVertex()
{
    QPointF mousePos = ui->graphicsView->mapToScene(QCursor::pos());

    qreal radius = 30.0;
    Vertex* vertex = new Vertex(mousePos.x(), mousePos.y(), radius, curVertexNum);

    scene->addItem(vertex);

    vertices.push_back(vertex);
    ++curVertexNum;

    connect(vertex, &Vertex::signalVertexClicked, this, &MainWindow::handleCickOnVertex);
}
