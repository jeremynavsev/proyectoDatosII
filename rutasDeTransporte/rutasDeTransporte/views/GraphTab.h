#ifndef GRAPHTAB_H
#define GRAPHTAB_H

#include "../Station.h"
#include "../Edge.h"
#include <QWidget>
#include <QMap>
#include <QVector>

class QPushButton;
class QTextEdit;
class QGraphicsView;
class QGraphicsScene;
class QGraphicsEllipseItem;
class QGraphicsTextItem;
class GraphController;

/**
 * @brief Tab for Graph operations and visualization
 */
class GraphTab : public QWidget {
    Q_OBJECT
    
public:
    explicit GraphTab(GraphController* controller, QWidget* parent = nullptr);
    
public slots:
    void onLoadMapClicked();
    void onAddEdgeClicked();
    void onRemoveEdgeClicked();
    void onMarkClosureClicked();
    void onReopenEdgeClicked();
    void onBFSClicked();
    void onDFSClicked();
    void onDijkstraClicked();
    void onFloydWarshallClicked();
    void onKruskalClicked();
    void onPrimClicked();
    void onGenerateReportClicked();
    
    void onMapLoaded(const QVector<Station>& stations, const QVector<Edge>& edges);
    void onPathFound(const QString& algorithm, const QVector<int>& path, double distance);
    void onPathNotFound(const QString& algorithm);
    void onError(const QString& message);
    
private:
    void setupUI();
    void appendOutput(const QString& text);
    void drawGraph();
    void drawPath(const QVector<int>& path);
    void clearScene();
    
    GraphController* m_controller;
    
    QPushButton* m_loadMapButton;
    QPushButton* m_addEdgeButton;
    QPushButton* m_removeEdgeButton;
    QPushButton* m_markClosureButton;
    QPushButton* m_reopenEdgeButton;
    QPushButton* m_bfsButton;
    QPushButton* m_dfsButton;
    QPushButton* m_dijkstraButton;
    QPushButton* m_floydButton;
    QPushButton* m_kruskalButton;
    QPushButton* m_primButton;
    QPushButton* m_reportButton;
    
    QGraphicsView* m_graphView;
    QGraphicsScene* m_graphScene;
    QTextEdit* m_outputText;
    
    QVector<Station> m_stations;
    QVector<Edge> m_edges;
    QMap<int, QGraphicsEllipseItem*> m_nodeItems;
    QMap<int, QGraphicsTextItem*> m_nodeTextItems;
};

#endif // GRAPHTAB_H
