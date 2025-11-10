#include "FileController.h"
#include "BinarySearch.h"
#include "Graph.h"
#include "ReportManager.h"
#include "Station.h"
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QDir>
#include <QSet>

FileController::FileController(QObject* parent) 
    : QObject(parent), m_tree(nullptr), m_graph(nullptr), 
      m_reportManager(nullptr), m_dataPath("data") {}

void FileController::setTree(BinarySearchTree* tree) { m_tree = tree; }
void FileController::setGraph(Graph* graph) { m_graph = graph; }
void FileController::setReportManager(ReportManager* reportManager) { m_reportManager = reportManager; }
void FileController::setDataPath(const QString& path) { m_dataPath = path; }

ReportManager* FileController::getReportManager() const { return m_reportManager; }

QString FileController::getStationsFilePath() const { return m_dataPath + "/estaciones.txt"; }
QString FileController::getRoutesFilePath() const { return m_dataPath + "/rutas.txt"; }
QString FileController::getClosuresFilePath() const { return m_dataPath + "/cierres.txt"; }
QString FileController::getReportsFilePath() const { return m_dataPath + "/reportes.txt"; }

bool FileController::loadAll() {
    try {
        loadStations();
        loadRoutes();
        loadClosures();
        loadReports();
        emit dataLoaded(true);
        return true;
    } catch (...) {
        emit dataLoaded(false);
        emit errorOccurred("Error al cargar datos");
        return false;
    }
}

bool FileController::saveAll() {
    try {
        saveStations();
        saveRoutes();
        saveClosures();
        saveReports();
        emit dataSaved(true);
        return true;
    } catch (...) {
        emit dataSaved(false);
        emit errorOccurred("Error al guardar datos");
        return false;
    }
}

void FileController::loadStations() {
    if (!m_tree || !m_graph) {
        emit errorOccurred("Tree o Graph no inicializados");
        return;
    }
    
    QFile file(getStationsFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit stationsLoaded(0);
        return;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    int count = 0;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#")) continue;
        
        QStringList parts = line.split(';');
        if (parts.size() >= 2) {
            bool ok;
            int id = parts[0].trimmed().toInt(&ok);
            if (ok) {
                QString name = parts[1].trimmed();
                Station station(id, name);
                m_tree->insert(station);
                m_graph->addStation(station);
                count++;
            }
        }
    }
    file.close();
    emit stationsLoaded(count);
}

void FileController::loadRoutes() {
    if (!m_graph) {
        emit errorOccurred("Graph no inicializado");
        return;
    }
    
    QFile file(getRoutesFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit routesLoaded(0);
        return;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    int count = 0;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#")) continue;
        
        QStringList parts = line.split(';');
        if (parts.size() >= 3) {
            bool ok1, ok2, ok3;
            int from = parts[0].trimmed().toInt(&ok1);
            int to = parts[1].trimmed().toInt(&ok2);
            double weight = parts[2].trimmed().toDouble(&ok3);
            if (ok1 && ok2 && ok3) {
                m_graph->addEdge(from, to, weight, true);
                count++;
            }
        }
    }
    file.close();
    emit routesLoaded(count);
}

void FileController::loadClosures() {
    if (!m_graph) {
        emit errorOccurred("Graph no inicializado");
        return;
    }
    
    QFile file(getClosuresFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit closuresLoaded(0);
        return;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    int count = 0;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#")) continue;
        
        QStringList parts = line.split(';');
        if (parts.size() >= 2) {
            bool ok1, ok2;
            int from = parts[0].trimmed().toInt(&ok1);
            int to = parts[1].trimmed().toInt(&ok2);
            if (ok1 && ok2) {
                m_graph->markEdgeClosed(from, to, true, true);
                count++;
            }
        }
    }
    file.close();
    emit closuresLoaded(count);
}

void FileController::loadReports() {
    if (!m_reportManager) return;
    m_reportManager->loadReports(getReportsFilePath());
}

void FileController::saveStations() {
    if (!m_tree) {
        emit errorOccurred("Tree no inicializado");
        return;
    }
    
    QDir dir;
    dir.mkpath(m_dataPath);
    
    QFile file(getStationsFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred("No se pudo guardar estaciones.txt");
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "# Estaciones\n# Formato: ID;Nombre\n\n";
    
    QVector<Station> stations = m_tree->getAllStations();
    for (const Station& station : stations) {
        out << station.getId() << ";" << station.getName() << "\n";
    }
    file.close();
}

void FileController::saveRoutes() {
    if (!m_graph) {
        emit errorOccurred("Graph no inicializado");
        return;
    }
    
    QDir dir;
    dir.mkpath(m_dataPath);
    
    QFile file(getRoutesFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred("No se pudo guardar rutas.txt");
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "# Rutas\n# Formato: ID_origen;ID_destino;peso\n\n";
    
    QSet<QString> written;
    QVector<Edge> edges = m_graph->getAllEdges();
    for (const Edge& edge : edges) {
        QString key = QString("%1-%2").arg(qMin(edge.getFrom(), edge.getTo()))
                                      .arg(qMax(edge.getFrom(), edge.getTo()));
        if (!written.contains(key)) {
            out << edge.getFrom() << ";" << edge.getTo() << ";" << edge.getWeight() << "\n";
            written.insert(key);
        }
    }
    file.close();
}

void FileController::saveClosures() {
    if (!m_graph) {
        emit errorOccurred("Graph no inicializado");
        return;
    }
    
    QDir dir;
    dir.mkpath(m_dataPath);
    
    QFile file(getClosuresFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred("No se pudo guardar cierres.txt");
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "# Rutas cerradas\n# Formato: ID_origen;ID_destino\n\n";
    
    QSet<QString> written;
    QVector<Edge> edges = m_graph->getAllEdges();
    for (const Edge& edge : edges) {
        if (edge.isClosed()) {
            QString key = QString("%1-%2").arg(qMin(edge.getFrom(), edge.getTo()))
                                          .arg(qMax(edge.getFrom(), edge.getTo()));
            if (!written.contains(key)) {
                out << edge.getFrom() << ";" << edge.getTo() << "\n";
                written.insert(key);
            }
        }
    }
    file.close();
}

void FileController::saveReports() {
    if (!m_reportManager) return;
    QDir dir;
    dir.mkpath(m_dataPath);
    m_reportManager->saveReports(getReportsFilePath());
}
