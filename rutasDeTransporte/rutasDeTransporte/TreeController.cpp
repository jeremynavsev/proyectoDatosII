#include "TreeController.h"

TreeController::TreeController(BinarySearchTree* tree, ReportManager* reportManager, QObject* parent) 
    : QObject(parent), m_tree(tree), m_reportManager(reportManager) {
}

TreeController::~TreeController() {
}

BinarySearchTree* TreeController::getTree() {
    return m_tree;
}

QVector<Station> TreeController::getAllStations() const {
    return m_tree->getAllStations();
}

void TreeController::insertStation(int id, const QString& name) {
    try {
        Station station(id, name);
        Station existing;
        if (m_tree->search(id, existing)) {
            emit stationInserted(id, name, false);
            emit errorOccurred(QString("La estación con ID %1 ya existe").arg(id));
            return;
        }
        m_tree->insert(station);
        emit stationInserted(id, name, true);
    } catch (...) {
        emit stationInserted(id, name, false);
        emit errorOccurred("Error al insertar la estación");
    }
}

void TreeController::deleteStation(int id) {
    try {
        bool success = m_tree->remove(id);
        emit stationDeleted(id, success);
        if (!success) {
            emit errorOccurred(QString("No se encontró la estación con ID %1").arg(id));
        }
    } catch (...) {
        emit stationDeleted(id, false);
        emit errorOccurred("Error al eliminar la estación");
    }
}

void TreeController::searchStation(int id) {
    try {
        Station result;
        if (m_tree->search(id, result)) {
            emit stationFound(result);
        } else {
            emit stationNotFound(id);
        }
    } catch (...) {
        emit errorOccurred("Error al buscar la estación");
    }
}

void TreeController::performInOrderTraversal() {
    try {
        QVector<int> result = m_tree->inOrderTraversal();
        emit traversalResult("In-Order", result);
    } catch (...) {
        emit errorOccurred("Error al realizar recorrido In-Order");
    }
}

void TreeController::performPreOrderTraversal() {
    try {
        QVector<int> result = m_tree->preOrderTraversal();
        emit traversalResult("Pre-Order", result);
    } catch (...) {
        emit errorOccurred("Error al realizar recorrido Pre-Order");
    }
}

void TreeController::performPostOrderTraversal() {
    try {
        QVector<int> result = m_tree->postOrderTraversal();
        emit traversalResult("Post-Order", result);
    } catch (...) {
        emit errorOccurred("Error al realizar recorrido Post-Order");
    }
}

void TreeController::exportTraversals(const QString& filename) {
    try {
        bool success = m_tree->exportTraversals(filename);
        emit traversalsExported(success, filename);
        if (!success) {
            emit errorOccurred("No se pudo exportar los recorridos");
        }
    } catch (...) {
        emit traversalsExported(false, filename);
        emit errorOccurred("Error al exportar recorridos");
    }
}

void TreeController::clearTree() {
    m_tree->clear();
}
