#include "dashboard.h"
#include "theme.h"
#include <QFont>
#include <QApplication>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSettings>
#include <QDebug>
#include <QSizePolicy>

DashboardWindow::DashboardWindow(QWidget *parent)
    : QWidget(parent),
      totalFilesProcessed(0),
      totalSpaceSaved(0),
      totalCompressionRatio(0.0)
{
    mainCentralWidget = new QWidget(this);
    QHBoxLayout *outerLayout = new QHBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);
    outerLayout->addWidget(mainCentralWidget);

    mainLayout = new QHBoxLayout(mainCentralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setupSidebar();
    setupContentArea();

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1); // Content area takes remaining space
    
    // Set initial responsive sizes
    updateResponsiveSizes();

    applyStyles();
    setupAnimations();
    
    // Load saved stats
    loadStats();

    // --- Sidebar Navigation Connections ---
    connect(backButton, &QPushButton::clicked, this, &DashboardWindow::navigateToSelection);
    connect(dashboardButton, &QPushButton::clicked, this, &DashboardWindow::navigateToDashboard);
    connect(compressButton, &QPushButton::clicked, this, &DashboardWindow::navigateToCompress);
    connect(decompressButton, &QPushButton::clicked, this, &DashboardWindow::navigateToDecompress);
    connect(visualizerButton, &QPushButton::clicked, this, &DashboardWindow::navigateToVisualizer);
}

DashboardWindow::~DashboardWindow() = default;

// =============================================================
// ====================== UI SETUP METHODS =======================
// =============================================================

void DashboardWindow::setupSidebar()
{
    sidebar = new QWidget();
    sidebar->setMinimumWidth(200);
    sidebar->setMaximumWidth(260);
    sidebar->setObjectName("Sidebar");
    sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(16, 26, 16, 26);
    sidebarLayout->setSpacing(12);

    // Back button
    backButton = new QPushButton("← Back", sidebar);
    backButton->setMinimumWidth(170);
    backButton->setMaximumWidth(240);
    backButton->setFixedHeight(48);
    backButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    backButton->setCursor(Qt::PointingHandCursor);
    QFont backFont("Segoe UI", 12, QFont::Bold);
    backButton->setFont(backFont);
    sidebarLayout->addWidget(backButton);
    sidebarLayout->addSpacing(20);

    // Navigation buttons in consistent order
    dashboardButton = new QPushButton("📊 Dashboard", sidebar);
    dashboardButton->setMinimumWidth(170);
    dashboardButton->setMaximumWidth(240);
    dashboardButton->setFixedHeight(48);
    dashboardButton->setCursor(Qt::PointingHandCursor);
    dashboardButton->setObjectName("NavButton");

    compressButton = new QPushButton("📦 Compression", sidebar);
    compressButton->setMinimumWidth(170);
    compressButton->setMaximumWidth(240);
    compressButton->setFixedHeight(48);
    compressButton->setCursor(Qt::PointingHandCursor);
    compressButton->setObjectName("NavButton");

    decompressButton = new QPushButton("📤 Decompression", sidebar);
    decompressButton->setMinimumWidth(170);
    decompressButton->setMaximumWidth(240);
    decompressButton->setFixedHeight(48);
    decompressButton->setCursor(Qt::PointingHandCursor);
    decompressButton->setObjectName("NavButton");

    visualizerButton = new QPushButton("📈 Visualizer", sidebar);
    visualizerButton->setMinimumWidth(170);
    visualizerButton->setMaximumWidth(240);
    visualizerButton->setFixedHeight(48);
    visualizerButton->setCursor(Qt::PointingHandCursor);
    visualizerButton->setObjectName("NavButton");

    QPushButton *historyButton = new QPushButton("📜 History", sidebar);
    historyButton->setMinimumWidth(170);
    historyButton->setMaximumWidth(240);
    historyButton->setFixedHeight(48);
    historyButton->setCursor(Qt::PointingHandCursor);
    historyButton->setObjectName("NavButton");

    QPushButton *aboutButton = new QPushButton("ℹ️ About", sidebar);
    aboutButton->setMinimumWidth(170);
    aboutButton->setMaximumWidth(240);
    aboutButton->setFixedHeight(48);
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
    connect(backButton, &QPushButton::clicked, this, &DashboardWindow::navigateToSelection);
    connect(dashboardButton, &QPushButton::clicked, this, &DashboardWindow::navigateToDashboard);
    connect(compressButton, &QPushButton::clicked, this, &DashboardWindow::navigateToCompress);
    connect(decompressButton, &QPushButton::clicked, this, &DashboardWindow::navigateToDecompress);
    connect(visualizerButton, &QPushButton::clicked, this, &DashboardWindow::navigateToVisualizer);
    connect(historyButton, &QPushButton::clicked, this, &DashboardWindow::navigateToHistory);
    connect(aboutButton, &QPushButton::clicked, this, &DashboardWindow::navigateToAboutHelp);
}

void DashboardWindow::setupContentArea()
{
    contentArea = new QWidget();
    contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(32, 28, 32, 28);
    contentLayout->setSpacing(26);

    // Title
    titleLabel = new QLabel("Dashboard", this);
    QFont titleFont("Segoe UI", 42, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    contentLayout->addWidget(titleLabel);
    contentLayout->addSpacing(25);

    // Welcome Card - Modern Design
    welcomeCard = new QWidget(this);
    welcomeCard->setObjectName("WelcomeCard");
    welcomeCard->setFixedHeight(160);
    QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomeCard);
    welcomeLayout->setContentsMargins(34, 26, 34, 26);
    welcomeLayout->setSpacing(12);

    welcomeLabel = new QLabel("✨ Welcome to File Optimizer", welcomeCard);
    QFont welcomeFont("Segoe UI", 32, QFont::Bold);
    welcomeLabel->setFont(welcomeFont);
    welcomeLabel->setStyleSheet("color: #0ea5e9; background: transparent; letter-spacing: 1px;");
    welcomeLabel->setAlignment(Qt::AlignLeft);
    welcomeLayout->addWidget(welcomeLabel);

    descriptionLabel = new QLabel("Manage and optimize your files with ease. Use compression and decompression tools to reduce file sizes and improve storage efficiency.", welcomeCard);
    QFont descFont("Segoe UI", 14);
    descriptionLabel->setFont(descFont);
    descriptionLabel->setStyleSheet("color: #bae6fd; background: transparent; line-height: 1.6;");
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignLeft);
    welcomeLayout->addWidget(descriptionLabel);

    contentLayout->addWidget(welcomeCard);
    contentLayout->addSpacing(35);

    // Stats Cards Container
    statsContainer = new QWidget();
    statsLayout = new QHBoxLayout(statsContainer);
    statsLayout->setSpacing(18);
    statsLayout->setContentsMargins(0, 0, 0, 0);

    // Stat Card 1 - Modern Design with Icon
    statCard1 = new QWidget();
    statCard1->setObjectName("StatCard1");
    statCard1->setFixedHeight(200);
    QVBoxLayout *cardLayout1 = new QVBoxLayout(statCard1);
    cardLayout1->setContentsMargins(24, 20, 24, 20);
    cardLayout1->setSpacing(10);
    
    QLabel *icon1 = new QLabel("📊", statCard1);
    QFont iconFont("Segoe UI", 26);
    icon1->setFont(iconFont);
    icon1->setAlignment(Qt::AlignLeft);
    icon1->setStyleSheet("background: transparent;");
    icon1->setFixedHeight(35);
    cardLayout1->addWidget(icon1);
    
    statLabel1 = new QLabel("Total Files Compressed", statCard1);
    QFont statLabelFont("Segoe UI", 12, QFont::Medium);
    statLabel1->setFont(statLabelFont);
    statLabel1->setFixedHeight(20);
    cardLayout1->addWidget(statLabel1);
    
    statValue1 = new QLabel("0", statCard1);
    QFont statValueFont("Segoe UI", 34, QFont::Bold);
    statValue1->setFont(statValueFont);
    statValue1->setMinimumHeight(50);
    statValue1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    cardLayout1->addWidget(statValue1);
    cardLayout1->addStretch();

    // Stat Card 2 - Modern Design with Icon
    statCard2 = new QWidget();
    statCard2->setObjectName("StatCard2");
    statCard2->setFixedHeight(200);
    QVBoxLayout *cardLayout2 = new QVBoxLayout(statCard2);
    cardLayout2->setContentsMargins(24, 20, 24, 20);
    cardLayout2->setSpacing(10);
    
    QLabel *icon2 = new QLabel("💾", statCard2);
    icon2->setFont(iconFont);
    icon2->setAlignment(Qt::AlignLeft);
    icon2->setStyleSheet("background: transparent;");
    icon2->setFixedHeight(35);
    cardLayout2->addWidget(icon2);
    
    statLabel2 = new QLabel("Space Saved", statCard2);
    statLabel2->setFont(statLabelFont);
    statLabel2->setFixedHeight(20);
    cardLayout2->addWidget(statLabel2);
    
    statValue2 = new QLabel("0 MB", statCard2);
    statValue2->setFont(statValueFont);
    statValue2->setMinimumHeight(50);
    statValue2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    cardLayout2->addWidget(statValue2);
    cardLayout2->addStretch();

    // Stat Card 3 - Modern Design with Icon
    statCard3 = new QWidget();
    statCard3->setObjectName("StatCard3");
    statCard3->setFixedHeight(200);
    QVBoxLayout *cardLayout3 = new QVBoxLayout(statCard3);
    cardLayout3->setContentsMargins(24, 20, 24, 20);
    cardLayout3->setSpacing(10);
    
    QLabel *icon3 = new QLabel("📈", statCard3);
    icon3->setFont(iconFont);
    icon3->setAlignment(Qt::AlignLeft);
    icon3->setStyleSheet("background: transparent;");
    icon3->setFixedHeight(35);
    cardLayout3->addWidget(icon3);
    
    statLabel3 = new QLabel("Average Compression", statCard3);
    statLabel3->setFont(statLabelFont);
    statLabel3->setFixedHeight(20);
    cardLayout3->addWidget(statLabel3);
    
    statValue3 = new QLabel("0%", statCard3);
    statValue3->setFont(statValueFont);
    statValue3->setMinimumHeight(50);
    statValue3->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    cardLayout3->addWidget(statValue3);
    cardLayout3->addStretch();

    statsLayout->addWidget(statCard1);
    statsLayout->addWidget(statCard2);
    statsLayout->addWidget(statCard3);

    contentLayout->addWidget(statsContainer);
    contentLayout->addStretch(1);
}

void DashboardWindow::applyStyles()
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
    
    // Highlight the Dashboard button (active screen)
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
    
    dashboardButton->setStyleSheet(activeButtonStyle);

    QString contentStyle = QString(R"(
        QWidget {
            background: %1;
            color: %2;
        }
    )").arg(colors.bgGradientMid, colors.textTertiary);
    
    contentArea->setStyleSheet(contentStyle);

    QString titleStyle = QString(R"(
        QLabel {
            color: %1;
            background: transparent;
            text-shadow: 0 2px 15px %2;
            letter-spacing: 2px;
        }
    )").arg(colors.primary, colors.primary);
    
    titleLabel->setStyleSheet(titleStyle);

    QString welcomeCardStyle = QString(R"(
        QWidget#WelcomeCard {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 %1,
                stop:1 %2);
            border: 2px solid %3;
            border-radius: 25px;
            box-shadow: 0 8px 32px %4;
        }
    )").arg(colors.cardBg, colors.cardBg, colors.cardBorder, colors.primary);
    
    welcomeCard->setStyleSheet(welcomeCardStyle);

    QString welcomeLabelStyle = QString(R"(
        QLabel {
            color: %1;
            background: transparent;
            letter-spacing: 1px;
            text-shadow: 0 2px 10px %2;
        }
    )").arg(colors.primary, colors.primary);
    
    welcomeLabel->setStyleSheet(welcomeLabelStyle);

    QString descStyle = QString(R"(
        QLabel {
            color: %1;
            background: transparent;
            line-height: 1.6;
        }
    )").arg(colors.textTertiary);
    
    descriptionLabel->setStyleSheet(descStyle);

    // Stat Cards Styling - Modern Design with Shadows
    QString statCardStyle = QString(R"(
        QWidget#StatCard1, QWidget#StatCard2, QWidget#StatCard3 {
            background: %1;
            border: 2px solid %2;
            border-radius: 22px;
            box-shadow: 0 6px 24px %3;
        }
    )").arg(colors.cardBg, colors.cardBorder, colors.primary);
    
    statCard1->setStyleSheet(statCardStyle);
    statCard2->setStyleSheet(statCardStyle);
    statCard3->setStyleSheet(statCardStyle);

    QString statLabelStyle = QString(R"(
        QLabel {
            color: %1;
            background: transparent;
            font-weight: 500;
            letter-spacing: 0.5px;
        }
    )").arg(colors.textTertiary);
    
    statLabel1->setStyleSheet(statLabelStyle);
    statLabel2->setStyleSheet(statLabelStyle);
    statLabel3->setStyleSheet(statLabelStyle);

    QString statValueStyle = QString(R"(
        QLabel {
            color: %1;
            background: transparent;
            text-shadow: 0 2px 8px %2;
            min-height: 50px;
        }
    )").arg(colors.primary, colors.primary);
    
    statValue1->setStyleSheet(statValueStyle);
    statValue2->setStyleSheet(statValueStyle);
    statValue3->setStyleSheet(statValueStyle);
}

void DashboardWindow::setupAnimations()
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

    // Animate stat cards
    QGraphicsOpacityEffect *stat1Effect = new QGraphicsOpacityEffect(this);
    stat1Effect->setOpacity(0);
    statCard1->setGraphicsEffect(stat1Effect);
    
    QPropertyAnimation *stat1Anim = new QPropertyAnimation(stat1Effect, "opacity", this);
    stat1Anim->setDuration(1200);
    stat1Anim->setStartValue(0);
    stat1Anim->setEndValue(1);
    stat1Anim->setEasingCurve(QEasingCurve::OutCubic);

    QGraphicsOpacityEffect *stat2Effect = new QGraphicsOpacityEffect(this);
    stat2Effect->setOpacity(0);
    statCard2->setGraphicsEffect(stat2Effect);
    
    QPropertyAnimation *stat2Anim = new QPropertyAnimation(stat2Effect, "opacity", this);
    stat2Anim->setDuration(1200);
    stat2Anim->setStartValue(0);
    stat2Anim->setEndValue(1);
    stat2Anim->setEasingCurve(QEasingCurve::OutCubic);

    QGraphicsOpacityEffect *stat3Effect = new QGraphicsOpacityEffect(this);
    stat3Effect->setOpacity(0);
    statCard3->setGraphicsEffect(stat3Effect);
    
    QPropertyAnimation *stat3Anim = new QPropertyAnimation(stat3Effect, "opacity", this);
    stat3Anim->setDuration(1200);
    stat3Anim->setStartValue(0);
    stat3Anim->setEndValue(1);
    stat3Anim->setEasingCurve(QEasingCurve::OutCubic);

    QTimer::singleShot(600, [stat1Anim, stat2Anim, stat3Anim]() {
        stat1Anim->start();
        QTimer::singleShot(100, [stat2Anim]() { stat2Anim->start(); });
        QTimer::singleShot(200, [stat3Anim]() { stat3Anim->start(); });
    });
}

// =============================================================
// ======================= STATS MANAGEMENT =======================
// =============================================================

void DashboardWindow::loadStats()
{
    QSettings settings;
    totalFilesProcessed = settings.value("stats/totalFiles", 0).toInt();
    totalSpaceSaved = settings.value("stats/totalSpaceSaved", 0).toLongLong();
    totalCompressionRatio = settings.value("stats/totalCompressionRatio", 0.0).toDouble();
    
    refreshStatsDisplay();
}

void DashboardWindow::saveStats()
{
    QSettings settings;
    settings.setValue("stats/totalFiles", totalFilesProcessed);
    settings.setValue("stats/totalSpaceSaved", totalSpaceSaved);
    settings.setValue("stats/totalCompressionRatio", totalCompressionRatio);
}

void DashboardWindow::refreshStatsDisplay()
{
    // Update Total Files Compressed
    statValue1->setText(QString::number(totalFilesProcessed));
    
    // Update Space Saved (in MB)
    QString spaceSavedStr;
    if (totalSpaceSaved < 1024) {
        spaceSavedStr = QString::number(totalSpaceSaved) + " B";
    } else if (totalSpaceSaved < 1024 * 1024) {
        spaceSavedStr = QString::number(totalSpaceSaved / 1024.0, 'f', 1) + " KB";
    } else if (totalSpaceSaved < 1024 * 1024 * 1024) {
        spaceSavedStr = QString::number(totalSpaceSaved / (1024.0 * 1024.0), 'f', 1) + " MB";
    } else {
        spaceSavedStr = QString::number(totalSpaceSaved / (1024.0 * 1024.0 * 1024.0), 'f', 2) + " GB";
    }
    statValue2->setText(spaceSavedStr);
    
    // Update Average Compression
    double avgCompression = 0.0;
    if (totalFilesProcessed > 0) {
        avgCompression = totalCompressionRatio / totalFilesProcessed;
    }
    statValue3->setText(QString::number(avgCompression, 'f', 1) + "%");
}

void DashboardWindow::updateStats(qint64 originalSize, qint64 compressedSize)
{
    // Increment total files
    totalFilesProcessed++;
    
    // Calculate space saved
    qint64 spaceSaved = originalSize - compressedSize;
    if (spaceSaved > 0) {
        totalSpaceSaved += spaceSaved;
    }
    
    // Calculate compression ratio and add to total
    double compressionRatio = 100.0 * (1.0 - (double)compressedSize / originalSize);
    if (compressionRatio > 0) {
        totalCompressionRatio += compressionRatio;
    }
    
    // Save stats
    saveStats();
    
    // Refresh display
    refreshStatsDisplay();
}

void DashboardWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateResponsiveSizes();
}

void DashboardWindow::applyTheme()
{
    // Re-apply styles with current theme
    applyStyles();
}

void DashboardWindow::updateResponsiveSizes() {
    // Scale fonts and component sizes based on window width
    const int windowWidth = width();
    const double scale = qBound(0.60, static_cast<double>(windowWidth) / 1400.0, 1.0);

    // Sidebar width and padding
    const int sidebarWidth = qBound(180, static_cast<int>(210 * scale), 240);
    sidebar->setMinimumWidth(sidebarWidth);
    sidebar->setMaximumWidth(sidebarWidth);

    if (sidebarLayout) {
        const int sideMargin = qBound(12, static_cast<int>(18 * scale), 26);
        const int sideSpacing = qBound(8, static_cast<int>(13 * scale), 20);
        sidebarLayout->setContentsMargins(sideMargin, sideMargin + 10, sideMargin, sideMargin + 10);
        sidebarLayout->setSpacing(sideSpacing);
    }

    auto adjustNavButton = [scale](QPushButton *btn) {
        if (!btn) return;
        btn->setMinimumWidth(qBound(150, static_cast<int>(180 * scale), 230));
        btn->setMaximumWidth(qBound(180, static_cast<int>(230 * scale), 250));
        btn->setFixedHeight(qBound(38, static_cast<int>(46 * scale), 54));
        QFont f = btn->font();
        f.setPointSize(qBound(11, static_cast<int>(12 * scale), 13));
        btn->setFont(f);
    };

    adjustNavButton(backButton);
    adjustNavButton(dashboardButton);
    adjustNavButton(compressButton);
    adjustNavButton(decompressButton);
    adjustNavButton(visualizerButton);

    // Content padding/spacing
    if (contentLayout) {
        const int margin = qBound(22, static_cast<int>(30 * scale), 42);
        const int spacing = qBound(18, static_cast<int>(24 * scale), 32);
        contentLayout->setContentsMargins(margin, margin, margin, margin);
        contentLayout->setSpacing(spacing);
    }

    // Title font
    if (titleLabel) {
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(qBound(28, static_cast<int>(42 * scale), 50));
        titleLabel->setFont(titleFont);
    }

    // Welcome card sizing
    if (welcomeCard) {
        welcomeCard->setFixedHeight(qBound(130, static_cast<int>(160 * scale), 200));
    }
    if (welcomeLabel) {
        QFont f = welcomeLabel->font();
        f.setPointSize(qBound(22, static_cast<int>(32 * scale), 38));
        welcomeLabel->setFont(f);
    }
    if (descriptionLabel) {
        QFont f = descriptionLabel->font();
        f.setPointSize(qBound(12, static_cast<int>(14 * scale), 16));
        descriptionLabel->setFont(f);
    }

    // Stat cards sizing
    auto adjustStatCard = [scale](QWidget *card, QLabel *label, QLabel *value) {
        if (card) {
            card->setFixedHeight(qBound(160, static_cast<int>(200 * scale), 230));
            if (auto layout = card->layout()) {
                const int margin = qBound(18, static_cast<int>(22 * scale), 28);
                const int spacing = qBound(8, static_cast<int>(12 * scale), 16);
                layout->setContentsMargins(margin, margin - 2, margin, margin - 2);
                layout->setSpacing(spacing);
            }
        }
        if (label) {
            QFont f = label->font();
            f.setPointSize(qBound(11, static_cast<int>(12 * scale), 14));
            label->setFont(f);
        }
        if (value) {
            QFont f = value->font();
            f.setPointSize(qBound(24, static_cast<int>(34 * scale), 40));
            value->setFont(f);
        }
    };

    adjustStatCard(statCard1, statLabel1, statValue1);
    adjustStatCard(statCard2, statLabel2, statValue2);
    adjustStatCard(statCard3, statLabel3, statValue3);

    if (statsLayout) {
        statsLayout->setSpacing(qBound(12, static_cast<int>(18 * scale), 26));
    }
}

