#ifndef STATION_H
#define STATION_H

#include <QString>
#include <QMetaType>

class Station {
public:
    Station();
    Station(int id, const QString& name);
    
    int getId() const;
    QString getName() const;
    void setId(int id);
    void setName(const QString& name);
    
    bool operator<(const Station& other) const;
    bool operator>(const Station& other) const;
    bool operator==(const Station& other) const;
    
private:
    int m_id;
    QString m_name;
};

Q_DECLARE_METATYPE(Station)

#endif // STATION_H
