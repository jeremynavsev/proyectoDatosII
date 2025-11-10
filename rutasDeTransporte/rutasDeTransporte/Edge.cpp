#include "Edge.h"

Edge::Edge() : m_from(0), m_to(0), m_weight(0.0), m_closed(false) {}

Edge::Edge(int from, int to, double weight) 
    : m_from(from), m_to(to), m_weight(weight), m_closed(false) {}

int Edge::getFrom() const { return m_from; }
int Edge::getTo() const { return m_to; }
double Edge::getWeight() const { return m_weight; }
bool Edge::isClosed() const { return m_closed; }

void Edge::setFrom(int from) { m_from = from; }
void Edge::setTo(int to) { m_to = to; }
void Edge::setWeight(double weight) { m_weight = weight; }
void Edge::setClosed(bool closed) { m_closed = closed; }
