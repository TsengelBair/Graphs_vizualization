#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "vertex.h"
#include "graphalgos.h"

#include <QDebug>
#include <QScreen>
#include <QContextMenuEvent>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentVertexNum(0)
    , radius(30.0)
    , firstSelectedVertex(nullptr)
{
    ui->setupUi(this);

    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);

    /* Получаем размер экрана */
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    /*
       Устанавливаем фиксированные границы сцены
       в противном случае при добавлении вершин происходит перемещение всех вершин
    */
    scene->setSceneRect(0, 0, screenWidth, screenHeight);

    connect(ui->findPathBtn, &QPushButton::clicked, this, &MainWindow::handleFindPath);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setFirstVertex(Vertex *vertex)
{
    firstSelectedVertex = vertex;
}

Vertex *MainWindow::getFirstVertex()
{
    return firstSelectedVertex;
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

void MainWindow::slotHandleVertexClick(Vertex *clickedVertex)
{
    if (!firstSelectedVertex){
        setFirstVertex(clickedVertex);
        clickedVertex->setPen(QPen(Qt::red, 3));
    } else {
        showDialog(clickedVertex);
    }
}

void MainWindow::showDialog(Vertex *secondSelectedVertex)
{
    // Если выбрана вторая вершина, создаем диалоговое окно
    secondSelectedVertex->setPen(QPen(Qt::red, 3));

    QDialog* edgeDialog = new QDialog(this);

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

    connect(edgeDialog, &QDialog::rejected, [&]() {
        // Сбрасываем выделение, если пользователь отменяет ввод
        firstSelectedVertex->setPen(QPen(Qt::black, 2));
        secondSelectedVertex->setPen(QPen(Qt::black, 2));
        setFirstVertex(nullptr);
    });

    // Добавление ребра при нажатии кнопки
    connect(createEdgeBtn, &QPushButton::clicked ,this, [=]() {
        QString weightText = le->text();
        if (weightText.isEmpty()) {
            qDebug() << "Вес ребра не введен!";
            return;
        }

        bool ok;
        int weight = weightText.toInt(&ok);
        if (!ok) {
            qDebug() << "Ошибка: введите число!";
            return;
        }

        // Координаты двух вершин
        QPointF p1 = firstSelectedVertex->sceneBoundingRect().center();
        QPointF p2 = secondSelectedVertex->sceneBoundingRect().center();

        // Создаем ребро в виде линии
        QGraphicsLineItem* edge = scene->addLine(QLineF(p1, p2), QPen(Qt::blue, 2));

        // добавляем текст с весом ребра
        QGraphicsTextItem* edgeWeight = scene->addText(weightText);
        edgeWeight->setDefaultTextColor(Qt::blue);
        edgeWeight->setPos((p1 + p2) / 2); // Размещаем текст по центру ребра

        qDebug() << "Ребро добавлено между вершинами:"
                 << vertices.indexOf(firstSelectedVertex) << "и"
                 << vertices.indexOf(secondSelectedVertex)
                 << "с весом" << weight;

        /* дважды, т.к. граф неориентированный */
        graph[vertices.indexOf(firstSelectedVertex)][vertices.indexOf(secondSelectedVertex)] = weight;
        graph[vertices.indexOf(secondSelectedVertex)][vertices.indexOf(firstSelectedVertex)] = weight;

        // Сбрасываем выделение вершин
        firstSelectedVertex->setPen(QPen(Qt::black, 2));
        secondSelectedVertex->setPen(QPen(Qt::black, 2));
        firstSelectedVertex = nullptr;

        edgeDialog->accept();
    });

    connect(cancelBtn, &QPushButton::clicked, edgeDialog, &QDialog::reject);
    edgeDialog->exec();
}

void MainWindow::slotAddVertex()
{
    QPointF mousePos = ui->graphicsView->mapToScene(QCursor::pos());
    Vertex* vertex = new Vertex(mousePos.x(), mousePos.y(), radius, currentVertexNum);

    scene->addItem(vertex);

    vertices.push_back(vertex);
    ++currentVertexNum;

    /* при добавлении вершины ресайзим граф и размер каждой вершины */
    graph.resize(vertices.size());
    for (int i = 0; i < graph.size(); ++i){
        graph[i].resize(graph.size());
    }

    qDebug() << graph.size();

    connect(vertex, &Vertex::signalVertexClicked, this, &MainWindow::slotHandleVertexClick);
}

void MainWindow::handleFindPath()
{
    /*
        Если ребро между вершинами отсутствует, значение graph[i][j] == 0
        graph[i][j] = 10000 это идентификация отсутствия ребра
        элементы главной диагонали всегда нули (граф не петлевой)
    */
    for (int i = 0; i < graph.size(); ++i){
        for (int j = 0; j < graph.size(); ++j){
            if (i != j && graph[i][j] == 0){
                graph[i][j] = 10000;
            }
        }
    }
    QDialog* dialog = new QDialog(this);

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    QLabel* lbFirst = new QLabel("Стартовая вершина:");
    QLineEdit* leStart = new QLineEdit();
    QLabel* lbEnd = new QLabel("Конечная вершина:");
    QLineEdit* leEnd = new QLineEdit();
    QPushButton* findPath = new QPushButton("Найти");

    layout->addWidget(lbFirst);
    layout->addWidget(leStart);
    layout->addWidget(lbEnd);
    layout->addWidget(leEnd);
    layout->addWidget(findPath);
    dialog->setLayout(layout);

    connect(findPath, &QPushButton::clicked, this, [=](){
        int start = leStart->text().toInt();
        int end = leEnd->text().toInt();
        auto res = GraphAlgos::djkstra(graph, start, end);
        for (int i = 0; i < res.size(); ++i){
            qDebug() << res[i] << " ";
        }
    });

    dialog->exec();
}


