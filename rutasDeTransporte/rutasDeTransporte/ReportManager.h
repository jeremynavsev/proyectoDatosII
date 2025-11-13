#ifndef REPORTMANAGER_H
#define REPORTMANAGER_H

#include <QString>
#include <QVector>
#include <QDateTime>

class ReportManager {
public:
    struct ReportEntry {
        QDateTime timestamp;
        QString algorithm;
        int originId;
        QString originName;
        int destinationId;
        QString destinationName;
        QVector<int> path;
        QVector<QString> pathNames;
        double totalCost;
    };
    
    ReportManager();
    
    void addReport(const ReportEntry& entry);
    bool loadReports(const QString& filename);
    bool saveReports(const QString& filename) const;
    QString getReportsAsText() const;
    void clear();
    int getReportCount() const;
    
private:
    QVector<ReportEntry> m_reports;
};

#endif // REPORTMANAGER_H
