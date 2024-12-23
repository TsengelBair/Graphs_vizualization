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
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentVertexNum(0)
    , radius(30.0)
    , firstSelectedVertex(nullptr)
    , sceneCloneWidget(nullptr)
{
    ui->setupUi(this);

    ui->resetFindedPathBtn->setVisible(false);
    ui->viewOriginalGraphBtn->setVisible(false);

    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    /*
       Устанавливаем фиксированные границы сцены
       в противном случае при добавлении вершин происходит перемещение всех вершин
    */
    scene->setSceneRect(0, 0, QApplication::primaryScreen()->geometry().width(), QApplication::primaryScreen()->geometry().height());

    connect(ui->findPathBtn, &QPushButton::clicked, this, &MainWindow::handleFindPath);
    connect(ui->resetGraphBtn, &QPushButton::clicked, this, &MainWindow::resetGraph);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setFirstVertex(Vertex *vertex)
{
    firstSelectedVertex = vertex;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction* addVertexAction = new QAction("Добавить вершину", this);
    QAction* cancelAction = new QAction("Отмена", this); /* без коннекта, т.к. при нажатии автоматически закроется */

    menu.addAction(addVertexAction);
    connect(addVertexAction, &QAction::triggered, this, &MainWindow::slotAddVertex);

    menu.addAction(cancelAction);

    /* Отображаем меню в позиции курсора */
    menu.exec(event->globalPos());
}

void MainWindow::slotHandleVertexClick(Vertex *clickedVertex)
{
    if (!firstSelectedVertex){
        setFirstVertex(clickedVertex);
        clickedVertex->setPen(QPen(Qt::red, 3));
    } else if (firstSelectedVertex && firstSelectedVertex == clickedVertex){
        firstSelectedVertex->setPen(QPen(Qt::black, 2));
        setFirstVertex(nullptr);
    }  else {
        showDialog(clickedVertex);
    }
}

void MainWindow::showDialog(Vertex *secondSelectedVertex)
{
    /* Если выбрана вторая вершина, создаем диалоговое окно */
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
        /* Сбрасываем выделение, если пользователь отменяет ввод */
        firstSelectedVertex->setPen(QPen(Qt::black, 2));
        secondSelectedVertex->setPen(QPen(Qt::black, 2));
        setFirstVertex(nullptr);
    });

    /* Добавление ребра при нажатии кнопки */
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

        /* Координаты двух вершин */
        QPointF p1 = firstSelectedVertex->sceneBoundingRect().center();
        QPointF p2 = secondSelectedVertex->sceneBoundingRect().center();

        /* Создаем ребро в виде линии */
        QGraphicsLineItem* edge = scene->addLine(QLineF(p1, p2), QPen(Qt::blue, 2));

        /* добавляем текст с весом ребра */
        QGraphicsTextItem* edgeWeight = scene->addText(weightText);
        edgeWeight->setDefaultTextColor(Qt::blue);
        edgeWeight->setPos((p1 + p2) / 2); /* Размещаем текст по центру ребра */

        qDebug() << "Ребро добавлено между вершинами:"
                 << vertices.indexOf(firstSelectedVertex) << "и"
                 << vertices.indexOf(secondSelectedVertex)
                 << "с весом" << weight;

        /* дважды, т.к. граф неориентированный */
        graph[vertices.indexOf(firstSelectedVertex)][vertices.indexOf(secondSelectedVertex)] = weight;
        graph[vertices.indexOf(secondSelectedVertex)][vertices.indexOf(firstSelectedVertex)] = weight;

        /* Сбрасываем выделение вершин */
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

    QComboBox* startComboBox = new QComboBox(this);
    QComboBox* endComboBox = new QComboBox(this);
    for (int i = 0; i < vertices.size(); ++i){
        startComboBox->addItem(QString::number(i));
        endComboBox->addItem(QString::number(i));
    }

    QLabel* lbEnd = new QLabel("Конечная вершина:");
    QPushButton* findPath = new QPushButton("Найти");

    layout->addWidget(lbFirst);
    layout->addWidget(startComboBox);
    layout->addWidget(lbEnd);
    layout->addWidget(endComboBox);
    layout->addWidget(findPath);
    dialog->setLayout(layout);

    connect(findPath, &QPushButton::clicked, this, [=](){
        int start = startComboBox->currentIndex();
        int end = endComboBox->currentIndex();

        dialog->accept(); // accept перед вызовом, чтобы текущее диалоговое окно закрылось
        createGraphClone(start, end);

    });

    dialog->exec();
}

void MainWindow::highlightFindPath(QVector<int>&path)
{   
    for (int i = 0; i < path.size() - 1; ++i){
        QPointF p1 = vertices[path[i]]->sceneBoundingRect().center();
        QPointF p2 = vertices[path[i + 1]]->sceneBoundingRect().center();
        scene->addLine(QLineF(p1, p2), QPen(Qt::green, 2));
    }

    ui->resetFindedPathBtn->setVisible(true);
    connect(ui->resetFindedPathBtn, &QPushButton::clicked, [=](){
        for (int i = 0; i < path.size() - 1; ++i){
            QPointF p1 = vertices[path[i]]->sceneBoundingRect().center();
            QPointF p2 = vertices[path[i + 1]]->sceneBoundingRect().center();
            scene->addLine(QLineF(p1, p2), QPen(Qt::blue, 2));
        }
    });
}

void MainWindow::createGraphClone(int start, int end)
{
    ui->viewOriginalGraphBtn->setVisible(true);

    sceneCloneWidget = new QWidget();
    sceneCloneWidget->setWindowTitle("Исходный граф");

    /* Создаём виджет QGraphicsView */
    QGraphicsView* view = new QGraphicsView(sceneCloneWidget);

    /* Добавляем компоновку для автоматического растягивания */
    QVBoxLayout* layout = new QVBoxLayout(sceneCloneWidget);
    layout->addWidget(view);
    sceneCloneWidget->setLayout(layout);

    /* Создаём клонированную сцену */
    QGraphicsScene* sceneClone = new QGraphicsScene();
    sceneClone->setSceneRect(scene->sceneRect()); /* Используем оригинальный размер сцены */

    for (auto item : scene->items()) {
        if (auto vertex = dynamic_cast<Vertex*>(item)) {
            Vertex* vertexClone = vertex->clone();
            vertexClone->setPos(vertex->pos()); /* Устанавливаем позицию */
            sceneClone->addItem(vertexClone);
        } else if (auto line = dynamic_cast<QGraphicsLineItem*>(item)) {
            QGraphicsLineItem* lineClone = new QGraphicsLineItem(line->line());
            lineClone->setPen(line->pen());
            sceneClone->addItem(lineClone);
        } else if (auto text = dynamic_cast<QGraphicsTextItem*>(item)) {
            QGraphicsTextItem* textClone = new QGraphicsTextItem(text->toPlainText());
            textClone->setFont(text->font());
            textClone->setDefaultTextColor(text->defaultTextColor());
            textClone->setPos(text->pos());
            sceneClone->addItem(textClone);
        }
    }

    view->setScene(sceneClone);
    sceneCloneWidget->hide();

    connect(ui->viewOriginalGraphBtn, &QPushButton::clicked, this, [&](){
        sceneCloneWidget->show();
    });

    QVector<int> res = GraphAlgos::djkstra(graph, start, end);
    highlightFindPath(res);
}

void MainWindow::resetGraph()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Предупреждение", "Подтвердите удаление графа", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes){
        vertices.clear();
        scene->clear();
        currentVertexNum = 0;
        ui->resetFindedPathBtn->setVisible(false);
    }
}
