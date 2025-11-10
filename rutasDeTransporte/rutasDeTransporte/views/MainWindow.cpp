#include "MainWindow.h"
#include "TreeTab.h"
#include "GraphTab.h"
#include "ReportDialog.h"
#include "../TreeController.h"
#include "../GraphController.h"
#include "../FileController.h"
#include <QTabWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QCloseEvent>
#include <QApplication>

MainWindow::MainWindow(TreeController* treeController, 
                       GraphController* graphController,
                       FileController* fileController,
                       QWidget* parent)
    : QMainWindow(parent),
      m_treeController(treeController),
      m_graphController(graphController),
      m_fileController(fileController),
      m_reportDialog(nullptr) {
    setupUI();
    createMenuBar();
    setWindowTitle("RUTAS DE TRANSPORTE 1.0");
    resize(1000, 700);
}

MainWindow::~MainWindow() {
    if (m_reportDialog) {
        delete m_reportDialog;
    }
}

void MainWindow::setupUI() {
    m_tabWidget = new QTabWidget(this);
    
    m_treeTab = new TreeTab(m_treeController, this);
    m_graphTab = new GraphTab(m_graphController, this);
    
    m_tabWidget->addTab(m_treeTab, "Gestión de Estaciones (BST)");
    m_tabWidget->addTab(m_graphTab, "Red de Transporte (Grafo)");
    
    setCentralWidget(m_tabWidget);
}

void MainWindow::createMenuBar() {
    QMenuBar* menuBar = new QMenuBar(this);
    
    QMenu* fileMenu = menuBar->addMenu("Archivo");
    QAction* loadAction = fileMenu->addAction("Cargar Datos");
    QAction* saveAction = fileMenu->addAction("Guardar Datos");
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction("Salir");
    
    QMenu* reportMenu = menuBar->addMenu("Reportes");
    QAction* viewReportsAction = reportMenu->addAction("Ver Reportes");
    
    QMenu* helpMenu = menuBar->addMenu("Ayuda");
    QAction* aboutAction = helpMenu->addAction("Acerca de");
    
    connect(loadAction, &QAction::triggered, this, &MainWindow::loadData);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveData);
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);
    connect(viewReportsAction, &QAction::triggered, this, &MainWindow::showReports);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    
    setMenuBar(menuBar);
}

void MainWindow::loadData() {
    bool success = m_fileController->loadAll();
    if (success) {
        QMessageBox::information(this, "Éxito", "Datos cargados correctamente");
        m_graphController->loadMap();
    } else {
        QMessageBox::warning(this, "Error", "No se pudieron cargar todos los datos");
    }
}

void MainWindow::saveData() {
    bool success = m_fileController->saveAll();
    if (success) {
        QMessageBox::information(this, "Éxito", "Datos guardados correctamente");
    } else {
        QMessageBox::warning(this, "Error", "No se pudieron guardar todos los datos");
    }
}

void MainWindow::showReports() {
    if (!m_reportDialog) {
        m_reportDialog = new ReportDialog(m_fileController->getReportManager(), this);
    }
    m_reportDialog->exec();
}

void MainWindow::showAbout() {
    QMessageBox::about(this, "Acerca de RUTAS DE TRANSPORTE 1.0",
        "<h3>RUTAS DE TRANSPORTE 1.0</h3>"
        "<p>Sistema de gestión de rutas de transporte</p>"
        "<p><b>Características:</b></p>"
        "<ul>"
        "<li>Árbol Binario de Búsqueda para gestión de estaciones</li>"
        "<li>Grafo ponderado para red de transporte</li>"
        "<li>Algoritmos: BFS, DFS, Dijkstra, Floyd-Warshall, Kruskal, Prim</li>"
        "<li>Visualización gráfica de la red</li>"
        "<li>Generación de reportes</li>"
        "</ul>"
        "<p><i>Desarrollado con Qt Widgets en C++</i></p>");
}

void MainWindow::closeEvent(QCloseEvent* event) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Salir", "¿Desea guardar los datos antes de salir?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    
    if (reply == QMessageBox::Cancel) {
        event->ignore();
        return;
    }
    
    if (reply == QMessageBox::Yes) {
        m_fileController->saveAll();
    }
    
    event->accept();
}
