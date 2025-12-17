#ifndef COMPRESS_H
#define COMPRESS_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLayout>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <vector>

// QtCharts headers
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSet> // Added for completeness

class CompressWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CompressWindow(QWidget *parent = nullptr);
    ~CompressWindow() override;

signals:
    void navigateToDashboard();
    void navigateToCompress();
    void navigateToDecompress();
    void navigateToVisualizer();
    void navigateToHistory();
    void navigateToSelection();
    void navigateToAboutHelp();
    void compressionCompleted(qint64 originalSize, qint64 compressedSize);
    void compressionCompletedWithType(const QString &fileType, qint64 originalSize, qint64 compressedSize);

private slots:
    // Slot for the main compression logic (the dispatcher)
    void compressSelectedFile(const QString &path);

public:
    void applyTheme();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupSidebar();
    void setupContentArea();
    void applyStyles();
    void setupAnimations();
    void updateCompressionChart(qint64 originalSize, qint64 compressedSize);
    void updateResponsiveSizes();

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
    QPushButton *backButton;

    // Content Area Components
    QWidget *contentArea;
    QVBoxLayout *contentLayout;
    QLabel *titleLabel;
    QPushButton *addFileButton;
    QLabel *filePathLabel;
    QPushButton *startButton;
    QString selectedFilePath;
    QPushButton *selectSaveLocationButton;
    QLabel *saveLocationLabel;
    QString saveLocationPath;

    // --- CHART AND SIZE DISPLAY COMPONENTS ---
    QWidget *chartContainer;
    QChartView *chartView;
    QLabel *originalSizeLabel;
    QLabel *compressedSizeLabel;
};

#endif // COMPRESS_H
