#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

// QtCharts headers
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QSettings>
#include <QDate>
#include <QMap>
#include <QResizeEvent>

class VisualizerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VisualizerWindow(QWidget *parent = nullptr);
    ~VisualizerWindow() override;
    
    // Public method to add new compression data
    void addCompressionData(const QString &fileType, qint64 originalSize, qint64 compressedSize);

signals:
    void navigateToDashboard();
    void navigateToCompress();
    void navigateToDecompress();
    void navigateToVisualizer();
    void navigateToHistory();
    void navigateToSelection();
    void navigateToAboutHelp();

public:
    void applyTheme();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupSidebar();
    void setupContentArea();
    void applyStyles();
    void setupAnimations();
    void setupCharts();
    void updateCharts();
    void loadCompressionHistory();
    void saveCompressionHistory();
    void updateResponsiveSizes();
    
    // Data storage
    struct CompressionRecord {
        QDate date;
        QString fileType;
        double compressionRatio; // percentage
        qint64 fileSize; // in bytes
    };
    QList<CompressionRecord> compressionHistory;

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
    QLabel *descriptionLabel;
    
    // Chart Containers
    QWidget *chartContainer2;
    QChartView *chartView2;
    QChart *chart2;
    
    // Chart series for dynamic updates
    QBarSeries *series2;
    QBarSet *set2;
};

#endif // VISUALIZER_H

