#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTableWidget>
#include <QHeaderView>
#include <QSettings>
#include <QDate>
#include <QTime>
#include <QResizeEvent>
#include <QComboBox>

class HistoryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryWindow(QWidget *parent = nullptr);
    ~HistoryWindow() override;
    
    // Public methods to add records
    void addCompressionRecord(const QString &fileType, qint64 originalSize, qint64 compressedSize, const QString &filePath = "");
    void addDecompressionRecord(const QString &fileType, qint64 compressedSize, qint64 decompressedSize, const QString &filePath = "");

signals:
    void navigateToDashboard();
    void navigateToCompress();
    void navigateToDecompress();
    void navigateToVisualizer();
    void navigateToHistory();
    void navigateToAboutHelp();
    void navigateToSelection();

public:
    void applyTheme();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupSidebar();
    void setupContentArea();
    void applyStyles();
    void setupAnimations();
    void updateResponsiveSizes();
    void loadHistory();
    void saveHistory();
    void refreshHistoryTable();
    void clearHistory();
    void onFilterChanged(int index);

    // Core UI Components
    QWidget *mainCentralWidget;
    QHBoxLayout *mainLayout;

    // Sidebar Components
    QWidget *sidebar;
    QVBoxLayout *sidebarLayout;
    QPushButton *dashboardButton;
    QPushButton *compressButton;
    QPushButton *decompressButton;
    QPushButton *visualizerButton;
    QPushButton *historyButton;
    QPushButton *aboutButton;
    QPushButton *backButton;

    // Content Area Components
    QWidget *contentArea;
    QVBoxLayout *contentLayout;
    QLabel *titleLabel;
    QComboBox *filterComboBox;
    QTableWidget *historyTable;
    QPushButton *clearButton;
    
    // Data storage
    struct HistoryRecord {
        QString type; // "Compress" or "Decompress"
        QDate date;
        QTime time;
        QString fileType;
        QString filePath;
        qint64 originalSize; // For compress: original, For decompress: compressed
        qint64 compressedSize; // For compress: compressed, For decompress: decompressed
        double compressionRatio; // For compress: reduction %, For decompress: expansion %
    };
    QList<HistoryRecord> historyRecords;
    QString currentFilter; // "All", "Compress", "Decompress"
};

#endif // HISTORY_H

