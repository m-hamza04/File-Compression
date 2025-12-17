#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QResizeEvent>

class DashboardWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWindow(QWidget *parent = nullptr);
    ~DashboardWindow() override;
    
    // Public method to update stats
    void updateStats(qint64 originalSize, qint64 compressedSize);

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
    void loadStats();
    void saveStats();
    void refreshStatsDisplay();
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
    QWidget *welcomeCard;
    QLabel *welcomeLabel;
    QLabel *descriptionLabel;
    
    // Stats Cards
    QWidget *statsContainer;
    QHBoxLayout *statsLayout;
    QWidget *statCard1;
    QWidget *statCard2;
    QWidget *statCard3;
    QLabel *statLabel1;
    QLabel *statLabel2;
    QLabel *statLabel3;
    QLabel *statValue1;
    QLabel *statValue2;
    QLabel *statValue3;
    
    // Stats tracking
    int totalFilesProcessed;
    qint64 totalSpaceSaved; // in bytes
    double totalCompressionRatio; // sum of all compression ratios
};

#endif // DASHBOARD_H

