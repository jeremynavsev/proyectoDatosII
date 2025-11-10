#ifndef TREETAB_H
#define TREETAB_H

#include "../Station.h"
#include <QWidget>
#include <QVector>

class QPushButton;
class QTextEdit;
class QLineEdit;
class TreeController;

/**
 * @brief Tab for Binary Search Tree operations
 */
class TreeTab : public QWidget {
    Q_OBJECT
    
public:
    explicit TreeTab(TreeController* controller, QWidget* parent = nullptr);
    
public slots:
    void onInsertClicked();
    void onDeleteClicked();
    void onSearchClicked();
    void onInOrderClicked();
    void onPreOrderClicked();
    void onPostOrderClicked();
    void onExportClicked();
    
    void onStationInserted(int id, const QString& name, bool success);
    void onStationDeleted(int id, bool success);
    void onStationFound(const Station& station);
    void onStationNotFound(int id);
    void onTraversalResult(const QString& type, const QVector<int>& ids);
    void onTraversalsExported(bool success, const QString& filename);
    void onError(const QString& message);
    
private:
    void setupUI();
    void appendOutput(const QString& text);
    
    TreeController* m_controller;
    QLineEdit* m_idInput;
    QLineEdit* m_nameInput;
    QPushButton* m_insertButton;
    QPushButton* m_deleteButton;
    QPushButton* m_searchButton;
    QPushButton* m_inOrderButton;
    QPushButton* m_preOrderButton;
    QPushButton* m_postOrderButton;
    QPushButton* m_exportButton;
    QTextEdit* m_outputText;
};

#endif // TREETAB_H
