#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTabWidget;
class TreeTab;
class GraphTab;
class TreeController;
class GraphController;
class FileController;
class ReportDialog;

/**
 * @brief Main application window
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(TreeController* treeController, 
               GraphController* graphController,
               FileController* fileController,
               QWidget* parent = nullptr);
    ~MainWindow();
    
protected:
    void closeEvent(QCloseEvent* event) override;
    
private slots:
    void loadData();
    void saveData();
    void showReports();
    void showAbout();
    
private:
    void setupUI();
    void createMenuBar();
    
    TreeController* m_treeController;
    GraphController* m_graphController;
    FileController* m_fileController;
    
    QTabWidget* m_tabWidget;
    TreeTab* m_treeTab;
    GraphTab* m_graphTab;
    ReportDialog* m_reportDialog;
};

#endif // MAINWINDOW_H
