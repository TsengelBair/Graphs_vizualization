#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QScreen>
#include <QMessageBox>

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

void MainWindow::handleCickOnVertex(Vertex *vertex)
{
    static Vertex* firstVertex = nullptr;
    if (!firstVertex){
        firstVertex = vertex;
        vertex->setPen(QPen(Qt::red, 3));
    } else {
        vertex->setPen(QPen(Qt::red, 3));
        // Логика для диалога
        QMessageBox::information(this, "Вершины выбраны",
                                 QString("Выбраны вершины %1 и %2")
                                 .arg(vertices.indexOf(firstVertex))
                                 .arg(vertices.indexOf(vertex)));

        firstVertex->setPen(QPen(Qt::black, 2));
        vertex->setPen(QPen(Qt::black, 2));
        firstVertex = nullptr;
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
