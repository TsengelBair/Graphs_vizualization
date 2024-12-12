#include <QPen>

#include "vertex.h"

Vertex::Vertex(qreal x, qreal y, qreal radius, int index, QGraphicsItem* parent)
    : QGraphicsEllipseItem(x - radius, y - radius, 2 * radius, 2 * radius, parent),
      currentColor(Qt::black)
{
    /* по умолчанию вершина с черной обводкой */
    setPen(QPen(Qt::black, 2));

    // Создаем текстовый элемент и добавляем его в качестве дочернего элемента
    textItem = new QGraphicsSimpleTextItem(QString::number(index), this);

    // Настраиваем шрифт и позиционируем текст по центру эллипса
    QFont font = textItem->font();
    font.setPointSize(20);
    textItem->setFont(font);

    textItem->setPos(x - textItem->boundingRect().width() / 2,
                     y - textItem->boundingRect().height() / 2);
}

QColor Vertex::getCurrentColor() const
{
    return currentColor;
}

void Vertex::setCurrentColor(const QColor &color)
{
    currentColor = color;
}

/* выделение вершины цветом при клике */
void Vertex::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (pen().color() == Qt::red) setPen(QPen(Qt::black, 2));
    else setPen(QPen(Qt::red, 3));

    // Вызываем базовую реализацию для стандартного поведения
    QGraphicsEllipseItem::mousePressEvent(event);
}
