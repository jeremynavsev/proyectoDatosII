#include "ReportDialog.h"
#include "../ReportManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStringConverter>

ReportDialog::ReportDialog(ReportManager* reportManager, QWidget* parent)
    : QDialog(parent), m_reportManager(reportManager) {
    setupUI();
    refreshReports();
}

void ReportDialog::setupUI() {
    setWindowTitle("Reportes de Consultas");
    resize(700, 500);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setFont(QFont("Courier New", 10));
    mainLayout->addWidget(m_textEdit);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_exportButton = new QPushButton("Exportar", this);
    m_closeButton = new QPushButton("Cerrar", this);
    
    buttonLayout->addWidget(m_exportButton);
    buttonLayout->addWidget(m_closeButton);
    mainLayout->addLayout(buttonLayout);
    
    connect(m_exportButton, &QPushButton::clicked, this, &ReportDialog::exportReports);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

void ReportDialog::refreshReports() {
    if (m_reportManager) {
        m_textEdit->setPlainText(m_reportManager->getReportsAsText());
    }
}

void ReportDialog::exportReports() {
    QString filename = QFileDialog::getSaveFileName(
        this, "Exportar Reportes", "reportes_exportados.txt",
        "Archivos de texto (*.txt);;Todos los archivos (*)");
    
    if (filename.isEmpty()) return;
    
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "No se pudo crear el archivo");
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << m_textEdit->toPlainText();
    file.close();
    
    QMessageBox::information(this, "Éxito", "Reportes exportados correctamente");
}
