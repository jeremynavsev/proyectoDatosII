#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>

class QTextEdit;
class QPushButton;
class ReportManager;

/**
 * @brief Dialog for displaying query reports
 */
class ReportDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit ReportDialog(ReportManager* reportManager, QWidget* parent = nullptr);
    
public slots:
    void refreshReports();
    void exportReports();
    
private:
    void setupUI();
    ReportManager* m_reportManager;
    QTextEdit* m_textEdit;
    QPushButton* m_exportButton;
    QPushButton* m_closeButton;
};

#endif // REPORTDIALOG_H
