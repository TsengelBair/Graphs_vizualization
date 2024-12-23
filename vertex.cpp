#include "vertex.h"

#include <QPen>
#include <QFont>
#include <QGraphicsTextItem>

Vertex::Vertex(qreal x, qreal y, qreal radius, int index)
    : QObject()
    , QGraphicsEllipseItem(QRectF(x - radius, y - radius, 2 * radius, 2 * radius))
    , _index(index)
{
    /* черная обводка по умолчанию */
    setPen(QPen(Qt::black, 2));

    /* Добавляем текст с индексом в центр вершины */
    QGraphicsTextItem* textItem = new QGraphicsTextItem(QString::number(index), this);

    /* Настраиваем шрифт и позиционируем текст */
    QFont font = textItem->font();
    font.setPointSize(20);
    textItem->setFont(font);

    textItem->setPos(boundingRect().center() - QPointF(textItem->boundingRect().width() / 2,
                                                       textItem->boundingRect().height() / 2));
}

int Vertex::getIndex() const
{
    return _index;
}

Vertex *Vertex::clone()
{
    // Создаём новую вершину с теми же координатами, радиусом и индексом
    qreal radius = rect().width() / 2; // Радиус вычисляем из текущего размера
    Vertex* newVertex = new Vertex(rect().center().x(), rect().center().y(), radius, _index);

    // Копируем свойства: обводка (Pen) и цвет заливки (Brush)
    newVertex->setPen(this->pen());
    newVertex->setBrush(this->brush());

    // Текст с индексом уже создаётся в конструкторе, так что дополнительного копирования не требуется

    return newVertex;
}

void Vertex::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    emit signalVertexClicked(this);
}
