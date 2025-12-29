#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <clocale>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <windows.h>
#include <cmath>

using namespace std;

const double INF = numeric_limits<double>::infinity();

class Graph;

extern Graph currentGraph;
extern vector<double> lastDistances;
extern vector<int> lastPredecessor;
extern int lastStartVertex;
extern bool graphCreated;

void clearScreen();

struct Edge {
    int from;
    int to;
    double weight;

    Edge(int f, int t, double w = 1.0) : from(f), to(t), weight(w) {}
};

class Graph {
private:
    int vertices;
    vector<Edge> edges;
    bool isDirected;
    bool isWeighted;

public:
    Graph(int v = 0, bool directed = true, bool weighted = true)
        : vertices(v), isDirected(directed), isWeighted(weighted) {
    }

    void setVertices(int v) {
        vertices = v;
        edges.clear();
    }

    void setDirected(bool directed) {
        isDirected = directed;
    }

    void setWeighted(bool weighted) {
        isWeighted = weighted;
    }

    void addEdge(int from, int to, double weight = 1.0) {
        if (!isWeighted) weight = 1.0;

        edges.push_back(Edge(from, to, weight));

        if (!isDirected) {
            edges.push_back(Edge(to, from, weight));
        }
    }

    void clearEdges() {
        edges.clear();
    }

    int getVerticesCount() const {
        return vertices;
    }

    bool getIsDirected() const {
        return isDirected;
    }

    bool getIsWeighted() const {
        return isWeighted;
    }

    const vector<Edge>& getEdges() const {
        return edges;
    }

    pair<vector<double>, vector<int>> bellmanFord(int start) const {
        vector<double> dist(vertices, INF);
        vector<int> predecessor(vertices, -1);

        dist[start] = 0;

        for (int i = 0; i < vertices - 1; i++) {
            bool updated = false;

            for (const Edge& edge : edges) {
                if (dist[edge.from] < INF) {
                    if (dist[edge.to] > dist[edge.from] + edge.weight) {
                        dist[edge.to] = dist[edge.from] + edge.weight;
                        predecessor[edge.to] = edge.from;
                        updated = true;
                    }
                }
            }

            if (!updated) break;
        }

        bool hasNegativeCycle = false;
        for (const Edge& edge : edges) {
            if (dist[edge.from] < INF) {
                if (dist[edge.to] > dist[edge.from] + edge.weight) {
                    hasNegativeCycle = true;
                    break;
                }
            }
        }

        if (hasNegativeCycle) {
            cout << "Граф содержит отрицательный цикл!" << endl;
            fill(dist.begin(), dist.end(), INF);
            fill(predecessor.begin(), predecessor.end(), -1);
        }

        return { dist, predecessor };
    }

    vector<int> getPath(const vector<int>& predecessor, int start, int target) const {
        vector<int> path;

        if (predecessor[target] == -1 && target != start) {
            return path;
        }

        for (int v = target; v != -1; v = predecessor[v]) {
            path.push_back(v);
            if (v == start) break;
        }

        reverse(path.begin(), path.end());

        if (!path.empty() && path[0] != start) {
            path.clear();
        }

        return path;
    }

    void printDetailedPaths(int start, const vector<double>& distances,
        const vector<int>& predecessor) const {
        cout << "\nДЕТАЛЬНЫЕ ПУТИ ОТ ВЕРШИНЫ " << start << ":\n";
        cout << "========================================\n";

        for (int i = 0; i < vertices; i++) {
            cout << "До вершины " << i << ": ";
            if (distances[i] == INF) {
                cout << "недостижима";
            }
            else if (i == start) {
                cout << distances[i] << ", путь: " << start;
            }
            else {
                cout << distances[i] << ", путь: ";
                vector<int> path = getPath(predecessor, start, i);
                if (!path.empty()) {
                    for (size_t j = 0; j < path.size(); j++) {
                        cout << path[j];
                        if (j != path.size() - 1) cout << " -> ";
                    }
                }
                else {
                    cout << "не найден";
                }
            }
            cout << endl;
        }
    }

    void printInfo() const {
        cout << "Тип графа: ";
        if (isDirected) cout << "Ориентированный";
        else cout << "Неориентированный";

        cout << ", ";

        if (isWeighted) cout << "Взвешенный";
        else cout << "Невзвешенный";

        cout << "\nКоличество вершин: " << vertices << endl;
        cout << "Количество ребер: " << edges.size() << endl;

        if (isDirected) {
            cout << "Ребра:\n";
            for (const Edge& edge : edges) {
                cout << edge.from << " -> " << edge.to;
                if (isWeighted) {
                    cout << " (вес: " << edge.weight << ")";
                }
                cout << endl;
            }
        }
        else {
            cout << "Ребра (неориентированные):\n";
            vector<vector<bool>> printed(vertices, vector<bool>(vertices, false));
            for (const Edge& edge : edges) {
                int u = min(edge.from, edge.to);
                int v = max(edge.from, edge.to);
                if (!printed[u][v]) {
                    cout << u << " -- " << v;
                    if (isWeighted) {
                        cout << " (вес: " << edge.weight << ")";
                    }
                    cout << endl;
                    printed[u][v] = true;
                }
            }
        }
        cout << endl;
    }

    void saveToFile(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Ошибка открытия файла для записи!" << endl;
            return;
        }

        file << vertices << endl;
        file << (isDirected ? 1 : 0) << endl;
        file << (isWeighted ? 1 : 0) << endl;
        file << edges.size() << endl;

        for (const Edge& edge : edges) {
            file << edge.from << " " << edge.to << " " << edge.weight << endl;
        }

        file.close();
        cout << "Граф сохранен в файл: " << filename << endl;
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Ошибка открытия файла!" << endl;
            return;
        }

        int v;
        file >> v;
        vertices = v;

        int directed, weighted;
        file >> directed >> weighted;
        isDirected = (directed == 1);
        isWeighted = (weighted == 1);

        edges.clear();
        int edgeCount;
        file >> edgeCount;

        for (int i = 0; i < edgeCount; i++) {
            int from, to;
            double weight;
            file >> from >> to >> weight;
            edges.push_back(Edge(from, to, weight));
        }

        file.close();
        cout << "Граф загружен из файла: " << filename << endl;
    }

    void generateRandomGraph(int maxEdges = -1, double minWeight = -10.0, double maxWeight = 10.0) {
        edges.clear();

        if (maxEdges == -1) {
            maxEdges = vertices * (vertices - 1);
            if (!isDirected) maxEdges /= 2;
        }

        int edgeCount = rand() % maxEdges + 1;

        vector<vector<bool>> added(vertices, vector<bool>(vertices, false));

        for (int i = 0; i < edgeCount; i++) {
            int from, to;

            int attempts = 0;
            do {
                from = rand() % vertices;
                to = rand() % vertices;
                attempts++;
                if (attempts > vertices * vertices * 2) {
                    edgeCount = i;
                    break;
                }
            } while (from == to || added[from][to]);

            if (!isDirected) {
                added[from][to] = true;
                added[to][from] = true;
            }
            else {
                added[from][to] = true;
            }

            double weight = 1.0;
            if (isWeighted) {
                weight = minWeight + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (maxWeight - minWeight)));
                weight = round(weight * 100.0) / 100.0;
            }

            addEdge(from, to, weight);
        }

        cout << "Сгенерирован случайный граф с " << edgeCount << " ребрами" << endl;
    }

    void generateCompleteRandomGraph() {
        clearScreen();  
        cout << "===  СЛУЧАЙНАЯ ГЕНЕРАЦИЯ ГРАФА ===\n\n";

        int vertices;
        bool directed, weighted;
        char dirChoice, weightChoice;

        cout << "Введите количество вершин (от 2 до 20): ";
        cin >> vertices;
        if (vertices < 2) vertices = 2;
        if (vertices > 20) vertices = 20;

        cout << "Ориентированный граф? (y/n): ";
        cin >> dirChoice;
        directed = (dirChoice == 'y' || dirChoice == 'Y');

        cout << "Взвешенный граф? (y/n): ";
        cin >> weightChoice;
        weighted = (weightChoice == 'y' || weightChoice == 'Y');

        setVertices(vertices);
        setDirected(directed);
        setWeighted(weighted);

        double minWeight = -10.0;
        double maxWeight = 10.0;
        if (weighted) {
            cout << "Минимальный вес ребра (по умолчанию -10.0): ";
            cin >> minWeight;
            cout << "Максимальный вес ребра (по умолчанию 10.0): ";
            cin >> maxWeight;

            if (minWeight > maxWeight) {
                swap(minWeight, maxWeight);
                cout << "Диапазоны весов поменяны местами." << endl;
            }
        }

        int densityChoice;
        cout << "\nВыберите плотность графа:\n";
        cout << "1. Разреженный (20% от максимального количества ребер)\n";
        cout << "2. Средней плотности (50% от максимального количества ребер)\n";
        cout << "3. Плотный (80% от максимального количества ребер)\n";
        cout << "4. Случайная плотность\n";
        cout << "Ваш выбор: ";
        cin >> densityChoice;

        int maxPossibleEdges;
        if (directed) {
            maxPossibleEdges = vertices * (vertices - 1);
        }
        else {
            maxPossibleEdges = vertices * (vertices - 1) / 2;
        }

        int maxEdges;
        switch (densityChoice) {
        case 1: maxEdges = maxPossibleEdges * 0.2; break;
        case 2: maxEdges = maxPossibleEdges * 0.5; break;
        case 3: maxEdges = maxPossibleEdges * 0.8; break;
        case 4: maxEdges = maxPossibleEdges; break;
        default: maxEdges = maxPossibleEdges * 0.5; break;
        }

        if (maxEdges < 1) maxEdges = 1;

        generateRandomGraph(maxEdges, minWeight, maxWeight);

        cout << "\nГраф успешно сгенерирован со следующими параметрами:\n";
        cout << "- Вершин: " << vertices << endl;
        cout << "- Тип: " << (directed ? "Ориентированный" : "Неориентированный") << endl;
        cout << "- Веса: " << (weighted ? "Взвешенный" : "Невзвешенный") << endl;
        if (weighted) {
            cout << "- Диапазон весов: [" << minWeight << ", " << maxWeight << "]" << endl;
        }
        cout << "- Плотность: ";
        switch (densityChoice) {
        case 1: cout << "Разреженный"; break;
        case 2: cout << "Средней плотности"; break;
        case 3: cout << "Плотный"; break;
        case 4: cout << "Случайная"; break;
        }
        cout << endl;
    }

    void inputFromKeyboard() {
        edges.clear();
        int edgeCount;

        cout << "Введите количество ребер: ";
        cin >> edgeCount;

        cout << "Введите ребра в формате: из_вершины в_вершину [вес]" << endl;
        cout << "Вершины нумеруются от 0 до " << vertices - 1 << endl;

        for (int i = 0; i < edgeCount; i++) {
            int from, to;
            double weight = 1.0;

            cout << "Ребро " << i + 1 << ": ";
            cin >> from >> to;

            if (isWeighted) {
                cin >> weight;
            }

            if (from < 0 || from >= vertices || to < 0 || to >= vertices) {
                cout << "Ошибка: вершины должны быть в диапазоне 0-" << vertices - 1 << endl;
                i--;
                continue;
            }

            addEdge(from, to, weight);
        }
    }
};

Graph currentGraph;
vector<double> lastDistances;
vector<int> lastPredecessor;
int lastStartVertex = -1;
bool graphCreated = false;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printMenu() {
    cout << "" << endl;
    cout << "            АЛГОРИТМ ФОРДА-БЕЛЛМАНА                " << endl;
    cout << "" << endl;
    cout << " 1. Задать параметры графа                         " << endl;
    cout << " 2. Автоматическая генерация графа                 " << endl;
    cout << " 3. Случайная генерация графа               " << endl;
    cout << " 4. Ввод графа с клавиатуры                 " << endl;
    cout << " 5. Загрузить граф из файла                        " << endl;
    cout << " 6. Показать информацию о графе                    " << endl;
    cout << " 7. Выполнить алгоритм Форда-Беллмана              " << endl;
    cout << " 8. Сохранить граф в файл                          " << endl;
    cout << " 9. Сохранить результаты в файл                    " << endl;
    cout << "10. Демонстрация всех типов графов                 " << endl;
    cout << "11. Тест с отрицательным циклом                    " << endl;
    cout << " 0. Выход                                          " << endl;
    cout << "" << endl;
    cout << "Выберите пункт меню: ";
}

void saveResultsToFile(const Graph& graph, const vector<double>& distances,
    const vector<int>& predecessor, int startVertex,
    const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для записи результатов!" << endl;
        return;
    }

    file << "Результаты алгоритма Форда-Беллмана" << endl;
    file << "=====================================" << endl << endl;

    file << "Параметры графа:" << endl;
    file << "Количество вершин: " << graph.getVerticesCount() << endl;
    file << "Тип: " << (graph.getIsDirected() ? "Ориентированный" : "Неориентированный") << endl;
    file << "Взвешенный: " << (graph.getIsWeighted() ? "Да" : "Нет") << endl;
    file << "Начальная вершина: " << startVertex << endl << endl;

    file << "Кратчайшие расстояния и пути от вершины " << startVertex << ":" << endl;
    for (int i = 0; i < distances.size(); i++) {
        file << "До вершины " << i << ": ";
        if (distances[i] == INF) {
            file << "недостижима";
        }
        else if (i == startVertex) {
            file << distances[i] << ", путь: " << startVertex;
        }
        else {
            file << distances[i] << ", путь: ";
            vector<int> path = graph.getPath(predecessor, startVertex, i);
            if (!path.empty()) {
                for (size_t j = 0; j < path.size(); j++) {
                    file << path[j];
                    if (j != path.size() - 1) file << " -> ";
                }
            }
            else {
                file << "не найден";
            }
        }
        file << endl;
    }

    file.close();
    cout << "Результаты сохранены в файл: " << filename << endl;
}

void demonstrateAllGraphTypes() {
    clearScreen();
    cout << "=== РАЗНЫЕ ТИПЫ ГРАФОВ И АЛГОРИТМ ФОРДА-БЕЛЛМАНА ===\n\n";

    vector<pair<string, Graph>> graphs = {
        {"ОРИЕНТИРОВАННЫЙ ВЗВЕШЕННЫЙ ГРАФ", Graph(5, true, true)},
        {"ОРИЕНТИРОВАННЫЙ НЕВЗВЕШЕННЫЙ ГРАФ", Graph(5, true, false)},
        {"НЕОРИЕНТИРОВАННЫЙ ВЗВЕШЕННЫЙ ГРАФ", Graph(5, false, true)},
        {"НЕОРИЕНТИРОВАННЫЙ НЕВЗВЕШЕННЫЙ ГРАФ", Graph(6, false, false)}
    };

    graphs[0].second.addEdge(0, 1, 6);
    graphs[0].second.addEdge(0, 2, 7);
    graphs[0].second.addEdge(1, 2, 8);
    graphs[0].second.addEdge(1, 3, 5);
    graphs[0].second.addEdge(1, 4, -4);
    graphs[0].second.addEdge(2, 3, -3);
    graphs[0].second.addEdge(2, 4, 9);
    graphs[0].second.addEdge(3, 1, -2);
    graphs[0].second.addEdge(4, 0, 2);
    graphs[0].second.addEdge(4, 3, 7);

    graphs[1].second.addEdge(0, 1);
    graphs[1].second.addEdge(0, 2);
    graphs[1].second.addEdge(1, 2);
    graphs[1].second.addEdge(1, 3);
    graphs[1].second.addEdge(2, 3);
    graphs[1].second.addEdge(3, 4);

    graphs[2].second.addEdge(0, 1, 2);
    graphs[2].second.addEdge(0, 2, 3);
    graphs[2].second.addEdge(1, 2, 1);
    graphs[2].second.addEdge(1, 3, 4);
    graphs[2].second.addEdge(2, 4, 5);
    graphs[2].second.addEdge(3, 4, 1);

    graphs[3].second.addEdge(0, 1);
    graphs[3].second.addEdge(0, 2);
    graphs[3].second.addEdge(1, 3);
    graphs[3].second.addEdge(2, 3);
    graphs[3].second.addEdge(3, 4);
    graphs[3].second.addEdge(4, 5);

    for (int i = 0; i < graphs.size(); i++) {
        cout << i + 1 << ". " << graphs[i].first << ":\n";
        graphs[i].second.printInfo();

        auto result = graphs[i].second.bellmanFord(0);
        vector<double> distances = result.first;
        vector<int> predecessor = result.second;

        graphs[i].second.printDetailedPaths(0, distances, predecessor);
        cout << "\n" << string(60, '-') << "\n\n";
    }

    cout << "Нажмите Enter для продолжения...";
    cin.ignore();
    cin.get();
}

void testNegativeCycle() {
    clearScreen();
    cout << "=== ТЕСТ С ОТРИЦАТЕЛЬНЫМ ЦИКЛОМ ===\n\n";

    Graph g(4, true, true);
    g.addEdge(0, 1, 1);
    g.addEdge(1, 2, -1);
    g.addEdge(2, 3, -1);
    g.addEdge(3, 1, -1);

    g.printInfo();

    auto result = g.bellmanFord(0);
    vector<double> distances = result.first;
    vector<int> predecessor = result.second;

    g.printDetailedPaths(0, distances, predecessor);

    cout << "\nНажмите Enter для продолжения...";
    cin.ignore();
    cin.get();
}

int main() {
    setlocale(LC_ALL, "rus");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    srand(time(NULL));

    int choice;
    do {
        clearScreen();
        printMenu();
        cin >> choice;
        clearScreen();

        switch (choice) {
        case 1: {
            int vertices;
            bool directed, weighted;
            char dirChoice, weightChoice;

            cout << "" << endl;
            cout << "            ЗАДАНИЕ ПАРАМЕТРОВ ГРАФА               " << endl;
            cout << "" << endl;

            cout << "Введите количество вершин: ";
            cin >> vertices;

            cout << "Ориентированный граф? (y/n): ";
            cin >> dirChoice;
            directed = (dirChoice == 'y' || dirChoice == 'Y');

            cout << "Взвешенный граф? (y/n): ";
            cin >> weightChoice;
            weighted = (weightChoice == 'y' || weightChoice == 'Y');

            currentGraph.setVertices(vertices);
            currentGraph.setDirected(directed);
            currentGraph.setWeighted(weighted);
            currentGraph.clearEdges();

            graphCreated = true;
            cout << "\nПараметры графа установлены!" << endl;
            cout << "Нажмите Enter для продолжения...";
            cin.ignore();
            cin.get();
            break;
        }

        case 2: {
            if (!graphCreated) {
                cout << "Сначала задайте параметры графа (пункт 1)!" << endl;
            }
            else {
                currentGraph.clearEdges();
                currentGraph.generateRandomGraph();
                cout << "\nГраф сгенерирован успешно!" << endl;
            }
            cout << "Нажмите Enter для продолжения...";
            cin.ignore();
            cin.get();
            break;
        }

        case 3: {
            currentGraph.generateCompleteRandomGraph();
            graphCreated = true;
            cout << "\nНажмите Enter для продолжения...";
            cin.ignore();
            cin.get();
            break;
        }

        case 4: {
            if (!graphCreated) {
                cout << "Сначала задайте параметры графа (пункт 1)!" << endl;
            }
            else {
                currentGraph.clearEdges();
                currentGraph.inputFromKeyboard();
                cout << "\nГраф введен успешно!" << endl;
            }
            cout << "Нажмите Enter для продолжения...";
            cin.ignore();
            cin.get();
            break;
        }

        case 5: {
            string filename;
            cout << "Введите имя файла для загрузки: ";
            cin >> filename;
            currentGraph.loadFromFile(filename);
            graphCreated = true;
            cout << "Нажмите Enter для продолжения...";
            cin.ignore();
            cin.get();
            break;
        }

        case 6: {
            if (!graphCreated) {
                cout << "Граф еще не создан!" << endl;
            }
            else {
                currentGraph.printInfo();
            }
            cout << "Нажмите Enter для продолжения...";
            cin.ignore();
            cin.get();
            break;
        }

        case 7: {
            if (!graphCreated) {
                cout << "Граф еще не создан!" << endl;
            }
            else {
                int startVertex;
                cout << "Введите начальную вершину (0-" << currentGraph.getVerticesCount() - 1 << "): ";
                cin >> startVertex;

                if (startVertex < 0 || startVertex >= currentGraph.getVerticesCount()) {
                    cout << "Неверная вершина!" << endl;
                }
                else {
                    auto result = currentGraph.bellmanFord(startVertex);
                    lastDistances = result.first;
                    lastPredecessor = result.second;
                    lastStartVertex = startVertex;

                    currentGraph.printDetailedPaths(startVertex, lastDistances, lastPredecessor);
                }
            }
            cout << "\nНажмите Enter для продолжения...";
            cin.ignore();
            cin.get();
            break;
        }

        case 8: {
            if (!graphCreated) {
                cout << "Граф еще не создан!" << endl;
            }
            else {
                string filename;
                cout << "Введите имя файла для сохранения: ";
                cin >> filename;
                currentGraph.saveToFile(filename);
            }
            cout << "Нажмите Enter для продолжения...";
            cin.ignore();
            cin.get();
            break;
        }

        case 9: {
            if (!graphCreated || lastStartVertex == -1) {
                cout << "Сначала выполните алгоритм (пункт 7)!" << endl;
            }
            else {
                string filename;
                cout << "Введите имя файла для сохранения результатов: ";
                cin >> filename;
                saveResultsToFile(currentGraph, lastDistances, lastPredecessor, lastStartVertex, filename);
            }
            cout << "Нажмите Enter для продолжения...";
            cin.ignore();
            cin.get();
            break;
        }

        case 10: {
            demonstrateAllGraphTypes();
            break;
        }

        case 11: {
            testNegativeCycle();
            break;
        }

        case 0: {
            cout << "Выход из программы..." << endl;
            break;
        }

        default: {
            cout << "Неверный выбор! Попробуйте снова." << endl;
            cout << "Нажмите Enter для продолжения...";
            cin.ignore();
            cin.get();
            break;
        }
        }
    } while (choice != 0);

    return 0;
}