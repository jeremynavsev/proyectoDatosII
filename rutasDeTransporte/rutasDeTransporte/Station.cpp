#include "Station.h"

Station::Station() : m_id(0), m_name("") {}

Station::Station(int id, const QString& name) : m_id(id), m_name(name) {}

int Station::getId() const { return m_id; }

QString Station::getName() const { return m_name; }

void Station::setId(int id) { m_id = id; }

void Station::setName(const QString& name) { m_name = name; }

bool Station::operator<(const Station& other) const {
    return m_id < other.m_id;
}

bool Station::operator>(const Station& other) const {
    return m_id > other.m_id;
}

bool Station::operator==(const Station& other) const {
    return m_id == other.m_id;
}
