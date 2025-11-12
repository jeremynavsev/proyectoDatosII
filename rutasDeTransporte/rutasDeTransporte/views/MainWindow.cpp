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
    applyModernStyle();
    setWindowTitle("RUTAS DE TRANSPORTE");
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
    
    // Connect tree tab with graph controller for station management
    m_treeTab->setGraphController(m_graphController);
    
    m_tabWidget->addTab(m_treeTab, "Gestión de Estaciones (BST)");
    m_tabWidget->addTab(m_graphTab, "Red de Transporte (Grafo)");
    
    setCentralWidget(m_tabWidget);
}

void MainWindow::displayInitialStations() {
    m_treeTab->showLoadedStations();
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
    m_reportDialog->refreshReports();  // Refrescar reportes antes de mostrar
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

void MainWindow::applyModernStyle() {
    // Modern color palette with professional look
    QString styleSheet = R"(
        QMainWindow {
            background-color: #f5f7fa;
        }
        
        QMenuBar {
            background-color: #2c3e50;
            color: #ecf0f1;
            padding: 4px;
            font-size: 10pt;
        }
        
        QMenuBar::item {
            background-color: transparent;
            padding: 6px 12px;
        }
        
        QMenuBar::item:selected {
            background-color: #34495e;
            border-radius: 4px;
        }
        
        QMenuBar::item:pressed {
            background-color: #1abc9c;
        }
        
        QMenu {
            background-color: #ecf0f1;
            color: #2c3e50;
            border: 1px solid #bdc3c7;
        }
        
        QMenu::item {
            padding: 6px 25px;
        }
        
        QMenu::item:selected {
            background-color: #1abc9c;
            color: white;
        }
        
        QTabWidget::pane {
            border: 1px solid #bdc3c7;
            background-color: white;
            border-radius: 4px;
        }
        
        QTabBar::tab {
            background-color: #ecf0f1;
            color: #2c3e50;
            border: 1px solid #bdc3c7;
            border-bottom: none;
            padding: 10px 20px;
            margin-right: 2px;
            font-weight: bold;
        }
        
        QTabBar::tab:selected {
            background-color: #1abc9c;
            color: white;
        }
        
        QTabBar::tab:hover {
            background-color: #16a085;
            color: white;
        }
        
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
            font-size: 9pt;
        }
        
        QPushButton:hover {
            background-color: #2980b9;
        }
        
        QPushButton:pressed {
            background-color: #21618c;
        }
        
        QPushButton:disabled {
            background-color: #95a5a6;
        }
        
        QTextEdit {
            background-color: #fefefe;
            border: 1px solid #bdc3c7;
            border-radius: 4px;
            padding: 4px;
            color: #2c3e50;
            font-family: 'Consolas', 'Courier New', monospace;
        }
        
        QLineEdit {
            background-color: white;
            border: 2px solid #bdc3c7;
            border-radius: 4px;
            padding: 6px;
            color: #2c3e50;
        }
        
        QLineEdit:focus {
            border: 2px solid #3498db;
        }
        
        QGraphicsView {
            background-color: #ffffff;
            border: 2px solid #bdc3c7;
            border-radius: 4px;
        }
        
        QLabel {
            color: #2c3e50;
            font-size: 9pt;
        }
        
        QMessageBox {
            background-color: white;
        }
        
        QScrollBar:vertical {
            background-color: #ecf0f1;
            width: 12px;
            margin: 0px;
        }
        
        QScrollBar::handle:vertical {
            background-color: #95a5a6;
            border-radius: 6px;
            min-height: 20px;
        }
        
        QScrollBar::handle:vertical:hover {
            background-color: #7f8c8d;
        }
        
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        
        QScrollBar:horizontal {
            background-color: #ecf0f1;
            height: 12px;
            margin: 0px;
        }
        
        QScrollBar::handle:horizontal {
            background-color: #95a5a6;
            border-radius: 6px;
            min-width: 20px;
        }
        
        QScrollBar::handle:horizontal:hover {
            background-color: #7f8c8d;
        }
        
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }
    )";
    
    setStyleSheet(styleSheet);
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
