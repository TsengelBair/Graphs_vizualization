## Step by step guide

### QGraphicsScene

Вся визуализация осуществляется за счет класса QGraphicsScene

Создадим объект сцены, причем самой сцене нужно задать максимальные размеры

```c++
scene = new QGraphicsScene();
ui->graphicsView->setScene(scene);
/*
    Устанавливаем фиксированные границы сцены
    в противном случае при добавлении вершин происходит перемещение всех вершин
*/
scene->setSceneRect(0, 0, QApplication::primaryScreen()->geometry().width(), QApplication::primaryScreen()->geometry().height());    
```

### QContextMenuEvent

Для реализации логики появления контекстного меню по ПКМ переопределим метод contextMenuEvent

```c++
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction* addVertexAction = new QAction("Добавить вершину", this);
    QAction* cancelAction = new QAction("Отмена", this); /* без коннекта, т.к. при нажатии автоматически закроется */

    menu.addAction(addVertexAction);
    connect(addVertexAction, &QAction::triggered, this, &MainWindow::slotAddVertex);

    menu.addAction(cancelAction);

    /* Отображаем меню в позиции курсора */
    menu.exec(event->globalPos());
}
```

### Vertex

После клика на "Добавить вершину" необходимо создать вершину и отобразить ее в ui, изначально для отображения использовался встроенный класс QGraphicsEllipseItem, но у данного класса нет метода установки текста, поэтому в качестве вершины используется кастомный класс Vertex, унаследованный от QGraphicsEllipseItem и QObject (для сигналов и слотов)

```c++
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
```

В конструктор передаем координаты места нажатия из mainWindow, радиус круга представляющего вершину (фиксированная переменная qreal radius = 30.0) и индекс (просто автоинкрементируемый счетчик)

Внутри себя данный класс содержит вершину (ее отображение через QGraphicsEllipseItem, причем расчеты в его конструкторе нужны для того, чтобы вершина появлялась ровно в месте нажатия) и номер вершины через QGraphicsTextItem

```c++
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
```

Помимо отображения вершины в ui, ее нужно также сохранить в любой структуре данных, подходящей для представления графа.

В данном приложении используется представление графа в виде матрицы смежности (вложенный вектор)

**Пример матрицы смежности для представления графа**

![exmaple](/img/graph_example.jpg)
```c++
const int INF = 10000;

std::vector<std::vector<int>> graph = {
    {0, 5, 3, 1},
    {5, 0, 8, INF},
    {3, 8, 0, 2},
    {1, INF, 2, 0}
};
```
Для неориентированного (двунаправленного) графа будут выполняться свойства симметричной матрицы , по главной диагонали всегда нули, т.к. путь из до самой себя = 0, константа INF сигнализирует об отсутствии ребра между вершинами

Слот обработчик добавления вершины

```c++
void MainWindow::slotAddVertex()
{
    QPointF mousePos = ui->graphicsView->mapToScene(QCursor::pos());
    Vertex* vertex = new Vertex(mousePos.x(), mousePos.y(), radius, currentVertexNum);

    scene->addItem(vertex);

    vertices.push_back(vertex);
    ++currentVertexNum;

    /* при добавлении вершины ресайзим граф и размер каждой вершины */
    graph.resize(vertices.size());
    for (int i = 0; i < graph.size(); ++i){
        graph[i].resize(graph.size());
    }

    qDebug() << graph.size();

    connect(vertex, &Vertex::signalVertexClicked, this, &MainWindow::slotHandleVertexClick);
}
```

### Добавление вершинам ребер и весов

Для добавление ребра с весом необходимо выбрать две вершины

Выбор осуществляется через "клик" по соответствующей вершине, причем если одна и та же вершина нажата дважды, выделение цветом у кликнутой вершины нужно сбросить, для этого отслеживаем первую нажатую вершину 

```c++
private:
    /* отслеживаем первую "нажатую" вершину */
    Vertex* firstSelectedVertex;
```

```c++
void MainWindow::slotHandleVertexClick(Vertex *clickedVertex)
{
    if (!firstSelectedVertex){
        setFirstVertex(clickedVertex);
        clickedVertex->setPen(QPen(Qt::red, 3));
    } else if (firstSelectedVertex && firstSelectedVertex == clickedVertex){
        firstSelectedVertex->setPen(QPen(Qt::black, 2));
        setFirstVertex(nullptr);
    }  else {
        showDialog(clickedVertex);
    }
}
```

Как только были выбраны две вершины вызываем метод showDialog (передав вторую вершину)

![dialog](/img/dialog.png)

```c++
void MainWindow::showDialog(Vertex *secondSelectedVertex)
{
    /* Если выбрана вторая вершина, создаем диалоговое окно */
    secondSelectedVertex->setPen(QPen(Qt::red, 3));

    QDialog* edgeDialog = new QDialog(this);

    QVBoxLayout* layout = new QVBoxLayout(edgeDialog);
    QLabel* label = new QLabel("Введите вес ребра:");
    QLineEdit* le = new QLineEdit();
    QPushButton* createEdgeBtn = new QPushButton("Добавить ребро");
    QPushButton* cancelBtn = new QPushButton("Отмена");

    layout->addWidget(label);
    layout->addWidget(le);
    layout->addWidget(createEdgeBtn);
    layout->addWidget(cancelBtn);
    edgeDialog->setLayout(layout);

    connect(edgeDialog, &QDialog::rejected, [&]() {
        /* Сбрасываем выделение, если пользователь отменяет ввод */
        firstSelectedVertex->setPen(QPen(Qt::black, 2));
        secondSelectedVertex->setPen(QPen(Qt::black, 2));
        setFirstVertex(nullptr);
    });

    /* Добавление ребра при нажатии кнопки */
    connect(createEdgeBtn, &QPushButton::clicked ,this, [=]() {
        QString weightText = le->text();
        if (weightText.isEmpty()) {
            qDebug() << "Вес ребра не введен!";
            return;
        }

        bool ok;
        int weight = weightText.toInt(&ok);
        if (!ok) {
            qDebug() << "Ошибка: введите число!";
            return;
        }

        /* Координаты двух вершин */
        QPointF p1 = firstSelectedVertex->sceneBoundingRect().center();
        QPointF p2 = secondSelectedVertex->sceneBoundingRect().center();

        /* Создаем ребро в виде линии */
        QGraphicsLineItem* edge = scene->addLine(QLineF(p1, p2), QPen(Qt::blue, 2));

        /* добавляем текст с весом ребра */
        QGraphicsTextItem* edgeWeight = scene->addText(weightText);
        edgeWeight->setDefaultTextColor(Qt::blue);
        edgeWeight->setPos((p1 + p2) / 2); /* Размещаем текст по центру ребра */

        qDebug() << "Ребро добавлено между вершинами:"
                 << vertices.indexOf(firstSelectedVertex) << "и"
                 << vertices.indexOf(secondSelectedVertex)
                 << "с весом" << weight;

        /* дважды, т.к. граф неориентированный */
        graph[vertices.indexOf(firstSelectedVertex)][vertices.indexOf(secondSelectedVertex)] = weight;
        graph[vertices.indexOf(secondSelectedVertex)][vertices.indexOf(firstSelectedVertex)] = weight;

        /* Сбрасываем выделение вершин */
        firstSelectedVertex->setPen(QPen(Qt::black, 2));
        secondSelectedVertex->setPen(QPen(Qt::black, 2));
        firstSelectedVertex = nullptr;

        edgeDialog->accept();
    });

    connect(cancelBtn, &QPushButton::clicked, edgeDialog, &QDialog::reject);
    edgeDialog->exec();
}
```

Создание ребра с весом в ui и сохранение данных в структуру графа

```c++
    /* Координаты двух вершин */
    QPointF p1 = firstSelectedVertex->sceneBoundingRect().center();
    QPointF p2 = secondSelectedVertex->sceneBoundingRect().center();

    /* Создаем ребро в виде линии */
    QGraphicsLineItem* edge = scene->addLine(QLineF(p1, p2), QPen(Qt::blue, 2));

    /* добавляем текст с весом ребра */
    QGraphicsTextItem* edgeWeight = scene->addText(weightText);
    edgeWeight->setDefaultTextColor(Qt::blue);
    edgeWeight->setPos((p1 + p2) / 2); /* Размещаем текст по центру ребра */

    qDebug() << "Ребро добавлено между вершинами:"
                << vertices.indexOf(firstSelectedVertex) << "и"
                << vertices.indexOf(secondSelectedVertex)
                << "с весом" << weight;

    /* дважды, т.к. граф неориентированный */
    graph[vertices.indexOf(firstSelectedVertex)][vertices.indexOf(secondSelectedVertex)] = weight;
    graph[vertices.indexOf(secondSelectedVertex)][vertices.indexOf(firstSelectedVertex)] = weight;

    /* Сбрасываем выделение вершин */
    firstSelectedVertex->setPen(QPen(Qt::black, 2));
    secondSelectedVertex->setPen(QPen(Qt::black, 2));
    firstSelectedVertex = nullptr;
```

### Поиск маршрута

В кач-ве алгоритма поиска маршрута взята классическая Дейкстра 

```c++
void MainWindow::handleFindPath()
{
    /*
        Если ребро между вершинами отсутствует, значение graph[i][j] == 0
        graph[i][j] = 10000 это идентификация отсутствия ребра
        элементы главной диагонали всегда нули (граф не петлевой)
    */
    for (int i = 0; i < graph.size(); ++i){
        for (int j = 0; j < graph.size(); ++j){
            if (i != j && graph[i][j] == 0){
                graph[i][j] = 10000;
            }
        }
    }

    QDialog* dialog = new QDialog(this);

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    QLabel* lbFirst = new QLabel("Стартовая вершина:");

    QComboBox* startComboBox = new QComboBox(this);
    QComboBox* endComboBox = new QComboBox(this);
    for (int i = 0; i < vertices.size(); ++i){
        startComboBox->addItem(QString::number(i));
        endComboBox->addItem(QString::number(i));
    }

    QLabel* lbEnd = new QLabel("Конечная вершина:");
    QPushButton* findPath = new QPushButton("Найти");

    layout->addWidget(lbFirst);
    layout->addWidget(startComboBox);
    layout->addWidget(lbEnd);
    layout->addWidget(endComboBox);
    layout->addWidget(findPath);
    dialog->setLayout(layout);

    connect(findPath, &QPushButton::clicked, this, [=](){
        int start = startComboBox->currentIndex();
        int end = endComboBox->currentIndex();
        QVector<int> res = GraphAlgos::djkstra(graph, start, end);
        highlightFindPath(res);
        dialog->accept();
    });

    dialog->exec();
}
```

### Example

![1](/img/1.jpg)
