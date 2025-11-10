#ifndef GRAPH_H
#define GRAPH_H

#include "Edge.h"
#include "Station.h"
#include <QVector>
#include <QMap>

/**
 * @brief Weighted graph representing the transport network
 */
class Graph {
public:
    Graph();
    
    void addStation(const Station& station);
    void addEdge(int from, int to, double weight, bool bidirectional = true);
    void removeEdge(int from, int to, bool bidirectional = true);
    void markEdgeClosed(int from, int to, bool closed, bool bidirectional = true);
    
    bool hasStation(int id) const;
    Station getStation(int id) const;
    QVector<Station> getAllStations() const;
    QVector<Edge> getEdgesFrom(int stationId) const;
    QVector<Edge> getAllEdges() const;
    bool isEdgeClosed(int from, int to) const;
    
    int getStationCount() const;
    int getEdgeCount() const;
    void clear();
    
private:
    QMap<int, Station> m_stations;
    QMap<int, QVector<Edge>> m_adjacencyList;
};

#endif // GRAPH_H
