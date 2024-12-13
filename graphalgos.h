#ifndef GRAPHALGOS_H
#define GRAPHALGOS_H

#include <QVector>

class GraphAlgos
{
public:
    GraphAlgos();

    static QVector<int> djkstra(QVector<QVector<int>> &graph, int start, int end);
};

#endif /* GRAPHALGOS_H */
