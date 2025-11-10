#include "Graph.h"

Graph::Graph() {}

void Graph::addStation(const Station& station) {
    m_stations[station.getId()] = station;
    if (!m_adjacencyList.contains(station.getId())) {
        m_adjacencyList[station.getId()] = QVector<Edge>();
    }
}

void Graph::addEdge(int from, int to, double weight, bool bidirectional) {
    if (!m_adjacencyList.contains(from)) {
        m_adjacencyList[from] = QVector<Edge>();
    }
    if (!m_adjacencyList.contains(to)) {
        m_adjacencyList[to] = QVector<Edge>();
    }
    
    m_adjacencyList[from].append(Edge(from, to, weight));
    if (bidirectional) {
        m_adjacencyList[to].append(Edge(to, from, weight));
    }
}

void Graph::removeEdge(int from, int to, bool bidirectional) {
    if (m_adjacencyList.contains(from)) {
        QVector<Edge>& edges = m_adjacencyList[from];
        for (int i = 0; i < edges.size(); ++i) {
            if (edges[i].getTo() == to) {
                edges.removeAt(i);
                break;
            }
        }
    }
    if (bidirectional && m_adjacencyList.contains(to)) {
        QVector<Edge>& edges = m_adjacencyList[to];
        for (int i = 0; i < edges.size(); ++i) {
            if (edges[i].getTo() == from) {
                edges.removeAt(i);
                break;
            }
        }
    }
}

void Graph::markEdgeClosed(int from, int to, bool closed, bool bidirectional) {
    if (m_adjacencyList.contains(from)) {
        QVector<Edge>& edges = m_adjacencyList[from];
        for (int i = 0; i < edges.size(); ++i) {
            if (edges[i].getTo() == to) {
                edges[i].setClosed(closed);
                break;
            }
        }
    }
    if (bidirectional && m_adjacencyList.contains(to)) {
        QVector<Edge>& edges = m_adjacencyList[to];
        for (int i = 0; i < edges.size(); ++i) {
            if (edges[i].getTo() == from) {
                edges[i].setClosed(closed);
                break;
            }
        }
    }
}

bool Graph::hasStation(int id) const {
    return m_stations.contains(id);
}

Station Graph::getStation(int id) const {
    if (m_stations.contains(id)) return m_stations[id];
    return Station();
}

QVector<Station> Graph::getAllStations() const {
    QVector<Station> result;
    for (auto it = m_stations.begin(); it != m_stations.end(); ++it) {
        result.append(it.value());
    }
    return result;
}

QVector<Edge> Graph::getEdgesFrom(int stationId) const {
    if (m_adjacencyList.contains(stationId)) {
        return m_adjacencyList[stationId];
    }
    return QVector<Edge>();
}

QVector<Edge> Graph::getAllEdges() const {
    QVector<Edge> result;
    for (auto it = m_adjacencyList.begin(); it != m_adjacencyList.end(); ++it) {
        result.append(it.value());
    }
    return result;
}

bool Graph::isEdgeClosed(int from, int to) const {
    if (m_adjacencyList.contains(from)) {
        const QVector<Edge>& edges = m_adjacencyList[from];
        for (const Edge& edge : edges) {
            if (edge.getTo() == to) return edge.isClosed();
        }
    }
    return false;
}

int Graph::getStationCount() const { return m_stations.size(); }

int Graph::getEdgeCount() const {
    int count = 0;
    for (auto it = m_adjacencyList.begin(); it != m_adjacencyList.end(); ++it) {
        count += it.value().size();
    }
    return count / 2;
}

void Graph::clear() {
    m_stations.clear();
    m_adjacencyList.clear();
}
