#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "vertex.h"

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

public slots:
    void handleCickOnVertex(Vertex* vertex);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void slotAddVertex();

private:
    Ui::MainWindow *ui;
    QGraphicsScene* scene;
    int curVertexNum; /* пока просто инкрементирую индекс добавляемых вершин */

    // вершины храню в векторе, т.к планирую обращаться по индексу
    QVector<Vertex*> vertices;
};
#endif // MAINWINDOW_H
