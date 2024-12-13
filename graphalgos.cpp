#include <QPair>
#include <QMultiMap>

#include "graphalgos.h"

GraphAlgos::GraphAlgos()
{
}

QVector<int> GraphAlgos::djkstra(QVector<QVector<int>> &graph, int start, int end)
{
    int n = graph.size();

    QVector<int> dist(n, 10000); // Инициализируем все расстояния "бесконечностью"
    QVector<int> from(n, -1);   // Для хранения пути

    // Очередь с приоритетом на основе QMultiMap
    QMultiMap<int, int> q; // Ключ: расстояние, значение: вершина

    dist[start] = 0;
    q.insert(0, start); // Добавляем стартовую вершину с расстоянием 0

    while (!q.isEmpty()) {
        // Получаем вершину с минимальным весом
        int weight = q.firstKey();
        int minVertex = q.first();
        q.erase(q.begin()); // Удаляем её из очереди

        if (weight > dist[minVertex])
            continue;

        // Обходим соседей текущей вершины
        for (int v = 0; v < n; ++v) {
            if (graph[minVertex][v] != 10000 && dist[v] > weight + graph[minVertex][v]) {
                dist[v] = weight + graph[minVertex][v];
                from[v] = minVertex;

                // Добавляем/обновляем вершину в очереди с приоритетом
                q.insert(dist[v], v);
            }
        }
    }

    // Восстановление пути
    QVector<int> path;
    for (int v = end; v != -1; v = from[v]) {
        path.push_back(v);
    }

    // Разворачиваем путь
    std::reverse(path.begin(), path.end());
    return path;
}
