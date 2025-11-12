#include "TreeTab.h"
#include "../TreeController.h"
#include "../GraphController.h"
#include "../Station.h"
#include "../Edge.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

TreeTab::TreeTab(TreeController* controller, QWidget* parent)
    : QWidget(parent), m_controller(controller), m_graphController(nullptr) {
    setupUI();
    
    connect(m_controller, &TreeController::stationInserted, this, &TreeTab::onStationInserted);
    connect(m_controller, &TreeController::stationDeleted, this, &TreeTab::onStationDeleted);
    connect(m_controller, &TreeController::stationFound, this, &TreeTab::onStationFound);
    connect(m_controller, &TreeController::stationNotFound, this, &TreeTab::onStationNotFound);
    connect(m_controller, &TreeController::traversalResult, this, &TreeTab::onTraversalResult);
    connect(m_controller, &TreeController::traversalsExported, this, &TreeTab::onTraversalsExported);
    connect(m_controller, &TreeController::errorOccurred, this, &TreeTab::onError);
}

void TreeTab::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QGroupBox* inputGroup = new QGroupBox("", this);
    QGridLayout* inputLayout = new QGridLayout(inputGroup);
    
    QLabel* idLabel = new QLabel("ID:", this);
    m_idInput = new QLineEdit(this);
    m_idInput->setPlaceholderText("ID de estación");
    
    QLabel* nameLabel = new QLabel("Nombre:", this);
    m_nameInput = new QLineEdit(this);
    m_nameInput->setPlaceholderText("Nombre de estación");
    
    inputLayout->addWidget(idLabel, 0, 0);
    inputLayout->addWidget(m_idInput, 0, 1);
    inputLayout->addWidget(nameLabel, 1, 0);
    inputLayout->addWidget(m_nameInput, 1, 1);
    mainLayout->addWidget(inputGroup);
    
    QHBoxLayout* buttonLayout1 = new QHBoxLayout();
    m_insertButton = new QPushButton("Insertar Estación", this);
    m_deleteButton = new QPushButton("Eliminar Estación", this);
    m_searchButton = new QPushButton("Buscar Estación", this);
    buttonLayout1->addWidget(m_insertButton);
    buttonLayout1->addWidget(m_deleteButton);
    buttonLayout1->addWidget(m_searchButton);
    mainLayout->addLayout(buttonLayout1);
    
    QHBoxLayout* buttonLayout2 = new QHBoxLayout();
    m_inOrderButton = new QPushButton("Recorrido In-Order", this);
    m_preOrderButton = new QPushButton("Recorrido Pre-Order", this);
    m_postOrderButton = new QPushButton("Recorrido Post-Order", this);
    m_exportButton = new QPushButton("Exportar Recorridos", this);
    buttonLayout2->addWidget(m_inOrderButton);
    buttonLayout2->addWidget(m_preOrderButton);
    buttonLayout2->addWidget(m_postOrderButton);
    buttonLayout2->addWidget(m_exportButton);
    mainLayout->addLayout(buttonLayout2);
    
    QLabel* outputLabel = new QLabel("Resultados:", this);
    mainLayout->addWidget(outputLabel);
    
    m_outputText = new QTextEdit(this);
    m_outputText->setReadOnly(true);
    m_outputText->setFont(QFont("Courier New", 9));
    mainLayout->addWidget(m_outputText);
    
    connect(m_insertButton, &QPushButton::clicked, this, &TreeTab::onInsertClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &TreeTab::onDeleteClicked);
    connect(m_searchButton, &QPushButton::clicked, this, &TreeTab::onSearchClicked);
    connect(m_inOrderButton, &QPushButton::clicked, this, &TreeTab::onInOrderClicked);
    connect(m_preOrderButton, &QPushButton::clicked, this, &TreeTab::onPreOrderClicked);
    connect(m_postOrderButton, &QPushButton::clicked, this, &TreeTab::onPostOrderClicked);
    connect(m_exportButton, &QPushButton::clicked, this, &TreeTab::onExportClicked);
}

void TreeTab::appendOutput(const QString& text) {
    m_outputText->append(text);
}

void TreeTab::setGraphController(GraphController* graphController) {
    m_graphController = graphController;
}

void TreeTab::showLoadedStations() {
    // Get all stations directly from the tree
    QVector<Station> allStations = m_controller->getAllStations();
    
    if (allStations.isEmpty()) {
        appendOutput("⚠ No se encontraron estaciones en el sistema");
        return;
    }
    
    // Display welcome message
    appendOutput("═══════════════════════════════════════════════════════");
    appendOutput("✓ ESTACIONES CARGADAS CORRECTAMENTE DESDE ARCHIVO");
    appendOutput(QString("  Total: %1 estaciones").arg(allStations.size()));
    appendOutput("═══════════════════════════════════════════════════════");
    appendOutput("");
    
    // Display each station
    for (const Station& station : allStations) {
        appendOutput(QString("  [ID: %1] %2")
                     .arg(station.getId(), 3)
                     .arg(station.getName()));
    }
    
    appendOutput("");
    appendOutput("───────────────────────────────────────────────────────");
    appendOutput("Utilice los botones de arriba para gestionar estaciones");
    appendOutput("───────────────────────────────────────────────────────");
}

void TreeTab::onInsertClicked() {
    bool ok;
    int id = m_idInput->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Error", "ID inválido");
        return;
    }
    QString name = m_nameInput->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "El nombre no puede estar vacío");
        return;
    }
    
    // Insert station in tree and graph
    m_controller->insertStation(id, name);
    
    if (m_graphController) {
        Station station(id, name);
        m_graphController->getGraph()->addStation(station);
        
        // Ask if user wants to add connections
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Agregar Conexiones");
        msgBox.setText(QString("¿Desea agregar conexiones para la estación '%1'?").arg(name));
        msgBox.setIcon(QMessageBox::Question);
        QPushButton* yesButton = msgBox.addButton("Sí", QMessageBox::YesRole);
        msgBox.addButton("No", QMessageBox::NoRole);
        msgBox.exec();
        
        if (msgBox.clickedButton() == yesButton) {
            // Get all existing stations
            QVector<Station> allStations = m_controller->getAllStations();
            QStringList stationList;
            
            for (const Station& s : allStations) {
                if (s.getId() != id) { // Exclude the newly added station
                    stationList.append(QString("[%1] %2").arg(s.getId()).arg(s.getName()));
                }
            }
            
            if (stationList.isEmpty()) {
                QMessageBox::information(this, "Sin Estaciones", 
                    "No hay otras estaciones para conectar.");
            } else {
                bool addingConnections = true;
                while (addingConnections) {
                    QString selected = QInputDialog::getItem(
                        this, "Seleccionar Estación", 
                        QString("Conectar '%1' con:").arg(name),
                        stationList, 0, false, &ok);
                    
                    if (ok && !selected.isEmpty()) {
                        // Extract ID from "[ID] Name" format
                        int otherStationId = selected.mid(1, selected.indexOf(']') - 1).toInt();
                        
                        // Ask for distance
                        double distance = QInputDialog::getDouble(
                            this, "Distancia", 
                            QString("Distancia entre estación %1 y %2 (km):").arg(id).arg(otherStationId),
                            5.0, 0.1, 10000.0, 1, &ok);
                        
                        if (ok) {
                            m_graphController->addEdge(id, otherStationId, distance);
                            appendOutput(QString("✓ Conexión agregada: %1 ↔ %2 (distancia: %3 km)")
                                         .arg(id).arg(otherStationId).arg(distance, 0, 'f', 1));
                            
                            // Ask if want to add more connections
                            QMessageBox msgBox2(this);
                            msgBox2.setWindowTitle("Más Conexiones");
                            msgBox2.setText("¿Desea agregar otra conexión?");
                            msgBox2.setIcon(QMessageBox::Question);
                            QPushButton* yesButton2 = msgBox2.addButton("Sí", QMessageBox::YesRole);
                            msgBox2.addButton("No", QMessageBox::NoRole);
                            msgBox2.exec();
                            
                            if (msgBox2.clickedButton() != yesButton2) {
                                addingConnections = false;
                            }
                        } else {
                            addingConnections = false;
                        }
                    } else {
                        addingConnections = false;
                    }
                }
                
                // Reload map to show new connections
                m_graphController->loadMap();
            }
        }
    }
}

void TreeTab::onDeleteClicked() {
    bool ok;
    int id = m_idInput->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Error", "ID inválido");
        return;
    }
    
    // Check if station has connections in graph
    if (m_graphController) {
        QVector<Edge> edges = m_graphController->getGraph()->getEdgesFrom(id);
        
        if (!edges.isEmpty()) {
            // Station has connections, show warning
            QString connectionsList;
            for (const Edge& edge : edges) {
                Station connectedStation = m_graphController->getGraph()->getStation(edge.getTo());
                connectionsList += QString("\n  • Estación %1: %2 (distancia: %3 km)")
                                   .arg(edge.getTo())
                                   .arg(connectedStation.getName())
                                   .arg(edge.getWeight(), 0, 'f', 1);
            }
            
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("⚠ Advertencia - Estación con Conexiones");
            msgBox.setText(QString("La estación ID=%1 tiene %2 conexión(es) activa(s):%3\n\n"
                    "Si elimina esta estación, también se eliminarán todas sus conexiones.\n\n"
                    "¿Está seguro que desea continuar?")
                .arg(id)
                .arg(edges.size())
                .arg(connectionsList));
            msgBox.setIcon(QMessageBox::Warning);
            QPushButton* yesButton = msgBox.addButton("Sí", QMessageBox::YesRole);
            QPushButton* noButton = msgBox.addButton("No", QMessageBox::NoRole);
            msgBox.setDefaultButton(noButton); // Default to "No"
            msgBox.exec();
            
            if (msgBox.clickedButton() != yesButton) {
                appendOutput(QString("✗ Eliminación cancelada para estación ID=%1").arg(id));
                return;
            }
            
            // Delete all edges connected to this station
            for (const Edge& edge : edges) {
                m_graphController->removeEdge(id, edge.getTo());
            }
            
            // Also check for edges coming TO this station
            QVector<Station> allStations = m_controller->getAllStations();
            for (const Station& station : allStations) {
                if (station.getId() != id) {
                    QVector<Edge> incomingEdges = m_graphController->getGraph()->getEdgesFrom(station.getId());
                    for (const Edge& edge : incomingEdges) {
                        if (edge.getTo() == id) {
                            m_graphController->removeEdge(station.getId(), id);
                        }
                    }
                }
            }
        }
    }
    
    // Delete from tree
    m_controller->deleteStation(id);
    
    // Reload map if graph controller exists
    if (m_graphController) {
        m_graphController->loadMap();
    }
}

void TreeTab::onSearchClicked() {
    bool ok;
    int id = m_idInput->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Error", "ID inválido");
        return;
    }
    m_controller->searchStation(id);
}

void TreeTab::onInOrderClicked() {
    m_controller->performInOrderTraversal();
}

void TreeTab::onPreOrderClicked() {
    m_controller->performPreOrderTraversal();
}

void TreeTab::onPostOrderClicked() {
    m_controller->performPostOrderTraversal();
}

void TreeTab::onExportClicked() {
    QString filename = QFileDialog::getSaveFileName(
        this, "Exportar Recorridos", "recorridos_rutas.txt",
        "Archivos de texto (*.txt);;Todos los archivos (*)");
    if (!filename.isEmpty()) {
        m_controller->exportTraversals(filename);
    }
}

void TreeTab::onStationInserted(int id, const QString& name, bool success) {
    if (success) {
        appendOutput(QString("✓ Estación insertada: ID=%1, Nombre=%2").arg(id).arg(name));
        m_idInput->clear();
        m_nameInput->clear();
    } else {
        appendOutput(QString("✗ No se pudo insertar la estación ID=%1").arg(id));
    }
}

void TreeTab::onStationDeleted(int id, bool success) {
    if (success) {
        appendOutput(QString("✓ Estación eliminada: ID=%1").arg(id));
        m_idInput->clear();
    } else {
        appendOutput(QString("✗ No se pudo eliminar la estación ID=%1").arg(id));
    }
}

void TreeTab::onStationFound(const Station& station) {
    appendOutput(QString("✓ Estación encontrada: ID=%1, Nombre=%2")
                 .arg(station.getId()).arg(station.getName()));
}

void TreeTab::onStationNotFound(int id) {
    appendOutput(QString("✗ No se encontró la estación con ID=%1").arg(id));
}

void TreeTab::onTraversalResult(const QString& type, const QVector<int>& ids) {
    QString result = QString("Recorrido %1: ").arg(type);
    if (ids.isEmpty()) {
        result += "(árbol vacío)";
    } else {
        for (int i = 0; i < ids.size(); ++i) {
            result += QString::number(ids[i]);
            if (i < ids.size() - 1) result += ", ";
        }
    }
    appendOutput(result);
}

void TreeTab::onTraversalsExported(bool success, const QString& filename) {
    if (success) {
        appendOutput(QString("✓ Recorridos exportados a: %1").arg(filename));
    } else {
        appendOutput("✗ Error al exportar recorridos");
    }
}

void TreeTab::onError(const QString& message) {
    appendOutput(QString("ERROR: %1").arg(message));
}
