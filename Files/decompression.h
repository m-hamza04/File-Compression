#ifndef DECOMPRESSION_H
#define DECOMPRESSION_H

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
#include <QtCharts/QBarSet>

class DecompressWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DecompressWindow(QWidget *parent = nullptr);
    ~DecompressWindow() override;

signals:
    void navigateToDashboard();
    void navigateToCompress();
    void navigateToDecompress();
    void navigateToVisualizer();
    void navigateToHistory();
    void navigateToSelection();
    void navigateToAboutHelp();
    void decompressionCompleted(const QString &fileType, qint64 compressedSize, qint64 decompressedSize);

private slots:
    // Slot for the main decompression logic
    void decompressSelectedFile(const QString &path);

public:
    void applyTheme();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupSidebar();
    void setupContentArea();
    void applyStyles();
    void setupAnimations();
    void updateDecompressionChart(qint64 compressedSize, qint64 decompressedSize);
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
    QLabel *compressedSizeLabel;
    QLabel *decompressedSizeLabel;
};

#endif // DECOMPRESSION_H

