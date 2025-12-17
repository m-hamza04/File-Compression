#include "abouthelp.h"
#include "theme.h"
#include <QFont>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>

AboutHelpWindow::AboutHelpWindow(QWidget *parent)
    : QWidget(parent)
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
}

void AboutHelpWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateResponsiveSizes();
}

void AboutHelpWindow::applyTheme()
{
    // Re-apply styles with current theme
    applyStyles();
}

void AboutHelpWindow::updateResponsiveSizes() {
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

void AboutHelpWindow::setupSidebar()
{
    sidebar = new QWidget();
    sidebar->setFixedWidth(250);
    sidebar->setObjectName("Sidebar");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
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
    QPushButton *dashboardButton = new QPushButton("📊 Dashboard", sidebar);
    dashboardButton->setFixedSize(200, 50);
    dashboardButton->setCursor(Qt::PointingHandCursor);
    dashboardButton->setObjectName("NavButton");

    QPushButton *compressButton = new QPushButton("📦 Compression", sidebar);
    compressButton->setFixedSize(200, 50);
    compressButton->setCursor(Qt::PointingHandCursor);
    compressButton->setObjectName("NavButton");

    QPushButton *decompressButton = new QPushButton("📤 Decompression", sidebar);
    decompressButton->setFixedSize(200, 50);
    decompressButton->setCursor(Qt::PointingHandCursor);
    decompressButton->setObjectName("NavButton");

    QPushButton *visualizerButton = new QPushButton("📈 Visualizer", sidebar);
    visualizerButton->setFixedSize(200, 50);
    visualizerButton->setCursor(Qt::PointingHandCursor);
    visualizerButton->setObjectName("NavButton");

    QPushButton *historyButton = new QPushButton("📜 History", sidebar);
    historyButton->setFixedSize(200, 50);
    historyButton->setCursor(Qt::PointingHandCursor);
    historyButton->setObjectName("NavButton");

    aboutHelpButton = new QPushButton("ℹ️ About", sidebar);
    aboutHelpButton->setFixedSize(200, 50);
    aboutHelpButton->setCursor(Qt::PointingHandCursor);
    aboutHelpButton->setObjectName("NavButton");

    sidebarLayout->addWidget(dashboardButton);
    sidebarLayout->addWidget(compressButton);
    sidebarLayout->addWidget(decompressButton);
    sidebarLayout->addWidget(visualizerButton);
    sidebarLayout->addWidget(historyButton);
    sidebarLayout->addWidget(aboutHelpButton);
    sidebarLayout->addStretch();

    // Connect navigation signals
    connect(backButton, &QPushButton::clicked, this, &AboutHelpWindow::navigateToSelection);
    connect(dashboardButton, &QPushButton::clicked, this, &AboutHelpWindow::navigateToDashboard);
    connect(compressButton, &QPushButton::clicked, this, &AboutHelpWindow::navigateToCompress);
    connect(decompressButton, &QPushButton::clicked, this, &AboutHelpWindow::navigateToDecompress);
    connect(visualizerButton, &QPushButton::clicked, this, &AboutHelpWindow::navigateToVisualizer);
    connect(historyButton, &QPushButton::clicked, this, &AboutHelpWindow::navigateToHistory);
    connect(aboutHelpButton, &QPushButton::clicked, this, &AboutHelpWindow::navigateToAboutHelp);
}

void AboutHelpWindow::setupContentArea()
{
    contentArea = new QWidget();
    contentArea->setObjectName("ContentArea");

    QVBoxLayout *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(80, 60, 80, 50);
    contentLayout->setSpacing(40);

    // Title
    titleLabel = new QLabel("About", contentArea);
    QFont titleFont("Segoe UI", 48, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    contentLayout->addWidget(titleLabel);
    contentLayout->addSpacing(40);

    // Scroll area for content
    scrollArea = new QScrollArea(contentArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setSpacing(30);

    // ========== WELCOME CARD ==========
    QWidget *welcomeCard = new QWidget(scrollContent);
    welcomeCard->setObjectName("WelcomeCard");
    welcomeCard->setFixedHeight(180);
    welcomeCard->setStyleSheet(R"(
        QWidget#WelcomeCard {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(14, 165, 233, 0.25),
                stop:1 rgba(56, 189, 248, 0.25));
            border: 2px solid rgba(14, 165, 233, 0.5);
            border-radius: 20px;
        }
    )");
    QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomeCard);
    welcomeLayout->setContentsMargins(40, 30, 40, 30);
    welcomeLayout->setSpacing(15);
    
    QLabel *welcomeTitle = new QLabel("✨ Welcome to File Optimizer", welcomeCard);
    QFont welcomeTitleFont("Segoe UI", 32, QFont::Bold);
    welcomeTitle->setFont(welcomeTitleFont);
    welcomeTitle->setStyleSheet("color: #0ea5e9; background: transparent;");
    welcomeLayout->addWidget(welcomeTitle);
    
    QLabel *welcomeDesc = new QLabel("Your all-in-one solution for efficient file compression and optimization", welcomeCard);
    welcomeDesc->setFont(QFont("Segoe UI", 16));
    welcomeDesc->setStyleSheet("color: #7dd3fc; background: transparent;");
    welcomeLayout->addWidget(welcomeDesc);
    
    scrollLayout->addWidget(welcomeCard);

    // ========== ABOUT SECTION CARD ==========
    QWidget *aboutCard = new QWidget(scrollContent);
    aboutCard->setObjectName("AboutCard");
    aboutCard->setStyleSheet(R"(
        QWidget#AboutCard {
            background: rgba(26, 14, 10, 0.6);
            border: 2px solid rgba(14, 165, 233, 0.4);
            border-radius: 18px;
        }
    )");
    QVBoxLayout *aboutCardLayout = new QVBoxLayout(aboutCard);
    aboutCardLayout->setContentsMargins(35, 30, 35, 30);
    aboutCardLayout->setSpacing(20);

    QLabel *aboutTitle = new QLabel("📖 About", aboutCard);
    QFont sectionFont("Segoe UI", 26, QFont::Bold);
    aboutTitle->setFont(sectionFont);
    aboutTitle->setStyleSheet("color: #0ea5e9; background: transparent;");
    aboutCardLayout->addWidget(aboutTitle);

    QLabel *aboutText = new QLabel(
        "File Optimizer is a powerful desktop application designed to help you compress and "
        "decompress files efficiently. Whether you're working with text files, images, or videos, "
        "our application provides a user-friendly interface to optimize your file storage.",
        aboutCard
    );
    aboutText->setFont(QFont("Segoe UI", 15));
    aboutText->setStyleSheet("color: #bae6fd; background: transparent; line-height: 1.9;");
    aboutText->setWordWrap(true);
    aboutCardLayout->addWidget(aboutText);
    aboutCardLayout->addSpacing(15);

    // Features in a grid-like card
    QWidget *featuresCard = new QWidget(aboutCard);
    featuresCard->setObjectName("FeaturesCard");
    featuresCard->setStyleSheet(R"(
        QWidget#FeaturesCard {
            background: rgba(30, 58, 138, 0.3);
            border: 1px solid rgba(14, 165, 233, 0.3);
            border-radius: 15px;
        }
    )");
    QVBoxLayout *featuresLayout = new QVBoxLayout(featuresCard);
    featuresLayout->setContentsMargins(25, 20, 25, 20);
    featuresLayout->setSpacing(12);

    QLabel *featuresTitle = new QLabel("🌟 Key Features", featuresCard);
    featuresTitle->setFont(QFont("Segoe UI", 18, QFont::Bold));
    featuresTitle->setStyleSheet("color: #38bdf8; background: transparent; margin-bottom: 10px;");
    featuresLayout->addWidget(featuresTitle);

    QStringList features = {
        "📦 Smart Compression - Reduce file sizes while maintaining quality",
        "📤 Fast Decompression - Quickly restore your compressed files",
        "📊 Dashboard - Track your compression statistics",
        "📈 Visualizer - View detailed compression analytics",
        "🎨 Modern UI - Beautiful orange-themed interface",
        "⚡ Fast Performance - Optimized algorithms for quick processing"
    };

    for (const QString &feature : features) {
        QLabel *featureLabel = new QLabel(feature, featuresCard);
        featureLabel->setFont(QFont("Segoe UI", 14));
        featureLabel->setStyleSheet("color: #7dd3fc; background: transparent; padding: 8px 0px;");
        featuresLayout->addWidget(featureLabel);
    }

    aboutCardLayout->addWidget(featuresCard);
    aboutCardLayout->addSpacing(15);

    // Version info card
    QWidget *versionCard = new QWidget(aboutCard);
    versionCard->setObjectName("VersionCard");
    versionCard->setStyleSheet(R"(
        QWidget#VersionCard {
            background: rgba(14, 165, 233, 0.15);
            border: 1px solid rgba(14, 165, 233, 0.3);
            border-radius: 12px;
        }
    )");
    QHBoxLayout *versionLayout = new QHBoxLayout(versionCard);
    versionLayout->setContentsMargins(25, 18, 25, 18);
    
    QLabel *versionLabel = new QLabel("Version: <b style='color:#0ea5e9;'>1.0.0</b>", versionCard);
    versionLabel->setFont(QFont("Segoe UI", 14, QFont::Medium));
    versionLabel->setStyleSheet("color: #bae6fd; background: transparent;");
    versionLayout->addWidget(versionLabel);
    versionLayout->addStretch();
    
    QLabel *techLabel = new QLabel("Built with <b style='color:#0ea5e9;'>Qt Framework</b>", versionCard);
    techLabel->setFont(QFont("Segoe UI", 14, QFont::Medium));
    techLabel->setStyleSheet("color: #bae6fd; background: transparent;");
    versionLayout->addWidget(techLabel);

    aboutCardLayout->addWidget(versionCard);
    scrollLayout->addWidget(aboutCard);

    // ========== HOW TO USE SECTION CARD ==========
    QWidget *howToCard = new QWidget(scrollContent);
    howToCard->setObjectName("HowToCard");
    howToCard->setStyleSheet(R"(
        QWidget#HowToCard {
            background: rgba(26, 14, 10, 0.6);
            border: 2px solid rgba(14, 165, 233, 0.4);
            border-radius: 18px;
        }
    )");
    QVBoxLayout *howToCardLayout = new QVBoxLayout(howToCard);
    howToCardLayout->setContentsMargins(35, 30, 35, 30);
    howToCardLayout->setSpacing(20);

    QLabel *howToTitle = new QLabel("🚀 How to Use", howToCard);
    howToTitle->setFont(sectionFont);
    howToTitle->setStyleSheet("color: #0ea5e9; background: transparent;");
    howToCardLayout->addWidget(howToTitle);

    // Compression Guide Card
    QWidget *compressCard = new QWidget(howToCard);
    compressCard->setObjectName("GuideCard");
    compressCard->setStyleSheet(R"(
        QWidget#GuideCard {
            background: rgba(30, 58, 138, 0.25);
            border: 1px solid rgba(14, 165, 233, 0.3);
            border-radius: 15px;
        }
    )");
    QVBoxLayout *compressCardLayout = new QVBoxLayout(compressCard);
    compressCardLayout->setContentsMargins(25, 20, 25, 20);
    compressCardLayout->setSpacing(12);

    QLabel *compressGuideTitle = new QLabel("📦 Compression", compressCard);
    QFont guideTitleFont("Segoe UI", 20, QFont::Bold);
    compressGuideTitle->setFont(guideTitleFont);
    compressGuideTitle->setStyleSheet("color: #38bdf8; background: transparent; margin-bottom: 10px;");
    compressCardLayout->addWidget(compressGuideTitle);

    QStringList compressSteps = {
        "1. Click on 'Compression' from the main menu",
        "2. Click 'Add File' button to select a file",
        "3. Choose the file you want to compress",
        "4. Review the file information displayed",
        "5. Click 'Start Compression' to begin",
        "6. Wait for the compression to complete",
        "7. View the compression ratio and saved space"
    };

    for (const QString &step : compressSteps) {
        QLabel *stepLabel = new QLabel(step, compressCard);
        stepLabel->setFont(QFont("Segoe UI", 14));
        stepLabel->setStyleSheet("color: #7dd3fc; background: transparent; padding: 6px 0px;");
        compressCardLayout->addWidget(stepLabel);
    }

    compressCardLayout->addSpacing(10);
    QLabel *formatsLabel = new QLabel("<b style='color:#38bdf8;'>Supported formats:</b> Text files (.txt, .doc, .docx) • Images (.jpg, .png, .bmp) • Videos (.mp4, .avi, .mov)", compressCard);
    formatsLabel->setFont(QFont("Segoe UI", 13));
    formatsLabel->setStyleSheet("color: #bae6fd; background: transparent; padding-top: 8px; border-top: 1px solid rgba(14, 165, 233, 0.3);");
    formatsLabel->setWordWrap(true);
    compressCardLayout->addWidget(formatsLabel);

    howToCardLayout->addWidget(compressCard);
    howToCardLayout->addSpacing(15);

    // Decompression Guide Card
    QWidget *decompressCard = new QWidget(howToCard);
    decompressCard->setObjectName("GuideCard2");
    decompressCard->setStyleSheet(R"(
        QWidget#GuideCard2 {
            background: rgba(30, 58, 138, 0.25);
            border: 1px solid rgba(14, 165, 233, 0.3);
            border-radius: 15px;
        }
    )");
    QVBoxLayout *decompressCardLayout = new QVBoxLayout(decompressCard);
    decompressCardLayout->setContentsMargins(25, 20, 25, 20);
    decompressCardLayout->setSpacing(12);

    QLabel *decompressGuideTitle = new QLabel("📤 Decompression", decompressCard);
    decompressGuideTitle->setFont(guideTitleFont);
    decompressGuideTitle->setStyleSheet("color: #38bdf8; background: transparent; margin-bottom: 10px;");
    decompressCardLayout->addWidget(decompressGuideTitle);

    QStringList decompressSteps = {
        "1. Click on 'Decompression' from the main menu",
        "2. Click 'Add File' button to select a compressed file",
        "3. Choose the compressed file (.compressed format)",
        "4. Review the file information displayed",
        "5. Click 'Start Decompression' to begin",
        "6. Wait for the decompression to complete",
        "7. Your original file will be restored"
    };

    for (const QString &step : decompressSteps) {
        QLabel *stepLabel = new QLabel(step, decompressCard);
        stepLabel->setFont(QFont("Segoe UI", 14));
        stepLabel->setStyleSheet("color: #7dd3fc; background: transparent; padding: 6px 0px;");
        decompressCardLayout->addWidget(stepLabel);
    }

    decompressCardLayout->addSpacing(10);
    QLabel *noteLabel = new QLabel("<b style='color:#38bdf8;'>Note:</b> Make sure the compressed file was created using this application.", decompressCard);
    noteLabel->setFont(QFont("Segoe UI", 13));
    noteLabel->setStyleSheet("color: #bae6fd; background: transparent; padding-top: 8px; border-top: 1px solid rgba(14, 165, 233, 0.3);");
    noteLabel->setWordWrap(true);
    decompressCardLayout->addWidget(noteLabel);

    howToCardLayout->addWidget(decompressCard);
    howToCardLayout->addSpacing(15);

    // Tips Card
    QWidget *tipsCard = new QWidget(howToCard);
    tipsCard->setObjectName("TipsCard");
    tipsCard->setStyleSheet(R"(
        QWidget#TipsCard {
            background: rgba(56, 189, 248, 0.15);
            border: 1px solid rgba(14, 165, 233, 0.3);
            border-radius: 15px;
        }
    )");
    QVBoxLayout *tipsCardLayout = new QVBoxLayout(tipsCard);
    tipsCardLayout->setContentsMargins(25, 20, 25, 20);
    tipsCardLayout->setSpacing(12);

    QLabel *tipsTitle = new QLabel("💡 Tips & Best Practices", tipsCard);
    tipsTitle->setFont(guideTitleFont);
    tipsTitle->setStyleSheet("color: #38bdf8; background: transparent; margin-bottom: 10px;");
    tipsCardLayout->addWidget(tipsTitle);

    QStringList tips = {
        "Always keep a backup of important files before compression",
        "Text files compress better than already compressed formats",
        "Large files may take longer to process",
        "Check the compression ratio to see space savings",
        "Use the Dashboard to track your compression history",
        "The Visualizer helps you understand compression patterns"
    };

    for (const QString &tip : tips) {
        QLabel *tipLabel = new QLabel("• " + tip, tipsCard);
        tipLabel->setFont(QFont("Segoe UI", 14));
        tipLabel->setStyleSheet("color: #7dd3fc; background: transparent; padding: 6px 0px;");
        tipLabel->setWordWrap(true);
        tipsCardLayout->addWidget(tipLabel);
    }

    howToCardLayout->addWidget(tipsCard);
    scrollLayout->addWidget(howToCard);

    scrollLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    contentLayout->addWidget(scrollArea);
}

void AboutHelpWindow::applyStyles()
{
    Theme::ThemeColors colors = Theme::getThemeColors(Theme::getCurrentTheme());
    
    // Main widget background
    QString mainStyle = QString(R"(
        QWidget {
            background: qlineargradient(
                x1: 0, y1: 0, x2: 1, y2: 1,
                stop: 0 %1,
                stop: 0.3 %2,
                stop: 0.6 %3,
                stop: 1 %4
            );
        }
    )").arg(colors.bgGradientStart, colors.bgGradientMid, colors.bgGradientEnd, colors.bgGradientStop3);
    
    setStyleSheet(mainStyle);

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

    // Highlight the About button (active screen)
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
    
    aboutHelpButton->setStyleSheet(activeButtonStyle);

    // Content area styling
    QString contentStyle = QString(R"(
        QWidget#ContentArea {
            background: %1;
            color: %2;
        }
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            background: %3;
            width: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background: %4;
            border-radius: 6px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: %5;
        }
    )").arg(colors.bgGradientMid, colors.textTertiary, colors.sidebarBg, colors.navButtonActive, colors.primary);
    
    contentArea->setStyleSheet(contentStyle);

    QString titleStyle = QString(R"(
        QLabel {
            color: %1;
            background: transparent;
            text-shadow: 0 0 20px %2;
        }
    )").arg(colors.primary, colors.primary);
    
    titleLabel->setStyleSheet(titleStyle);
}

void AboutHelpWindow::setupAnimations()
{
    // Animate sidebar fade-in
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

    // Animate title fade-in
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
}

