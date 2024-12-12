#ifndef VERTEX_H
#define VERTEX_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>
#include <QFont>

class Vertex : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    /* parent для того, чтобы сцена отвечала за очистку памяти текущего класса */
    explicit Vertex(qreal x, qreal y, qreal radius, int index, QGraphicsItem* parent = nullptr);

    QColor getCurrentColor() const ;
    void setCurrentColor(const QColor& color);

signals:
    void signalVertexClicked(Vertex* vertex);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QGraphicsSimpleTextItem* textItem;
    QColor currentColor;
};

#endif // VERTEX_H
