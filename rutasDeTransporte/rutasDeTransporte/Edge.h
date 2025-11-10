#ifndef EDGE_H
#define EDGE_H

#include <QMetaType>

/**
 * @brief Represents a weighted edge in the transport graph
 */
class Edge {
public:
    Edge();
    Edge(int from, int to, double weight);
    
    int getFrom() const;
    int getTo() const;
    double getWeight() const;
    bool isClosed() const;
    
    void setFrom(int from);
    void setTo(int to);
    void setWeight(double weight);
    void setClosed(bool closed);
    
private:
    int m_from;
    int m_to;
    double m_weight;
    bool m_closed;
};

Q_DECLARE_METATYPE(Edge)

#endif // EDGE_H
