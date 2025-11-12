#include "GraphController.h"
#include <QQueue>
#include <QStack>
#include <QSet>
#include <limits>
#include <algorithm>

GraphController::GraphController(Graph* graph, ReportManager* reportManager, QObject* parent) 
    : QObject(parent), m_graph(graph), m_reportManager(reportManager) {
}

GraphController::~GraphController() {
}

Graph* GraphController::getGraph() {
    return m_graph;
}

ReportManager* GraphController::getReportManager() {
    return m_reportManager;
}

void GraphController::loadMap() {
    QVector<Station> stations = m_graph->getAllStations();
    QVector<Edge> edges = m_graph->getAllEdges();
    emit mapLoaded(stations, edges);
}

void GraphController::addEdge(int from, int to, double weight) {
    try {
        if (!m_graph->hasStation(from) || !m_graph->hasStation(to)) {
            emit connectionAdded(from, to, weight, false);
            emit errorOccurred("Una o ambas estaciones no existen");
            return;
        }
        m_graph->addEdge(from, to, weight, true);
        emit connectionAdded(from, to, weight, true);
    } catch (...) {
        emit connectionAdded(from, to, weight, false);
        emit errorOccurred("Error al agregar conexión");
    }
}

void GraphController::removeEdge(int from, int to) {
    try {
        m_graph->removeEdge(from, to, true);
        emit connectionRemoved(from, to, true);
    } catch (...) {
        emit connectionRemoved(from, to, false);
        emit errorOccurred("Error al eliminar conexión");
    }
}

void GraphController::markEdgeClosed(int from, int to) {
    try {
        m_graph->markEdgeClosed(from, to, true, true);
        emit closureMarked(from, to, true);
    } catch (...) {
        emit errorOccurred("Error al marcar cierre");
    }
}

void GraphController::runBFS(int origin, int destination) {
    try {
        QVector<int> path = bfsSearch(origin, destination);
        if (path.isEmpty()) {
            emit pathNotFound("BFS");
        } else {
            double cost = 0.0;
            emit pathFound("BFS", path, cost);
            addReportEntry("BFS", origin, destination, path, cost);
        }
    } catch (...) {
        emit errorOccurred("Error al ejecutar BFS");
    }
}

void GraphController::runDFS(int origin, int destination) {
    try {
        QVector<int> path = dfsSearch(origin, destination);
        if (path.isEmpty()) {
            emit pathNotFound("DFS");
        } else {
            double cost = 0.0;
            emit pathFound("DFS", path, cost);
            addReportEntry("DFS", origin, destination, path, cost);
        }
    } catch (...) {
        emit errorOccurred("Error al ejecutar DFS");
    }
}

void GraphController::runDijkstra(int origin, int destination) {
    try {
        double cost = 0.0;
        QVector<int> path = dijkstraSearch(origin, destination, cost);
        if (path.isEmpty()) {
            emit pathNotFound("Dijkstra");
        } else {
            emit pathFound("Dijkstra", path, cost);
            addReportEntry("Dijkstra", origin, destination, path, cost);
        }
    } catch (...) {
        emit errorOccurred("Error al ejecutar Dijkstra");
    }
}

void GraphController::runFloydWarshall(int origin, int destination) {
    try {
        double cost = 0.0;
        QVector<int> path = floydWarshallSearch(origin, destination, cost);
        if (path.isEmpty()) {
            emit pathNotFound("Floyd-Warshall");
        } else {
            emit pathFound("Floyd-Warshall", path, cost);
            addReportEntry("Floyd-Warshall", origin, destination, path, cost);
        }
    } catch (...) {
        emit errorOccurred("Error al ejecutar Floyd-Warshall");
    }
}

void GraphController::runKruskal() {
    try {
        double totalCost = 0.0;
        QVector<QPair<int,int>> edges = kruskalMST(totalCost);
        if (edges.isEmpty()) {
            emit errorOccurred("No se pudo generar árbol de expansión mínima");
        } else {
            emit mstFound("Kruskal", edges, totalCost);
        }
    } catch (...) {
        emit errorOccurred("Error al ejecutar Kruskal");
    }
}

void GraphController::runPrim() {
    try {
        double totalCost = 0.0;
        QVector<QPair<int,int>> edges = primMST(totalCost);
        if (edges.isEmpty()) {
            emit errorOccurred("No se pudo generar árbol de expansión mínima");
        } else {
            emit mstFound("Prim", edges, totalCost);
        }
    } catch (...) {
        emit errorOccurred("Error al ejecutar Prim");
    }
}

void GraphController::generateReport() {
    emit reportGenerated(true);
}

void GraphController::addReportEntry(const QString& algorithm, int origin, int destination,
                                     const QVector<int>& path, double cost) {
    ReportManager::ReportEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.algorithm = algorithm;
    entry.originId = origin;
    entry.destinationId = destination;
    entry.path = path;
    entry.totalCost = cost;
    
    if (m_graph->hasStation(origin)) {
        entry.originName = m_graph->getStation(origin).getName();
    }
    if (m_graph->hasStation(destination)) {
        entry.destinationName = m_graph->getStation(destination).getName();
    }
    
    for (int id : path) {
        if (m_graph->hasStation(id)) {
            entry.pathNames.append(m_graph->getStation(id).getName());
        } else {
            entry.pathNames.append("");
        }
    }
    
    m_reportManager->addReport(entry);
}

// ========== ALGORITHM IMPLEMENTATIONS ==========

QVector<int> GraphController::bfsSearch(int origin, int destination) {
    QVector<int> path;
    if (!m_graph->hasStation(origin) || !m_graph->hasStation(destination)) {
        return path;
    }
    
    QQueue<int> queue;
    QSet<int> visited;
    QMap<int, int> parent;
    
    queue.enqueue(origin);
    visited.insert(origin);
    parent[origin] = -1;
    
    while (!queue.isEmpty()) {
        int current = queue.dequeue();
        
        if (current == destination) {
            // Reconstruct path
            int node = destination;
            while (node != -1) {
                path.prepend(node);
                node = parent[node];
            }
            return path;
        }
        
        QVector<Edge> edges = m_graph->getEdgesFrom(current);
        for (const Edge& edge : edges) {
            if (!edge.isClosed() && !visited.contains(edge.getTo())) {
                visited.insert(edge.getTo());
                parent[edge.getTo()] = current;
                queue.enqueue(edge.getTo());
            }
        }
    }
    
    return path;
}

QVector<int> GraphController::dfsSearch(int origin, int destination) {
    QVector<int> path;
    if (!m_graph->hasStation(origin) || !m_graph->hasStation(destination)) {
        return path;
    }
    
    QStack<int> stack;
    QSet<int> visited;
    QMap<int, int> parent;
    
    stack.push(origin);
    parent[origin] = -1;
    
    while (!stack.isEmpty()) {
        int current = stack.pop();
        
        if (visited.contains(current)) continue;
        visited.insert(current);
        
        if (current == destination) {
            // Reconstruct path
            int node = destination;
            while (node != -1) {
                path.prepend(node);
                node = parent[node];
            }
            return path;
        }
        
        QVector<Edge> edges = m_graph->getEdgesFrom(current);
        for (const Edge& edge : edges) {
            if (!edge.isClosed() && !visited.contains(edge.getTo())) {
                if (!parent.contains(edge.getTo())) {
                    parent[edge.getTo()] = current;
                }
                stack.push(edge.getTo());
            }
        }
    }
    
    return path;
}

QVector<int> GraphController::dijkstraSearch(int origin, int destination, double& cost) {
    QVector<int> path;
    cost = 0.0;
    
    if (!m_graph->hasStation(origin) || !m_graph->hasStation(destination)) {
        return path;
    }
    
    QMap<int, double> distances;
    QMap<int, int> parent;
    QSet<int> visited;
    
    // Initialize distances
    QVector<Station> allStations = m_graph->getAllStations();
    for (const Station& station : allStations) {
        distances[station.getId()] = std::numeric_limits<double>::infinity();
    }
    distances[origin] = 0.0;
    parent[origin] = -1;
    
    // Dijkstra's algorithm
    while (true) {
        // Find unvisited node with minimum distance
        int current = -1;
        double minDistance = std::numeric_limits<double>::infinity();
        
        for (auto it = distances.begin(); it != distances.end(); ++it) {
            if (!visited.contains(it.key()) && it.value() < minDistance) {
                minDistance = it.value();
                current = it.key();
            }
        }
        
        if (current == -1 || minDistance == std::numeric_limits<double>::infinity()) {
            break; // No more reachable nodes
        }
        
        visited.insert(current);
        
        if (current == destination) {
            break; // Found destination
        }
        
        // Update distances to neighbors
        QVector<Edge> edges = m_graph->getEdgesFrom(current);
        for (const Edge& edge : edges) {
            if (!edge.isClosed() && !visited.contains(edge.getTo())) {
                double newDistance = distances[current] + edge.getWeight();
                if (newDistance < distances[edge.getTo()]) {
                    distances[edge.getTo()] = newDistance;
                    parent[edge.getTo()] = current;
                }
            }
        }
    }
    
    // Reconstruct path
    if (visited.contains(destination) && distances[destination] != std::numeric_limits<double>::infinity()) {
        int node = destination;
        while (node != -1) {
            path.prepend(node);
            node = parent[node];
        }
        cost = distances[destination];
    }
    
    return path;
}

QVector<int> GraphController::floydWarshallSearch(int origin, int destination, double& cost) {
    QVector<int> path;
    cost = 0.0;
    
    if (!m_graph->hasStation(origin) || !m_graph->hasStation(destination)) {
        return path;
    }
    
    QVector<Station> stations = m_graph->getAllStations();
    int n = stations.size();
    
    // Create index mapping
    QMap<int, int> stationToIndex;
    QMap<int, int> indexToStation;
    for (int i = 0; i < n; ++i) {
        stationToIndex[stations[i].getId()] = i;
        indexToStation[i] = stations[i].getId();
    }
    
    // Initialize distance and next matrices
    QVector<QVector<double>> dist(n, QVector<double>(n, std::numeric_limits<double>::infinity()));
    QVector<QVector<int>> next(n, QVector<int>(n, -1));
    
    // Initialize diagonal
    for (int i = 0; i < n; ++i) {
        dist[i][i] = 0.0;
    }
    
    // Initialize edges
    for (const Station& station : stations) {
        int fromIdx = stationToIndex[station.getId()];
        QVector<Edge> edges = m_graph->getEdgesFrom(station.getId());
        for (const Edge& edge : edges) {
            if (!edge.isClosed() && stationToIndex.contains(edge.getTo())) {
                int toIdx = stationToIndex[edge.getTo()];
                dist[fromIdx][toIdx] = edge.getWeight();
                next[fromIdx][toIdx] = toIdx;
            }
        }
    }
    
    // Floyd-Warshall algorithm
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (dist[i][k] != std::numeric_limits<double>::infinity() &&
                    dist[k][j] != std::numeric_limits<double>::infinity() &&
                    dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }
    
    // Reconstruct path
    int originIdx = stationToIndex[origin];
    int destIdx = stationToIndex[destination];
    
    if (next[originIdx][destIdx] != -1) {
        path.append(origin);
        int current = originIdx;
        while (current != destIdx) {
            current = next[current][destIdx];
            path.append(indexToStation[current]);
        }
        cost = dist[originIdx][destIdx];
    }
    
    return path;
}

QVector<QPair<int,int>> GraphController::kruskalMST(double& totalCost) {
    QVector<QPair<int,int>> mstEdges;
    totalCost = 0.0;
    
    QVector<Edge> allEdges = m_graph->getAllEdges();
    QVector<Station> stations = m_graph->getAllStations();
    
    // Sort edges by weight
    std::sort(allEdges.begin(), allEdges.end(), [](const Edge& a, const Edge& b) {
        return a.getWeight() < b.getWeight();
    });
    
    // Union-Find structure
    QMap<int, int> parent;
    for (const Station& station : stations) {
        parent[station.getId()] = station.getId();
    }
    
    auto find = [&parent](int x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    };
    
    auto unite = [&parent, &find](int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        if (rootX != rootY) {
            parent[rootX] = rootY;
            return true;
        }
        return false;
    };
    
    // Kruskal's algorithm
    for (const Edge& edge : allEdges) {
        if (!edge.isClosed() && unite(edge.getFrom(), edge.getTo())) {
            mstEdges.append(qMakePair(edge.getFrom(), edge.getTo()));
            totalCost += edge.getWeight();
        }
    }
    
    return mstEdges;
}

QVector<QPair<int,int>> GraphController::primMST(double& totalCost) {
    QVector<QPair<int,int>> mstEdges;
    totalCost = 0.0;
    
    QVector<Station> stations = m_graph->getAllStations();
    if (stations.isEmpty()) return mstEdges;
    
    QSet<int> inMST;
    QMap<int, double> minWeight;
    QMap<int, int> parent;
    
    // Initialize
    int start = stations.first().getId();
    for (const Station& station : stations) {
        minWeight[station.getId()] = std::numeric_limits<double>::infinity();
        parent[station.getId()] = -1;
    }
    minWeight[start] = 0.0;
    
    // Prim's algorithm
    while (inMST.size() < stations.size()) {
        // Find minimum weight node not in MST
        int u = -1;
        double minW = std::numeric_limits<double>::infinity();
        for (const Station& station : stations) {
            int id = station.getId();
            if (!inMST.contains(id) && minWeight[id] < minW) {
                minW = minWeight[id];
                u = id;
            }
        }
        
        if (u == -1) break;
        
        inMST.insert(u);
        if (parent[u] != -1) {
            mstEdges.append(qMakePair(parent[u], u));
            totalCost += minWeight[u];
        }
        
        // Update weights of adjacent nodes
        QVector<Edge> edges = m_graph->getEdgesFrom(u);
        for (const Edge& edge : edges) {
            int v = edge.getTo();
            if (!edge.isClosed() && !inMST.contains(v) && edge.getWeight() < minWeight[v]) {
                minWeight[v] = edge.getWeight();
                parent[v] = u;
            }
        }
    }
    
    return mstEdges;
}
