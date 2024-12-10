#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <QVector>
#include <QHash>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void slotAddVertex();

private:
    Ui::MainWindow *ui;
    QGraphicsScene* scene;
    int curVertexNum; /* пока просто инкрементирую индекс добавляемых вершин */

    // вершины храню в векторе, т.к планирую обращаться по индексу
    QVector<QGraphicsItemGroup*> vertices;

    // ключ - пара вершин (их номера), значение - это вес ребра
    // исп-ю QHas, т.к. при добавлении будет проверка на существование
    QHash<QPair<int, int>, int> edges;
};
#endif // MAINWINDOW_H
