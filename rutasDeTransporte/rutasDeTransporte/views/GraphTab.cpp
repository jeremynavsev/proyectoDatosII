#include "GraphTab.h"
#include "../GraphController.h"
#include "../Station.h"
#include "../Edge.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QPen>
#include <QBrush>
#include <QtMath>

GraphTab::GraphTab(GraphController* controller, QWidget* parent)
    : QWidget(parent), m_controller(controller) {
    setupUI();
    
    connect(m_controller, &GraphController::mapLoaded, this, &GraphTab::onMapLoaded);
    connect(m_controller, &GraphController::pathFound, this, &GraphTab::onPathFound);
    connect(m_controller, &GraphController::pathNotFound, this, &GraphTab::onPathNotFound);
    connect(m_controller, &GraphController::errorOccurred, this, &GraphTab::onError);
}

void GraphTab::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout* buttonLayout1 = new QHBoxLayout();
    m_loadMapButton = new QPushButton("Cargar Mapa", this);
    m_addEdgeButton = new QPushButton("Agregar Conexión", this);
    m_removeEdgeButton = new QPushButton("Eliminar Conexión", this);
    m_markClosureButton = new QPushButton("⚠ Marcar Accidente", this);
    m_reopenEdgeButton = new QPushButton("✓ Reabrir Ruta", this);
    buttonLayout1->addWidget(m_loadMapButton);
    buttonLayout1->addWidget(m_addEdgeButton);
    buttonLayout1->addWidget(m_removeEdgeButton);
    buttonLayout1->addWidget(m_markClosureButton);
    buttonLayout1->addWidget(m_reopenEdgeButton);
    mainLayout->addLayout(buttonLayout1);
    
    QHBoxLayout* buttonLayout2 = new QHBoxLayout();
    m_bfsButton = new QPushButton("BFS", this);
    m_dfsButton = new QPushButton("DFS", this);
    m_dijkstraButton = new QPushButton("Dijkstra", this);
    m_floydButton = new QPushButton("Floyd-Warshall", this);
    buttonLayout2->addWidget(m_bfsButton);
    buttonLayout2->addWidget(m_dfsButton);
    buttonLayout2->addWidget(m_dijkstraButton);
    buttonLayout2->addWidget(m_floydButton);
    mainLayout->addLayout(buttonLayout2);
    
    QHBoxLayout* buttonLayout3 = new QHBoxLayout();
    m_kruskalButton = new QPushButton("Kruskal", this);
    m_primButton = new QPushButton("Prim", this);
    m_reportButton = new QPushButton("Generar Reporte", this);
    buttonLayout3->addWidget(m_kruskalButton);
    buttonLayout3->addWidget(m_primButton);
    buttonLayout3->addWidget(m_reportButton);
    buttonLayout3->addStretch();
    mainLayout->addLayout(buttonLayout3);
    
    m_graphScene = new QGraphicsScene(this);
    m_graphView = new QGraphicsView(m_graphScene, this);
    m_graphView->setRenderHint(QPainter::Antialiasing);
    m_graphView->setMinimumHeight(300);
    mainLayout->addWidget(m_graphView, 2);
    
    m_outputText = new QTextEdit(this);
    m_outputText->setReadOnly(true);
    m_outputText->setFont(QFont("Courier New", 9));
    m_outputText->setMaximumHeight(150);
    mainLayout->addWidget(m_outputText, 1);
    
    connect(m_loadMapButton, &QPushButton::clicked, this, &GraphTab::onLoadMapClicked);
    connect(m_addEdgeButton, &QPushButton::clicked, this, &GraphTab::onAddEdgeClicked);
    connect(m_removeEdgeButton, &QPushButton::clicked, this, &GraphTab::onRemoveEdgeClicked);
    connect(m_markClosureButton, &QPushButton::clicked, this, &GraphTab::onMarkClosureClicked);
    connect(m_reopenEdgeButton, &QPushButton::clicked, this, &GraphTab::onReopenEdgeClicked);
    connect(m_bfsButton, &QPushButton::clicked, this, &GraphTab::onBFSClicked);
    connect(m_dfsButton, &QPushButton::clicked, this, &GraphTab::onDFSClicked);
    connect(m_dijkstraButton, &QPushButton::clicked, this, &GraphTab::onDijkstraClicked);
    connect(m_floydButton, &QPushButton::clicked, this, &GraphTab::onFloydWarshallClicked);
    connect(m_kruskalButton, &QPushButton::clicked, this, &GraphTab::onKruskalClicked);
    connect(m_primButton, &QPushButton::clicked, this, &GraphTab::onPrimClicked);
    connect(m_reportButton, &QPushButton::clicked, this, &GraphTab::onGenerateReportClicked);
}

void GraphTab::appendOutput(const QString& text) {
    m_outputText->append(text);
}

void GraphTab::clearScene() {
    m_graphScene->clear();
    m_nodeItems.clear();
    m_nodeTextItems.clear();
}

void GraphTab::drawGraph() {
    clearScene();
    
    if (m_stations.isEmpty()) {
        return;
    }
    
    const double centerX = 0;
    const double centerY = 0;
    const double radius = 200;
    const int count = m_stations.size();
    
    // FIRST: Create a map of station positions
    QMap<int, QPointF> stationPositions;
    for (int i = 0; i < count; ++i) {
        const Station& station = m_stations[i];
        double angle = 2 * M_PI * i / count;
        double x = centerX + radius * qCos(angle);
        double y = centerY + radius * qSin(angle);
        stationPositions[station.getId()] = QPointF(x, y);
    }
    
    // SECOND: Draw edges (behind nodes)
    for (const Edge& edge : m_edges) {
        int fromId = edge.getFrom();
        int toId = edge.getTo();
        
        // Check if both stations exist in positions map
        if (!stationPositions.contains(fromId) || !stationPositions.contains(toId)) {
            continue;
        }
        
        QPointF pos1 = stationPositions[fromId];
        QPointF pos2 = stationPositions[toId];
        
        QPen linePen;
        if (edge.isClosed()) {
            // Closed edges are RED and DASHED with thicker line
            linePen = QPen(QColor(220, 50, 50), 4);
            linePen.setStyle(Qt::DashLine);
        } else {
            // Open edges are blue-gray
            linePen = QPen(QColor(100, 149, 237), 2);
        }
        
        QGraphicsLineItem* line = m_graphScene->addLine(pos1.x(), pos1.y(), pos2.x(), pos2.y(), linePen);
        line->setZValue(1);
        
        // Draw weight label
        QGraphicsTextItem* weightText = m_graphScene->addText(QString::number(edge.getWeight(), 'f', 1));
        weightText->setPos((pos1.x() + pos2.x()) / 2 - 10, (pos1.y() + pos2.y()) / 2 - 10);
        weightText->setZValue(2);
        weightText->setDefaultTextColor(edge.isClosed() ? QColor(220, 50, 50) : QColor(50, 50, 50));
        QFont weightFont = weightText->font();
        weightFont.setPointSize(8);
        weightFont.setBold(true);
        weightText->setFont(weightFont);
    }
    
    // THIRD: Draw nodes (on top of edges)
    for (int i = 0; i < count; ++i) {
        const Station& station = m_stations[i];
        QPointF pos = stationPositions[station.getId()];
        double x = pos.x();
        double y = pos.y();
        
        QGraphicsEllipseItem* ellipse = m_graphScene->addEllipse(
            x - 25, y - 25, 50, 50,
            QPen(QColor(70, 70, 70), 2.5),
            QBrush(QColor(240, 248, 255)));
        ellipse->setZValue(2);
        m_nodeItems[station.getId()] = ellipse;
        
        QGraphicsTextItem* text = m_graphScene->addText(
            QString("%1").arg(station.getId()));
        text->setDefaultTextColor(QColor(40, 40, 40));
        QFont font = text->font();
        font.setPointSize(10);
        font.setBold(true);
        text->setFont(font);
        text->setPos(x - 10, y - 10);
        text->setZValue(3);
        m_nodeTextItems[station.getId()] = text;
        
        // Add station name below
        QGraphicsTextItem* nameText = m_graphScene->addText(station.getName());
        QFont nameFont = nameText->font();
        nameFont.setPointSize(7);
        nameText->setFont(nameFont);
        nameText->setDefaultTextColor(QColor(80, 80, 80));
        nameText->setPos(x - 30, y + 20);
        nameText->setZValue(3);
    }
    
    m_graphView->fitInView(m_graphScene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void GraphTab::drawPath(const QVector<int>& path) {
    if (path.size() < 2) return;
    
    // Highlight path nodes
    for (int id : path) {
        if (m_nodeItems.contains(id)) {
            QGraphicsEllipseItem* item = m_nodeItems[id];
            item->setBrush(QBrush(QColor(255, 223, 0))); // Golden yellow
            item->setPen(QPen(QColor(218, 165, 32), 3));
        }
    }
    
    // Highlight path edges
    for (int i = 0; i < path.size() - 1; ++i) {
        int fromId = path[i];
        int toId = path[i + 1];
        
        if (!m_nodeItems.contains(fromId) || !m_nodeItems.contains(toId)) {
            continue;
        }
        
        // Calculate positions
        int fromIdx = -1, toIdx = -1;
        for (int j = 0; j < m_stations.size(); ++j) {
            if (m_stations[j].getId() == fromId) fromIdx = j;
            if (m_stations[j].getId() == toId) toIdx = j;
        }
        
        if (fromIdx == -1 || toIdx == -1) continue;
        
        const double centerX = 0;
        const double centerY = 0;
        const double radius = 200;
        const int count = m_stations.size();
        
        double angle1 = 2 * M_PI * fromIdx / count;
        double x1 = centerX + radius * qCos(angle1);
        double y1 = centerY + radius * qSin(angle1);
        
        double angle2 = 2 * M_PI * toIdx / count;
        double x2 = centerX + radius * qCos(angle2);
        double y2 = centerY + radius * qSin(angle2);
        
        // Check if edge is closed
        bool isClosed = false;
        for (const Edge& edge : m_edges) {
            if ((edge.getFrom() == fromId && edge.getTo() == toId) ||
                (edge.getFrom() == toId && edge.getTo() == fromId)) {
                isClosed = edge.isClosed();
                break;
            }
        }
        
        // Draw highlighted path edge
        QPen pathPen;
        if (isClosed) {
            // Closed path: Show in BRIGHT RED to indicate collision/accident
            pathPen = QPen(QColor(255, 0, 0), 6);
            pathPen.setStyle(Qt::DashDotLine);
        } else {
            // Open path: Show in BRIGHT GREEN to indicate valid route
            pathPen = QPen(QColor(50, 205, 50), 6);
        }
        
        QGraphicsLineItem* pathLine = m_graphScene->addLine(x1, y1, x2, y2, pathPen);
        pathLine->setZValue(4);
        
        // Add arrow to show direction
        double angle = qAtan2(y2 - y1, x2 - x1);
        double arrowSize = 15;
        QPointF arrowP1 = QPointF(x2 - arrowSize * qCos(angle - M_PI / 6),
                                   y2 - arrowSize * qSin(angle - M_PI / 6));
        QPointF arrowP2 = QPointF(x2 - arrowSize * qCos(angle + M_PI / 6),
                                   y2 - arrowSize * qSin(angle + M_PI / 6));
        
        QGraphicsLineItem* arrow1 = m_graphScene->addLine(x2, y2, arrowP1.x(), arrowP1.y(), pathPen);
        QGraphicsLineItem* arrow2 = m_graphScene->addLine(x2, y2, arrowP2.x(), arrowP2.y(), pathPen);
        arrow1->setZValue(4);
        arrow2->setZValue(4);
    }
    
    // Highlight start and end nodes specially
    if (path.size() >= 2) {
        if (m_nodeItems.contains(path.first())) {
            m_nodeItems[path.first()]->setBrush(QBrush(QColor(144, 238, 144))); // Light green for start
        }
        if (m_nodeItems.contains(path.last())) {
            m_nodeItems[path.last()]->setBrush(QBrush(QColor(255, 165, 0))); // Orange for end
        }
    }
}

void GraphTab::onLoadMapClicked() {
    m_controller->loadMap();
}

void GraphTab::onAddEdgeClicked() {
    bool ok;
    int fromId = QInputDialog::getInt(this, "Agregar Conexión", "ID estación origen:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    int toId = QInputDialog::getInt(this, "Agregar Conexión", "ID estación destino:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    double weight = QInputDialog::getDouble(this, "Agregar Conexión", "Distancia (km):", 0, 0, 10000, 2, &ok);
    if (!ok) return;
    
    m_controller->addEdge(fromId, toId, weight);
    m_controller->loadMap();
}

void GraphTab::onRemoveEdgeClicked() {
    bool ok;
    int fromId = QInputDialog::getInt(this, "Eliminar Conexión", "ID estación origen:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    int toId = QInputDialog::getInt(this, "Eliminar Conexión", "ID estación destino:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    
    m_controller->removeEdge(fromId, toId);
    m_controller->loadMap();
}

void GraphTab::onMarkClosureClicked() {
    bool ok;
    int fromId = QInputDialog::getInt(this, "Marcar Cierre", "ID estación origen:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    int toId = QInputDialog::getInt(this, "Marcar Cierre", "ID estación destino:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    
    m_controller->markEdgeClosed(fromId, toId);
    m_controller->loadMap();
    
    // Show message about closure
    appendOutput(QString("⚠ ACCIDENTE: Ruta cerrada entre estaciones %1 y %2").arg(fromId).arg(toId));
    appendOutput("  Se recomienda recalcular rutas alternativas usando Dijkstra");
}

void GraphTab::onReopenEdgeClicked() {
    bool ok;
    int fromId = QInputDialog::getInt(this, "Reabrir Ruta", "ID estación origen:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    int toId = QInputDialog::getInt(this, "Reabrir Ruta", "ID estación destino:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    
    // Mark edge as open (not closed)
    m_controller->getGraph()->markEdgeClosed(fromId, toId, false, true);
    m_controller->loadMap();
    
    appendOutput(QString("✓ Ruta reabierta entre estaciones %1 y %2").arg(fromId).arg(toId));
}

void GraphTab::onBFSClicked() {
    bool ok;
    int startId = QInputDialog::getInt(this, "BFS", "ID estación inicial:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    int endId = QInputDialog::getInt(this, "BFS", "ID estación final:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    
    m_controller->runBFS(startId, endId);
}

void GraphTab::onDFSClicked() {
    bool ok;
    int startId = QInputDialog::getInt(this, "DFS", "ID estación inicial:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    int endId = QInputDialog::getInt(this, "DFS", "ID estación final:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    
    m_controller->runDFS(startId, endId);
}

void GraphTab::onDijkstraClicked() {
    bool ok;
    int startId = QInputDialog::getInt(this, "Dijkstra", "ID estación inicial:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    int endId = QInputDialog::getInt(this, "Dijkstra", "ID estación final:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    
    m_controller->runDijkstra(startId, endId);
}

void GraphTab::onFloydWarshallClicked() {
    bool ok;
    int startId = QInputDialog::getInt(this, "Floyd-Warshall", "ID estación inicial:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    int endId = QInputDialog::getInt(this, "Floyd-Warshall", "ID estación final:", 0, 0, 99999, 1, &ok);
    if (!ok) return;
    
    m_controller->runFloydWarshall(startId, endId);
}

void GraphTab::onKruskalClicked() {
    m_controller->runKruskal();
    appendOutput("Kruskal ejecutado (verifica reportes)");
}

void GraphTab::onPrimClicked() {
    m_controller->runPrim();
    appendOutput("Prim ejecutado (verifica reportes)");
}

void GraphTab::onGenerateReportClicked() {
    m_controller->generateReport();
    appendOutput("Reporte generado");
}

void GraphTab::onMapLoaded(const QVector<Station>& stations, const QVector<Edge>& edges) {
    m_stations = stations;
    m_edges = edges;
    drawGraph();
    appendOutput(QString("✓ Mapa cargado: %1 estaciones, %2 conexiones")
                 .arg(stations.size()).arg(edges.size()));
}

void GraphTab::onPathFound(const QString& algorithm, const QVector<int>& path, double distance) {
    QString pathStr;
    for (int i = 0; i < path.size(); ++i) {
        pathStr += QString::number(path[i]);
        if (i < path.size() - 1) pathStr += " → ";
    }
    appendOutput(QString("✓ %1: %2 (distancia: %3 km)")
                 .arg(algorithm).arg(pathStr).arg(distance, 0, 'f', 2));
    drawPath(path);
}

void GraphTab::onPathNotFound(const QString& algorithm) {
    appendOutput(QString("✗ %1: No se encontró ruta").arg(algorithm));
}

void GraphTab::onError(const QString& message) {
    appendOutput(QString("ERROR: %1").arg(message));
}
