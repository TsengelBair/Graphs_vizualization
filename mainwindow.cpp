#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QScreen>

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
    /*
      первым и вторым параметром ожидаются координаты верхнего левого угла прямоугольника, в который вписан эллипс
      текущие расчеты позволяют отрисовывать эллипс на том месте, куда кликнул пользователь
    */
    QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(mousePos.x() - radius, mousePos.y() - radius, 2 * radius, 2 * radius);
    /*
      У QGraphicsEllipseItem нет метода для установки текста, поэтому отдельно создаем текст для эллипса/вершины
      ниже сгуппируем через QGraphicsItemGroup и вершину и ее индекс
    */
    QGraphicsTextItem* textItem = new QGraphicsTextItem(QString::number(curVertexNum));

    // Устанавливаем шрифт и цвет текста
    QFont font = textItem->font();
    font.setPointSize(20);
    textItem->setFont(font);

    // Позиционируем текст в центре эллипса
    textItem->setPos(mousePos.x() - textItem->boundingRect().width() / 2,
                     mousePos.y() - textItem->boundingRect().height() / 2);

    /* для отслеживания вершины по индексу группируем вершину вместе с ее индексом и добавляем в вектор */
    QGraphicsItemGroup* group = new QGraphicsItemGroup();
    group->addToGroup(ellipse);
    group->addToGroup(textItem);

    scene->addItem(group);

    vertices.push_back(group);
    ++curVertexNum;
}
