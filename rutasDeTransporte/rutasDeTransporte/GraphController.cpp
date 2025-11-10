#include "GraphController.h"
#include <QQueue>
#include <QStack>
#include <QSet>

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

// ========== ALGORITHM STUBS - TODO: IMPLEMENT ==========

QVector<int> GraphController::bfsSearch(int origin, int destination) {
    // TODO: Implement BFS algorithm
    // Return empty path for now
    QVector<int> path;
    return path;
}

QVector<int> GraphController::dfsSearch(int origin, int destination) {
    // TODO: Implement DFS algorithm
    QVector<int> path;
    return path;
}

QVector<int> GraphController::dijkstraSearch(int origin, int destination, double& cost) {
    // TODO: Implement Dijkstra's algorithm
    QVector<int> path;
    cost = 0.0;
    return path;
}

QVector<int> GraphController::floydWarshallSearch(int origin, int destination, double& cost) {
    // TODO: Implement Floyd-Warshall algorithm
    QVector<int> path;
    cost = 0.0;
    return path;
}

QVector<QPair<int,int>> GraphController::kruskalMST(double& totalCost) {
    // TODO: Implement Kruskal's algorithm
    QVector<QPair<int,int>> edges;
    totalCost = 0.0;
    return edges;
}

QVector<QPair<int,int>> GraphController::primMST(double& totalCost) {
    // TODO: Implement Prim's algorithm
    QVector<QPair<int,int>> edges;
    totalCost = 0.0;
    return edges;
}
