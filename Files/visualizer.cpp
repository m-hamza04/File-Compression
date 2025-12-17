#include "visualizer.h"
#include "theme.h"
#include <QFont>
#include <QApplication>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QPainter>
#include <QRandomGenerator>
#include <QSettings>
#include <QDate>
#include <QDebug>

VisualizerWindow::VisualizerWindow(QWidget *parent)
    : QWidget(parent), series2(nullptr), set2(nullptr)
{
    mainCentralWidget = new QWidget(this);
    QHBoxLayout *outerLayout = new QHBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);
    outerLayout->addWidget(mainCentralWidget);

    mainLayout = new QHBoxLayout(mainCentralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Load compression history
    loadCompressionHistory();

    setupSidebar();
    setupContentArea();
    setupCharts();

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1); // Content area takes remaining space
    
    // Set initial responsive sizes
    updateResponsiveSizes();

    applyStyles();
    setupAnimations();

    // --- Sidebar Navigation Connections ---
    connect(backButton, &QPushButton::clicked, this, &VisualizerWindow::navigateToSelection);
    connect(dashboardButton, &QPushButton::clicked, this, &VisualizerWindow::navigateToDashboard);
    connect(compressButton, &QPushButton::clicked, this, &VisualizerWindow::navigateToCompress);
    connect(decompressButton, &QPushButton::clicked, this, &VisualizerWindow::navigateToDecompress);
    connect(visualizerButton, &QPushButton::clicked, this, &VisualizerWindow::navigateToVisualizer);
}

VisualizerWindow::~VisualizerWindow() = default;

// =============================================================
// ====================== UI SETUP METHODS =======================
// =============================================================

void VisualizerWindow::setupSidebar()
{
    sidebar = new QWidget();
    sidebar->setFixedWidth(250);
    sidebar->setObjectName("Sidebar");
    sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(20, 30, 20, 30);
    sidebarLayout->setSpacing(15);

    // Back button
    backButton = new QPushButton("← Back", sidebar);
    backButton->setFixedSize(200, 50);
    backButton->setCursor(Qt::PointingHandCursor);
    QFont backFont("Segoe UI", 12, QFont::Bold);
    backButton->setFont(backFont);
    sidebarLayout->addWidget(backButton);
    sidebarLayout->addSpacing(20);

    // Navigation buttons in consistent order
    dashboardButton = new QPushButton("📊 Dashboard", sidebar);
    dashboardButton->setFixedSize(200, 50);
    dashboardButton->setCursor(Qt::PointingHandCursor);
    dashboardButton->setObjectName("NavButton");

    compressButton = new QPushButton("📦 Compression", sidebar);
    compressButton->setFixedSize(200, 50);
    compressButton->setCursor(Qt::PointingHandCursor);
    compressButton->setObjectName("NavButton");

    decompressButton = new QPushButton("📤 Decompression", sidebar);
    decompressButton->setFixedSize(200, 50);
    decompressButton->setCursor(Qt::PointingHandCursor);
    decompressButton->setObjectName("NavButton");

    visualizerButton = new QPushButton("📈 Visualizer", sidebar);
    visualizerButton->setFixedSize(200, 50);
    visualizerButton->setCursor(Qt::PointingHandCursor);
    visualizerButton->setObjectName("NavButton");

    QPushButton *historyButton = new QPushButton("📜 History", sidebar);
    historyButton->setFixedSize(200, 50);
    historyButton->setCursor(Qt::PointingHandCursor);
    historyButton->setObjectName("NavButton");

    QPushButton *aboutButton = new QPushButton("ℹ️ About", sidebar);
    aboutButton->setFixedSize(200, 50);
    aboutButton->setCursor(Qt::PointingHandCursor);
    aboutButton->setObjectName("NavButton");

    sidebarLayout->addWidget(dashboardButton);
    sidebarLayout->addWidget(compressButton);
    sidebarLayout->addWidget(decompressButton);
    sidebarLayout->addWidget(visualizerButton);
    sidebarLayout->addWidget(historyButton);
    sidebarLayout->addWidget(aboutButton);
    sidebarLayout->addStretch();
    
    // Connect all navigation buttons
    connect(backButton, &QPushButton::clicked, this, &VisualizerWindow::navigateToSelection);
    connect(dashboardButton, &QPushButton::clicked, this, &VisualizerWindow::navigateToDashboard);
    connect(compressButton, &QPushButton::clicked, this, &VisualizerWindow::navigateToCompress);
    connect(decompressButton, &QPushButton::clicked, this, &VisualizerWindow::navigateToDecompress);
    connect(visualizerButton, &QPushButton::clicked, this, &VisualizerWindow::navigateToVisualizer);
    connect(historyButton, &QPushButton::clicked, this, &VisualizerWindow::navigateToHistory);
    connect(aboutButton, &QPushButton::clicked, this, &VisualizerWindow::navigateToAboutHelp);
}

void VisualizerWindow::setupContentArea()
{
    contentArea = new QWidget();
    contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(60, 50, 60, 50);
    contentLayout->setSpacing(35);

    // Title
    titleLabel = new QLabel("Visualizer", this);
    QFont titleFont("Segoe UI", 52, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    contentLayout->addWidget(titleLabel);
    contentLayout->addSpacing(25);

    // Description Card - Modern Design
    QWidget *descCard = new QWidget(this);
    descCard->setObjectName("DescCard");
    descCard->setFixedHeight(140);
    QVBoxLayout *descLayout = new QVBoxLayout(descCard);
    descLayout->setContentsMargins(45, 35, 45, 35);
    descLayout->setSpacing(15);

    descriptionLabel = new QLabel("📊 Visualize compression statistics and performance metrics", descCard);
    QFont descFont("Segoe UI", 20, QFont::Bold);
    descriptionLabel->setFont(descFont);
    descriptionLabel->setStyleSheet("color: #0ea5e9; background: transparent; letter-spacing: 0.5px;");
    descriptionLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    descLayout->addWidget(descriptionLabel);

    QLabel *subDescLabel = new QLabel("Analyze compression trends, file size distributions, and performance data with interactive charts", descCard);
    QFont subDescFont("Segoe UI", 14);
    subDescLabel->setFont(subDescFont);
    subDescLabel->setStyleSheet("color: #bae6fd; background: transparent; line-height: 1.6;");
    subDescLabel->setWordWrap(true);
    subDescLabel->setAlignment(Qt::AlignLeft);
    descLayout->addWidget(subDescLabel);

    contentLayout->addWidget(descCard);
    contentLayout->addSpacing(35);

    // Chart Container 2 - Modern Design
    chartContainer2 = new QWidget();
    chartContainer2->setObjectName("ChartContainer2");
    chartContainer2->setFixedHeight(360);
    QVBoxLayout *chartLayout2 = new QVBoxLayout(chartContainer2);
    chartLayout2->setContentsMargins(35, 35, 35, 35);
    chartLayout2->setSpacing(0);

    chartView2 = new QChartView();
    chartView2->setRenderHint(QPainter::Antialiasing);
    chart2 = new QChart();
    chart2->setTitle("File Size Distribution");
    chart2->setTheme(QChart::ChartThemeDark);
    chart2->setBackgroundVisible(false);
    chartView2->setChart(chart2);
    chartView2->setMinimumHeight(300);
    chartLayout2->addWidget(chartView2);

    contentLayout->addWidget(chartContainer2);
    contentLayout->addStretch(1);
}

void VisualizerWindow::setupCharts()
{
    // Setup Chart 2 - File Size Distribution by Type
    set2 = new QBarSet("Size");
    set2->setBrush(QBrush(QColor("#38bdf8")));
    set2->setPen(QPen(QColor("#0ea5e9"), 2));

    series2 = new QBarSeries();
    series2->append(set2);
    chart2->addSeries(series2);

    QBarCategoryAxis *axisX2 = new QBarCategoryAxis();
    axisX2->setTitleText("File Types");
    axisX2->setTitleBrush(QBrush(QColor("#7dd3fc")));
    axisX2->setLabelsFont(QFont("Segoe UI", 9));
    axisX2->setLabelsBrush(QBrush(QColor("#bae6fd")));
    chart2->addAxis(axisX2, Qt::AlignBottom);
    series2->attachAxis(axisX2);

    QValueAxis *axisY2 = new QValueAxis();
    axisY2->setRange(0, 100);
    axisY2->setTitleText("Total Size (MB)");
    axisY2->setTitleBrush(QBrush(QColor("#7dd3fc")));
    axisY2->setLabelsFont(QFont("Segoe UI", 10));
    axisY2->setLabelsBrush(QBrush(QColor("#bae6fd")));
    axisY2->setGridLineColor(QColor(14, 165, 233, 100));
    chart2->addAxis(axisY2, Qt::AlignLeft);
    series2->attachAxis(axisY2);

    chart2->setTitleFont(QFont("Arial", 16, QFont::Bold));
    chart2->setTitleBrush(QBrush(QColor("#7dd3fc")));
    chart2->legend()->setVisible(false);
    
    // Update charts with real data
    updateCharts();
}

void VisualizerWindow::applyStyles()
{
    Theme::ThemeColors colors = Theme::getThemeColors(Theme::getCurrentTheme());
    
    QString mainStyle = QString(R"(
        QWidget {
            background: qlineargradient(
                x1: 0, y1: 0, x2: 1, y2: 1,
                stop: 0 %1,
                stop: 0.3 %2,
                stop: 0.6 %3,
                stop: 1 %4
            );
            color: white;
            font-family: "Segoe UI", sans-serif;
        }
    )").arg(colors.bgGradientStart, colors.bgGradientMid, colors.bgGradientEnd, colors.bgGradientStop3);
    
    mainCentralWidget->setStyleSheet(mainStyle);

    // Sidebar styling
    QString sidebarStyle = QString(R"(
        QWidget#Sidebar {
            background: %1;
            border-right: 2px solid %2;
        }
        QPushButton#NavButton {
            background: %3;
            color: %4;
            border-radius: 12px;
            border: 2px solid %5;
            padding: 12px;
            font-weight: bold;
            text-align: left;
        }
        QPushButton#NavButton:hover {
            background: %6;
            color: white;
            border: 2px solid %7;
        }
        QPushButton#NavButton:pressed {
            background: %8;
        }
    )").arg(colors.sidebarBg, colors.sidebarBorder, colors.navButtonBg, colors.textSecondary, 
            colors.sidebarBorder, colors.navButtonHover, colors.primary, colors.navButtonActive);
    
    sidebar->setStyleSheet(sidebarStyle);

    QString backButtonStyle = QString(R"(
        QPushButton {
            background: %1;
            color: %2;
            border-radius: 12px;
            border: 2px solid %3;
            padding: 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: %4;
            color: white;
            border: 2px solid %5;
        }
        QPushButton:pressed {
            background: %6;
        }
    )").arg(colors.navButtonBg, colors.textSecondary, colors.primary, 
            colors.navButtonHover, colors.primary, colors.navButtonActive);
    
    backButton->setStyleSheet(backButtonStyle);
    
    // Highlight the Visualizer button (active screen)
    QString activeButtonStyle = QString(R"(
        QPushButton {
            background: %1;
            color: white;
            border-radius: 12px;
            border: 2px solid %2;
            padding: 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: %3;
        }
    )").arg(colors.navButtonActive, colors.primary, colors.navButtonActive);
    
    visualizerButton->setStyleSheet(activeButtonStyle);

    QString contentStyle = QString(R"(
        QWidget {
            background: %1;
            color: %2;
        }
    )").arg(colors.bgGradientMid, colors.textTertiary);
    
    contentArea->setStyleSheet(contentStyle);

    // Description Card Styling - Modern Design
    QWidget *descCard = contentArea->findChild<QWidget*>("DescCard");
    if (descCard) {
        descCard->setStyleSheet(R"(
            QWidget#DescCard {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 rgba(14, 165, 233, 0.3),
                    stop:1 rgba(56, 189, 248, 0.3));
                border: 2px solid rgba(14, 165, 233, 0.6);
                border-radius: 25px;
                box-shadow: 0 8px 32px rgba(14, 165, 233, 0.3);
            }
        )");
    }

    chartContainer2->setStyleSheet(R"(
        QWidget#ChartContainer2 {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(26, 14, 10, 0.8),
                stop:1 rgba(26, 14, 10, 0.7));
            border: 2px solid rgba(14, 165, 233, 0.7);
            border-radius: 28px;
            box-shadow: 0 10px 40px rgba(14, 165, 233, 0.35);
        }
    )");

    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #0ea5e9;
            background: transparent;
            text-shadow: 0 2px 15px rgba(14, 165, 233, 0.5);
            letter-spacing: 2px;
        }
    )");

    descriptionLabel->setStyleSheet(R"(
        QLabel {
            color: #0ea5e9;
            background: transparent;
            letter-spacing: 1px;
            text-shadow: 0 2px 10px rgba(14, 165, 233, 0.3);
        }
    )");
}

void VisualizerWindow::setupAnimations()
{
    // Animate sidebar entrance
    QGraphicsOpacityEffect *sidebarEffect = new QGraphicsOpacityEffect(this);
    sidebarEffect->setOpacity(0);
    sidebar->setGraphicsEffect(sidebarEffect);
    
    QPropertyAnimation *sidebarAnim = new QPropertyAnimation(sidebarEffect, "opacity", this);
    sidebarAnim->setDuration(800);
    sidebarAnim->setStartValue(0);
    sidebarAnim->setEndValue(1);
    sidebarAnim->setEasingCurve(QEasingCurve::OutCubic);
    sidebarAnim->start();

    // Animate content area entrance
    QGraphicsOpacityEffect *contentEffect = new QGraphicsOpacityEffect(this);
    contentEffect->setOpacity(0);
    contentArea->setGraphicsEffect(contentEffect);
    
    QPropertyAnimation *contentAnim = new QPropertyAnimation(contentEffect, "opacity", this);
    contentAnim->setDuration(1000);
    contentAnim->setStartValue(0);
    contentAnim->setEndValue(1);
    contentAnim->setEasingCurve(QEasingCurve::OutCubic);

    QTimer::singleShot(200, [contentAnim]() {
        contentAnim->start();
    });

    // Animate title
    QGraphicsOpacityEffect *titleEffect = new QGraphicsOpacityEffect(this);
    titleEffect->setOpacity(0);
    titleLabel->setGraphicsEffect(titleEffect);
    
    QPropertyAnimation *titleAnim = new QPropertyAnimation(titleEffect, "opacity", this);
    titleAnim->setDuration(1200);
    titleAnim->setStartValue(0);
    titleAnim->setEndValue(1);
    titleAnim->setEasingCurve(QEasingCurve::OutCubic);

    QTimer::singleShot(400, [titleAnim]() {
        titleAnim->start();
    });

    // Animate chart container
    QGraphicsOpacityEffect *chart2Effect = new QGraphicsOpacityEffect(this);
    chart2Effect->setOpacity(0);
    chartContainer2->setGraphicsEffect(chart2Effect);
    
    QPropertyAnimation *chart2Anim = new QPropertyAnimation(chart2Effect, "opacity", this);
    chart2Anim->setDuration(1200);
    chart2Anim->setStartValue(0);
    chart2Anim->setEndValue(1);
    chart2Anim->setEasingCurve(QEasingCurve::OutCubic);

    QTimer::singleShot(600, [chart2Anim]() {
        chart2Anim->start();
    });
}

// =============================================================
// ======================= DATA MANAGEMENT =======================
// =============================================================

void VisualizerWindow::loadCompressionHistory()
{
    QSettings settings;
    int count = settings.value("visualizer/recordCount", 0).toInt();
    
    compressionHistory.clear();
    for (int i = 0; i < count; ++i) {
        QString key = QString("visualizer/record%1").arg(i);
        CompressionRecord record;
        record.date = QDate::fromString(settings.value(key + "/date").toString(), Qt::ISODate);
        record.fileType = settings.value(key + "/fileType").toString();
        record.compressionRatio = settings.value(key + "/ratio").toDouble();
        record.fileSize = settings.value(key + "/size").toLongLong();
        
        if (record.date.isValid() && !record.fileType.isEmpty()) {
            compressionHistory.append(record);
        }
    }
    
    // Keep only last 100 records to prevent excessive storage
    if (compressionHistory.size() > 100) {
        compressionHistory = compressionHistory.mid(compressionHistory.size() - 100);
        saveCompressionHistory();
    }
}

void VisualizerWindow::saveCompressionHistory()
{
    QSettings settings;
    settings.remove("visualizer"); // Clear old data
    
    settings.setValue("visualizer/recordCount", compressionHistory.size());
    for (int i = 0; i < compressionHistory.size(); ++i) {
        QString key = QString("visualizer/record%1").arg(i);
        const CompressionRecord &record = compressionHistory[i];
        settings.setValue(key + "/date", record.date.toString(Qt::ISODate));
        settings.setValue(key + "/fileType", record.fileType);
        settings.setValue(key + "/ratio", record.compressionRatio);
        settings.setValue(key + "/size", record.fileSize);
    }
}

void VisualizerWindow::addCompressionData(const QString &fileType, qint64 originalSize, qint64 compressedSize)
{
    if (originalSize <= 0) return;
    
    CompressionRecord record;
    record.date = QDate::currentDate();
    record.fileType = fileType;
    record.compressionRatio = 100.0 * (1.0 - (double)compressedSize / originalSize);
    record.fileSize = originalSize;
    
    compressionHistory.append(record);
    
    // Keep only last 100 records
    if (compressionHistory.size() > 100) {
        compressionHistory = compressionHistory.mid(compressionHistory.size() - 100);
    }
    
    saveCompressionHistory();
    updateCharts();
}

void VisualizerWindow::updateCharts()
{
    if (!set2 || !series2) return;
    
    // Chart 2: File Size Distribution by Type
    QMap<QString, qint64> typeSizes;
    QStringList allTypes = {"Image", "Video", "Text", "PDF", "Audio", "Archive", "Other"};
    
    // Initialize all types
    for (const QString &type : allTypes) {
        typeSizes[type] = 0;
    }
    
    // Sum file sizes by type
    for (const CompressionRecord &record : compressionHistory) {
        QString type = record.fileType;
        if (!allTypes.contains(type)) {
            type = "Other";
        }
        typeSizes[type] += record.fileSize;
    }
    
    // Convert to MB and update chart
    QStringList categories2;
    QList<double> typeSizesMB;
    double maxSizeMB = 0;
    
    for (const QString &type : allTypes) {
        categories2.append(type);
        double sizeMB = typeSizes[type] / (1024.0 * 1024.0);
        typeSizesMB.append(sizeMB);
        if (sizeMB > maxSizeMB) {
            maxSizeMB = sizeMB;
        }
    }
    
    // Update Y-axis range for chart 2 (with some padding)
    QValueAxis *axisY2 = qobject_cast<QValueAxis*>(chart2->axes(Qt::Vertical).first());
    if (axisY2) {
        double maxRange = qMax(100.0, maxSizeMB * 1.2);
        axisY2->setRange(0, maxRange);
    }
    
    // Update chart 2
    set2->remove(0, set2->count());
    for (double sizeMB : typeSizesMB) {
        *set2 << sizeMB;
    }
    
    QBarCategoryAxis *axisX2 = qobject_cast<QBarCategoryAxis*>(chart2->axes(Qt::Horizontal).first());
    if (axisX2) {
        axisX2->clear();
        axisX2->append(categories2);
    }
    
    // Refresh chart view
    chartView2->update();
}

void VisualizerWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateResponsiveSizes();
}

void VisualizerWindow::applyTheme()
{
    // Re-apply styles with current theme
    applyStyles();
}

void VisualizerWindow::updateResponsiveSizes() {
    // Scale fonts based on window size (sidebar fixed at 250px)
    int windowWidth = width();
    double scale = qBound(0.7, (double)windowWidth / 1920.0, 1.5);
    
    // Update title font
    if (titleLabel) {
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize((int)(52 * scale));
        titleLabel->setFont(titleFont);
    }
}

