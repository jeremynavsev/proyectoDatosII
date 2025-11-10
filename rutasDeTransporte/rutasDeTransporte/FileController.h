#ifndef FILECONTROLLER_H
#define FILECONTROLLER_H

#include <QObject>
#include <QString>

class BinarySearchTree;
class Graph;
class ReportManager;

/**
 * @brief Controller for file I/O operations
 */
class FileController : public QObject {
    Q_OBJECT
    
public:
    explicit FileController(QObject* parent = nullptr);
    
    void setTree(BinarySearchTree* tree);
    void setGraph(Graph* graph);
    void setReportManager(ReportManager* reportManager);
    void setDataPath(const QString& path);
    
    ReportManager* getReportManager() const;
    
public slots:
    bool loadAll();
    bool saveAll();
    void loadStations();
    void loadRoutes();
    void loadClosures();
    void loadReports();
    void saveStations();
    void saveRoutes();
    void saveClosures();
    void saveReports();
    
signals:
    void dataLoaded(bool success);
    void dataSaved(bool success);
    void stationsLoaded(int count);
    void routesLoaded(int count);
    void closuresLoaded(int count);
    void errorOccurred(const QString& message);
    
private:
    BinarySearchTree* m_tree;
    Graph* m_graph;
    ReportManager* m_reportManager;
    QString m_dataPath;
    
    QString getStationsFilePath() const;
    QString getRoutesFilePath() const;
    QString getClosuresFilePath() const;
    QString getReportsFilePath() const;
};

#endif // FILECONTROLLER_H
