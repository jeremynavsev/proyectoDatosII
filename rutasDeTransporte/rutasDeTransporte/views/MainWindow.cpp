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
    m_reportDialog->refreshReports();  
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
    // Nature-inspired Green Palette - Fresh & Professional
    QString styleSheet = R"(
        QMainWindow {
            background-color: #c7e9c0;
        }
        
        QMenuBar {
            background-color: #2d6a4f;
            color: #ffffff;
            padding: 5px;
            font-size: 10pt;
            border: none;
        }
        
        QMenuBar::item {
            background-color: transparent;
            padding: 8px 16px;
            border-radius: 4px;
        }
        
        QMenuBar::item:selected {
            background-color: #40916c;
        }
        
        QMenuBar::item:pressed {
            background-color: #52b788;
        }
        
        QMenu {
            background-color: #c7e9c0;
            color: #1b4332;
            border: none;
            border-radius: 4px;
            padding: 4px;
        }
        
        QMenu::item {
            padding: 8px 30px;
            border-radius: 4px;
            margin: 2px 4px;
        }
        
        QMenu::item:selected {
            background-color: #d8f3dc;
            color: #2d6a4f;
        }
        
        QTabWidget::pane {
            border: none;
            background-color: #c7e9c0;
            border-radius: 8px;
            margin-top: -1px;
        }
        
        QTabBar::tab {
            background-color: #d8f3dc;
            color: #1b4332;
            border: none;
            padding: 12px 24px;
            margin-right: 4px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            font-weight: 500;
            font-size: 10pt;
        }
        
        QTabBar::tab:selected {
            background-color: #40916c;
            color: #ffffff;
        }
        
        QTabBar::tab:hover:!selected {
            background-color: #95d5b2;
            color: #1b4332;
        }
        
        QPushButton {
            background-color: #52b788;
            color: #ffffff;
            border: none;
            padding: 10px 20px;
            border-radius: 4px;
            font-weight: 500;
            font-size: 10pt;
        }
        
        QPushButton:hover {
            background-color: #40916c;
        }
        
        QPushButton:pressed {
            background-color: #2d6a4f;
        }
        
        QPushButton:disabled {
            background-color: #b7e4c7;
            color: #74c69d;
        }
        
        QPushButton#deleteButton {
            background-color: #e07a5f;
        }
        
        QPushButton#deleteButton:hover {
            background-color: #d9534f;
        }
        
        QPushButton#successButton {
            background-color: #74c69d;
            color: #ffffff;
        }
        
        QPushButton#successButton:hover {
            background-color: #52b788;
        }
        
        QTextEdit {
            background-color: #e5f5e0;
            border: 1px solid #e0e0e0;
            border-radius: 4px;
            padding: 8px;
            color: #212121;
            font-family: 'Roboto', 'Segoe UI', 'Consolas', monospace;
            selection-background-color: #b39ddb;
        }
        
        QLineEdit {
            background-color: #e5f5e0;
            border: none;
            border-bottom: 2px solid #e0e0e0;
            border-radius: 4px 4px 0px 0px;
            padding: 10px 8px;
            color: #212121;
            font-size: 10pt;
        }
        
        QLineEdit:focus {
            border-bottom: 2px solid #2d6a4f;
            background-color: #f5f5f5;
        }
        
        QLineEdit:hover {
            background-color: #fafafa;
        }
        
        QGraphicsView {
            background-color: #e5f5e0;
            border: 1px solid #e0e0e0;
            border-radius: 8px;
        }
        
        QLabel {
            color: #424242;
            font-size: 10pt;
        }
        
        QMessageBox {
            background-color: #fffff;
        }
        
        QMessageBox QPushButton {
            min-width: 80px;
        }
        
        QScrollBar:vertical {
            background-color: #c7e9c0;
            width: 14px;
            margin: 0px;
            border-radius: 7px;
        }
        
        QScrollBar::handle:vertical {
            background-color: #bdbdbd;
            border-radius: 7px;
            min-height: 30px;
            margin: 2px;
        }
        
        QScrollBar::handle:vertical:hover {
            background-color: #9e9e9e;
        }
        
        QScrollBar::handle:vertical:pressed {
            background-color: #757575;
        }
        
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        
        QScrollBar:horizontal {
            background-color: #c7e9c0;
            height: 14px;
            margin: 0px;
            border-radius: 7px;
        }
        
        QScrollBar::handle:horizontal {
            background-color: #bdbdbd;
            border-radius: 7px;
            min-width: 30px;
            margin: 2px;
        }
        
        QScrollBar::handle:horizontal:hover {
            background-color: #9e9e9e;
        }
        
        QScrollBar::handle:horizontal:pressed {
            background-color: #757575;
        }
        
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }
        
        QComboBox {
            background-color: #c7e9c0;
            border: 1px solid #e0e0e0;
            border-radius: 4px;
            padding: 6px 12px;
            color: #212121;
            font-size: 10pt;
        }
        
        QComboBox:hover {
            border: 1px solid #2d6a4f;
        }
        
        QComboBox::drop-down {
            border: none;
            padding-right: 8px;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #757575;
            margin-right: 5px;
        }
        
        QSpinBox, QDoubleSpinBox {
            background-color: #c7e9c0;
            border: 1px solid #e0e0e0;
            border-radius: 4px;
            padding: 6px;
            color: #212121;
        }
        
        QSpinBox:focus, QDoubleSpinBox:focus {
            border: 1px solid #2d6a4f;
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
