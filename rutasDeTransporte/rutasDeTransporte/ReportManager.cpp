#include "ReportManager.h"
#include <QFile>
#include <QTextStream>
#include <QStringConverter>

ReportManager::ReportManager() {}

void ReportManager::addReport(const ReportEntry& entry) {
    m_reports.append(entry);
}

bool ReportManager::loadReports(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    file.close();
    return true;
}

bool ReportManager::saveReports(const QString& filename) const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << getReportsAsText();
    file.close();
    return true;
}

QString ReportManager::getReportsAsText() const {
    QString result;
    result += "=== REPORTES DE CONSULTAS DE RUTAS ===\n\n";
    
    for (int i = 0; i < m_reports.size(); ++i) {
        const ReportEntry& report = m_reports[i];
        result += QString("Reporte #%1\n").arg(i + 1);
        result += QString("Fecha y Hora: %1\n").arg(report.timestamp.toString("yyyy-MM-dd HH:mm:ss"));
        result += QString("Algoritmo: %1\n").arg(report.algorithm);
        
        if (report.algorithm.contains("MST", Qt::CaseInsensitive)) {
            result += "Tipo: Árbol de Expansión Mínima (MST)\n";
            result += QString("Número de aristas: %1\n").arg(report.path.size() / 2);
            result += "Aristas del MST:\n";
            
            for (int j = 0; j < report.path.size(); j += 2) {
                if (j + 1 < report.path.size()) {
                    QString from = QString::number(report.path[j]);
                    QString to = QString::number(report.path[j + 1]);
                    
                    if (j < report.pathNames.size() && !report.pathNames[j].isEmpty()) {
                        from += QString(" (%1)").arg(report.pathNames[j]);
                    }
                    if (j + 1 < report.pathNames.size() && !report.pathNames[j + 1].isEmpty()) {
                        to += QString(" (%1)").arg(report.pathNames[j + 1]);
                    }
                    
                    result += QString("  • %1 ↔ %2\n").arg(from).arg(to);
                }
            }
            result += QString("Costo Total del MST: %1 km\n").arg(report.totalCost, 0, 'f', 2);
        } else {
            result += QString("Origen: %1 - %2\n").arg(report.originId).arg(report.originName);
            result += QString("Destino: %1 - %2\n").arg(report.destinationId).arg(report.destinationName);
            
            if (report.path.isEmpty()) {
                result += "Ruta: No se encontró camino\n";
                result += "Costo Total: N/A\n";
            } else {
                result += "Ruta: ";
                for (int j = 0; j < report.path.size(); ++j) {
                    result += QString::number(report.path[j]);
                    if (j < report.pathNames.size() && !report.pathNames[j].isEmpty()) {
                        result += QString(" (%1)").arg(report.pathNames[j]);
                    }
                    if (j < report.path.size() - 1) result += " → ";
                }
                result += "\n";
                result += QString("Costo Total: %1 km\n").arg(report.totalCost, 0, 'f', 2);
            }
        }
        
        result += "\n----------------------------------------\n\n";
    }
    return result;
}

void ReportManager::clear() {
    m_reports.clear();
}

int ReportManager::getReportCount() const {
    return m_reports.size();
}
