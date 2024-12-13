#ifndef VERTEX_H
#define VERTEX_H

#include <QObject>
#include <QGraphicsEllipseItem>

class Vertex : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    explicit Vertex(qreal x, qreal y, qreal radius, int index);

    int getIndex() const ;

signals:
    /* сигнал клика по вершине обработаем в mainwindow */
    void signalVertexClicked(Vertex* vertex);

protected:
    /* в переопределенном событии emit signalVertexClicked */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    int _index;
};

#endif /* VERTEX_H */
