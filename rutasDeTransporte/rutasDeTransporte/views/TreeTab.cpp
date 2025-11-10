#include "TreeTab.h"
#include "../TreeController.h"
#include "../Station.h"
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

TreeTab::TreeTab(TreeController* controller, QWidget* parent)
    : QWidget(parent), m_controller(controller) {
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
    
    QGroupBox* inputGroup = new QGroupBox("Gestión de Estaciones", this);
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
    m_controller->insertStation(id, name);
}

void TreeTab::onDeleteClicked() {
    bool ok;
    int id = m_idInput->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Error", "ID inválido");
        return;
    }
    m_controller->deleteStation(id);
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
