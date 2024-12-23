#include "graphalgos.h"

#include <queue>

GraphAlgos::GraphAlgos()
{
}

QVector<int> GraphAlgos::djkstra(QVector<QVector<int>> &graph, int start, int end)
{
    int n = graph.size();

    QVector<int> dist(n, 10000); /* Инициализируем все расстояния "бесконечностью" */
    QVector<int> from(n, -1);   /* Для хранения пути */

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>>q;

    dist[start] = 0;
    q.push(std::make_pair(0, start)); /* Добавляем стартовую вершину с расстоянием 0 */

    while (!q.empty()) {
        /* Получаем вершину с минимальным весом */
        int weight = q.top().first;
        int minVertex = q.top().second;
        q.pop(); /* Удаляем её из очереди */

        if (weight > dist[minVertex]) continue;

        /* Обходим соседей текущей вершиныs */
        for (int v = 0; v < n; ++v) {
            if (graph[minVertex][v] != 10000 && dist[v] > weight + graph[minVertex][v]) {
                dist[v] = weight + graph[minVertex][v];
                from[v] = minVertex;

                /* Добавляем/обновляем вершину в очереди с приоритетом */
                q.push(std::make_pair(dist[v], v));
            }
        }
    }

    /* Восстановление пути */
    QVector<int> path;
    for (int v = end; v != -1; v = from[v]) {
        path.push_back(v);
    }

    /* Разворачиваем путь */
    std::reverse(path.begin(), path.end());
    return path;
}
