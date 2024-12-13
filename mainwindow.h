#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Vertex;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setFirstVertex(Vertex* vertex);

public slots:
    void slotHandleVertexClick(Vertex* clickedVertex);

protected:
    /* создание контекстного меню по ПКМ */
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void slotAddVertex();

private:
    /*
       открытие диалогового окна, должно появиться когда "нажаты" две вершины
       внутри форма, в которую нужно ввести вес ребра
    */
    void showDialog(Vertex* secondSelectedVertex);
    void handleFindPath();
    void highlightFindPath(QVector<int>&path);

private:
    /* отслеживаем первую "нажатую" вершину */
    Vertex* firstSelectedVertex;

    /* QGraphicView определен в UI */
    Ui::MainWindow *ui;
    QGraphicsScene* scene;
    /* счетчик для установки индекса текущей вершины */
    int currentVertexNum;
    /*
       радиус вершины (вершина представлена кругом)
       использую qreal так как именно этот тип данных ожидается в QGraphicEllipse
    */
    qreal radius;
    QVector<Vertex*>vertices;
    /*
       граф в виде матрицы смежности
    */
    QVector<QVector<int>>graph;
};
#endif /* MAINWINDOW_H */
