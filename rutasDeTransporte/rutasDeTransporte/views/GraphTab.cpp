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
    m_markClosureButton = new QPushButton("Marcar Cierre", this);
    buttonLayout1->addWidget(m_loadMapButton);
    buttonLayout1->addWidget(m_addEdgeButton);
    buttonLayout1->addWidget(m_removeEdgeButton);
    buttonLayout1->addWidget(m_markClosureButton);
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
    
    for (int i = 0; i < count; ++i) {
        const Station& station = m_stations[i];
        double angle = 2 * M_PI * i / count;
        double x = centerX + radius * qCos(angle);
        double y = centerY + radius * qSin(angle);
        
        QGraphicsEllipseItem* ellipse = m_graphScene->addEllipse(
            x - 25, y - 25, 50, 50,
            QPen(Qt::black, 2),
            QBrush(Qt::lightGray));
        ellipse->setZValue(2);
        m_nodeItems[station.getId()] = ellipse;
        
        QGraphicsTextItem* text = m_graphScene->addText(
            QString("%1\n%2").arg(station.getId()).arg(station.getName()));
        text->setPos(x - 20, y - 15);
        text->setZValue(3);
        m_nodeTextItems[station.getId()] = text;
    }
    
    for (const Edge& edge : m_edges) {
        if (!m_nodeItems.contains(edge.getFrom()) || !m_nodeItems.contains(edge.getTo())) {
            continue;
        }
        
        QGraphicsEllipseItem* fromItem = m_nodeItems[edge.getFrom()];
        QGraphicsEllipseItem* toItem = m_nodeItems[edge.getTo()];
        
        double x1 = fromItem->rect().center().x();
        double y1 = fromItem->rect().center().y();
        double x2 = toItem->rect().center().x();
        double y2 = toItem->rect().center().y();
        
        QPen linePen(edge.isClosed() ? Qt::red : Qt::blue, 2);
        if (edge.isClosed()) {
            linePen.setStyle(Qt::DashLine);
        }
        
        QGraphicsLineItem* line = m_graphScene->addLine(x1, y1, x2, y2, linePen);
        line->setZValue(1);
        
        QGraphicsTextItem* weightText = m_graphScene->addText(QString::number(edge.getWeight()));
        weightText->setPos((x1 + x2) / 2, (y1 + y2) / 2);
        weightText->setZValue(2);
        weightText->setDefaultTextColor(edge.isClosed() ? Qt::red : Qt::darkBlue);
    }
    
    m_graphView->fitInView(m_graphScene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void GraphTab::drawPath(const QVector<int>& path) {
    if (path.size() < 2) return;
    
    for (int i = 0; i < path.size() - 1; ++i) {
        int fromId = path[i];
        int toId = path[i + 1];
        
        if (!m_nodeItems.contains(fromId) || !m_nodeItems.contains(toId)) {
            continue;
        }
        
        QGraphicsEllipseItem* fromItem = m_nodeItems[fromId];
        QGraphicsEllipseItem* toItem = m_nodeItems[toId];
        
        fromItem->setBrush(QBrush(Qt::yellow));
        toItem->setBrush(QBrush(Qt::yellow));
        
        double x1 = fromItem->rect().center().x();
        double y1 = fromItem->rect().center().y();
        double x2 = toItem->rect().center().x();
        double y2 = toItem->rect().center().y();
        
        m_graphScene->addLine(x1, y1, x2, y2, QPen(Qt::green, 4))->setZValue(4);
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
