#ifndef VERTEX_H
#define VERTEX_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>
#include <QFont>

class Vertex : public QGraphicsEllipseItem
{
public:
    /* parent для того, чтобы сцена отвечала за очистку памяти текущего класса */
    explicit Vertex(qreal x, qreal y, qreal radius, int index, QGraphicsItem* parent = nullptr);

    void setIndex(int index);
    int getIndex() const;

private:
    QGraphicsSimpleTextItem* textItem;
};

#endif // VERTEX_H
