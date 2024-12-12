#include "vertex.h"

Vertex::Vertex(qreal x, qreal y, qreal radius, int index, QGraphicsItem* parent)
    : QGraphicsEllipseItem(x - radius, y - radius, 2 * radius, 2 * radius, parent)
{
    // Создаем текстовый элемент и добавляем его в качестве дочернего элемента
    textItem = new QGraphicsSimpleTextItem(QString::number(index), this);

    // Настраиваем шрифт и позиционируем текст по центру эллипса
    QFont font = textItem->font();
    font.setPointSize(20);
    textItem->setFont(font);

    textItem->setPos(x - textItem->boundingRect().width() / 2,
                     y - textItem->boundingRect().height() / 2);
}

