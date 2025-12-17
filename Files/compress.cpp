#include "compress.h"
#include "theme.h"
#include "huffman.h"
#include "imagecom.h"

#include <QDebug>
#include <QFont>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QLocale>
#include <QApplication>
#include <algorithm>
#include <QPainter>
#include <QtCharts/QLegend>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QFileInfo>
#include <cstdlib> // Required for std::rand, std::srand
#include <ctime>   // Required for std::time
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QSizePolicy>
#include "styledmessagebox.h"

CompressWindow::CompressWindow(QWidget *parent)
    : QWidget(parent)
{
    // Seed the random number generator once when the application starts
    std::srand(std::time(nullptr));

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

    // Ensure sidebar is visible immediately
    sidebar->show();
    sidebar->raise();
    sidebar->setVisible(true);

    applyStyles();
    setupAnimations();

    // Initialize size labels
    originalSizeLabel->setText("--");
    compressedSizeLabel->setText("--");

    // --- Signal/Slot Connections ---
    connect(addFileButton, &QPushButton::clicked, this, [=]() {
        QWidget *parentWindow = this->window();
        QString filePath = QFileDialog::getOpenFileName(parentWindow,
                                                        tr("Select File to Compress"),
                                                        QDir::homePath(),
                                                        tr("All Files (*.*)"));
        if (!filePath.isEmpty()) {
            selectedFilePath = filePath;
            QFileInfo fileInfo(filePath);
            filePathLabel->setText(fileInfo.fileName());
            // Update file status
            QFile file(filePath);
            qint64 fileSize = file.size();
            // Format file size
            QString sizeStr;
            if (fileSize < 1024) {
                sizeStr = QString::number(fileSize) + " B";
            } else if (fileSize < 1024 * 1024) {
                sizeStr = QString::number(fileSize / 1024.0, 'f', 1) + " KB";
            } else if (fileSize < 1024 * 1024 * 1024) {
                sizeStr = QString::number(fileSize / (1024.0 * 1024.0), 'f', 1) + " MB";
            } else {
                sizeStr = QString::number(fileSize / (1024.0 * 1024.0 * 1024.0), 'f', 2) + " GB";
            }
            originalSizeLabel->setText(sizeStr);
        } else {
            selectedFilePath.clear();
            filePathLabel->setText("No file selected");
            originalSizeLabel->setText("--");
            compressedSizeLabel->setText("--");
        }
    });

    connect(startButton, &QPushButton::clicked, this, [=]() {
        if (selectedFilePath.isEmpty()) {
            QWidget *parentWindow = this->window();
            QMessageBox *msgBox = createStyledMessageBox(parentWindow, "⚠️ No File Selected",
                                     "⚠️ Please select a file first.", QMessageBox::Warning);
            msgBox->exec();
            delete msgBox;
            return;
        }

        startButton->setEnabled(false);
        QApplication::processEvents();

        compressSelectedFile(selectedFilePath);

        startButton->setEnabled(true);
    });

    // Navigation connections are now in setupSidebar()
}

CompressWindow::~CompressWindow() = default;

// =============================================================
// ====================== UI SETUP METHODS =======================
// =============================================================

void CompressWindow::setupSidebar()
{
    sidebar = new QWidget();
    sidebar->setMinimumWidth(200);
    sidebar->setMaximumWidth(260);
    sidebar->setObjectName("Sidebar");
    sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(20, 30, 20, 30);
    sidebarLayout->setSpacing(15);

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
    
    // Connect navigation signals
    connect(backButton, &QPushButton::clicked, this, &CompressWindow::navigateToSelection);
    connect(dashboardButton, &QPushButton::clicked, this, &CompressWindow::navigateToDashboard);
    connect(compressButton, &QPushButton::clicked, this, &CompressWindow::navigateToCompress);
    connect(decompressButton, &QPushButton::clicked, this, &CompressWindow::navigateToDecompress);
    connect(visualizerButton, &QPushButton::clicked, this, &CompressWindow::navigateToVisualizer);
    connect(historyButton, &QPushButton::clicked, this, &CompressWindow::navigateToHistory);
    connect(aboutButton, &QPushButton::clicked, this, &CompressWindow::navigateToAboutHelp);
}

void CompressWindow::setupContentArea()
{
    contentArea = new QWidget();
    contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(32, 28, 32, 28);
    contentLayout->setSpacing(24);

    // Title with subtitle
    QWidget *titleContainer = new QWidget();
    QVBoxLayout *titleLayout = new QVBoxLayout(titleContainer);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(8);
    
    titleLabel = new QLabel("Compression");
    QFont titleFont("Segoe UI", 42, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    titleLayout->addWidget(titleLabel);
    
    QLabel *subtitleLabel = new QLabel("Reduce file sizes efficiently with smart compression");
    QFont subtitleFont("Segoe UI", 12);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setStyleSheet("color: #7dd3fc; background: transparent;");
    subtitleLabel->setAlignment(Qt::AlignLeft);
    titleLayout->addWidget(subtitleLabel);
    
    contentLayout->addWidget(titleContainer);
    contentLayout->addSpacing(30);

    // Select File Button - Modern Design
    QHBoxLayout *fileButtonCenterLayout = new QHBoxLayout();
    fileButtonCenterLayout->addStretch(1);
    
    addFileButton = new QPushButton("📁 Select File to Compress...");
    addFileButton->setMinimumWidth(340);
    addFileButton->setMaximumWidth(620);
    addFileButton->setFixedHeight(78);
    addFileButton->setCursor(Qt::PointingHandCursor);
    QFont buttonFont("Segoe UI", 18, QFont::Bold);
    addFileButton->setFont(buttonFont);
    
    fileButtonCenterLayout->addWidget(addFileButton);
    fileButtonCenterLayout->addStretch(1);
    contentLayout->addLayout(fileButtonCenterLayout);
    contentLayout->addSpacing(35);

    // File Info Card (Modern Card Design)
    QWidget *fileInfoCard = new QWidget();
    fileInfoCard->setObjectName("FileInfoCard");
    QHBoxLayout *fileInfoLayout = new QHBoxLayout(fileInfoCard);
    fileInfoLayout->setContentsMargins(28, 24, 28, 24);
    fileInfoLayout->setSpacing(28);
    
    // File icon/thumbnail area
    QLabel *fileIconLabel = new QLabel("📦");
    QFont iconFont("Segoe UI", 32);
    fileIconLabel->setFont(iconFont);
    fileIconLabel->setAlignment(Qt::AlignCenter);
    fileIconLabel->setFixedSize(64, 64);
    fileIconLabel->setStyleSheet("background: rgba(14, 165, 233, 0.15); border-radius: 15px;");
    fileInfoLayout->addWidget(fileIconLabel);
    
    // File details
    QVBoxLayout *fileDetailsLayout = new QVBoxLayout();
    fileDetailsLayout->setSpacing(8);
    
    filePathLabel = new QLabel("No file selected");
    QFont filePathFont("Segoe UI", 17, QFont::Bold);
    filePathLabel->setFont(filePathFont);
    filePathLabel->setStyleSheet("color: #ffffff; background: transparent; letter-spacing: 0.5px;");
    fileDetailsLayout->addWidget(filePathLabel);
    
    QLabel *fileStatusLabel = new QLabel("Select a file to compress");
    QFont statusFont("Segoe UI", 12);
    fileStatusLabel->setFont(statusFont);
    fileStatusLabel->setStyleSheet("color: #bae6fd; background: transparent;");
    fileDetailsLayout->addWidget(fileStatusLabel);
    
    fileInfoLayout->addLayout(fileDetailsLayout);
    fileInfoLayout->addStretch();
    
    // Size info cards (Modern Design)
    QWidget *sizeCard1 = new QWidget();
    sizeCard1->setObjectName("SizeCard1");
    sizeCard1->setMinimumSize(150, 92);
    sizeCard1->setMaximumSize(220, 130);
    QVBoxLayout *sizeCard1Layout = new QVBoxLayout(sizeCard1);
    sizeCard1Layout->setContentsMargins(20, 18, 20, 18);
    sizeCard1Layout->setSpacing(10);
    
    QLabel *sizeLabel1 = new QLabel("Original Size");
    sizeLabel1->setFont(QFont("Segoe UI", 11, QFont::Medium));
    sizeLabel1->setStyleSheet("color: #bae6fd; background: transparent;");
    sizeLabel1->setAlignment(Qt::AlignCenter);
    sizeCard1Layout->addWidget(sizeLabel1);
    
    originalSizeLabel = new QLabel("--");
    originalSizeLabel->setFont(QFont("Segoe UI", 15, QFont::Bold));
    originalSizeLabel->setStyleSheet("color: #ffffff; background: transparent;");
    originalSizeLabel->setAlignment(Qt::AlignCenter);
    sizeCard1Layout->addWidget(originalSizeLabel);
    
    QWidget *sizeCard2 = new QWidget();
    sizeCard2->setObjectName("SizeCard2");
    sizeCard2->setMinimumSize(150, 92);
    sizeCard2->setMaximumSize(220, 130);
    QVBoxLayout *sizeCard2Layout = new QVBoxLayout(sizeCard2);
    sizeCard2Layout->setContentsMargins(20, 18, 20, 18);
    sizeCard2Layout->setSpacing(10);
    
    QLabel *sizeLabel2 = new QLabel("Compressed Size");
    sizeLabel2->setFont(QFont("Segoe UI", 11, QFont::Medium));
    sizeLabel2->setStyleSheet("color: #bae6fd; background: transparent;");
    sizeLabel2->setAlignment(Qt::AlignCenter);
    sizeCard2Layout->addWidget(sizeLabel2);
    
    compressedSizeLabel = new QLabel("--");
    compressedSizeLabel->setFont(QFont("Segoe UI", 15, QFont::Bold));
    compressedSizeLabel->setStyleSheet("color: #ffffff; background: transparent;");
    compressedSizeLabel->setAlignment(Qt::AlignCenter);
    sizeCard2Layout->addWidget(compressedSizeLabel);
    
    fileInfoLayout->addWidget(sizeCard1);
    fileInfoLayout->addSpacing(20);
    fileInfoLayout->addWidget(sizeCard2);
    
    contentLayout->addWidget(fileInfoCard);
    contentLayout->addSpacing(30);

    // Save Location Selector Card
    QWidget *saveLocationCard = new QWidget();
    saveLocationCard->setObjectName("SaveLocationCard");
    QVBoxLayout *saveLocationLayout = new QVBoxLayout(saveLocationCard);
    saveLocationLayout->setContentsMargins(22, 18, 22, 18);
    saveLocationLayout->setSpacing(14);
    
    QLabel *saveLocationTitle = new QLabel("💾 Save Location");
    QFont saveLocationTitleFont("Segoe UI", 16, QFont::Bold);
    saveLocationTitle->setFont(saveLocationTitleFont);
    saveLocationTitle->setStyleSheet("color: #ffffff; background: transparent;");
    saveLocationLayout->addWidget(saveLocationTitle);
    
    QHBoxLayout *saveLocationButtonLayout = new QHBoxLayout();
    saveLocationButtonLayout->setSpacing(15);
    
    selectSaveLocationButton = new QPushButton("📂 Select Save Location...");
    selectSaveLocationButton->setMinimumWidth(220);
    selectSaveLocationButton->setMaximumWidth(360);
    selectSaveLocationButton->setFixedHeight(56);
    selectSaveLocationButton->setCursor(Qt::PointingHandCursor);
    QFont saveLocationButtonFont("Segoe UI", 14, QFont::Bold);
    selectSaveLocationButton->setFont(saveLocationButtonFont);
    saveLocationButtonLayout->addWidget(selectSaveLocationButton);
    
    saveLocationLabel = new QLabel("Default: Same as source file");
    QFont saveLocationLabelFont("Segoe UI", 13);
    saveLocationLabel->setFont(saveLocationLabelFont);
    saveLocationLabel->setStyleSheet("color: #bae6fd; background: transparent; padding: 10px;");
    saveLocationLabel->setWordWrap(true);
    saveLocationButtonLayout->addWidget(saveLocationLabel, 1);
    
    saveLocationLayout->addLayout(saveLocationButtonLayout);
    contentLayout->addWidget(saveLocationCard);
    contentLayout->addSpacing(30);
    
    // Connect save location button
    connect(selectSaveLocationButton, &QPushButton::clicked, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Select Save Location"), 
                                                        saveLocationPath.isEmpty() ? QDir::homePath() : saveLocationPath);
        if (!dir.isEmpty()) {
            saveLocationPath = dir;
            saveLocationLabel->setText("📍 " + dir);
        }
    });

    // Start Button (Centered, Large, Modern)
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    startButton = new QPushButton("🚀 Start Compression");
    startButton->setMinimumWidth(260);
    startButton->setMaximumWidth(420);
    startButton->setFixedHeight(74);
    startButton->setCursor(Qt::PointingHandCursor);
    QFont startFont("Segoe UI", 20, QFont::Bold);
    startButton->setFont(startFont);
    
    buttonLayout->addWidget(startButton);
    buttonLayout->addStretch();
    
    contentLayout->addLayout(buttonLayout);
    contentLayout->addSpacing(30);
    contentLayout->addStretch(1);
}

void CompressWindow::applyStyles()
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
    
    // Highlight the Compression button (active screen)
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
    
    compressButton->setStyleSheet(activeButtonStyle);

    QString contentStyle = QString(R"(
        QWidget {
            background: %1;
            color: %2;
        }
    )").arg(colors.bgGradientMid, colors.textTertiary);
    
    contentArea->setStyleSheet(contentStyle);

    // File Info Card Styling - Modern Design (matching Decompression screen)
    QWidget *fileInfoCard = contentArea->findChild<QWidget*>("FileInfoCard");
    if (fileInfoCard) {
        fileInfoCard->setStyleSheet(R"(
            QWidget#FileInfoCard {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 rgba(14, 165, 233, 0.25),
                    stop:1 rgba(2, 132, 199, 0.2));
                border: 2px solid rgba(14, 165, 233, 0.5);
                border-radius: 22px;
                box-shadow: 0 6px 24px rgba(14, 165, 233, 0.25);
            }
        )");
    }
    
    QWidget *sizeCard1 = contentArea->findChild<QWidget*>("SizeCard1");
    if (sizeCard1) {
        sizeCard1->setStyleSheet(R"(
            QWidget#SizeCard1 {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 rgba(30, 58, 138, 0.35),
                    stop:1 rgba(30, 58, 138, 0.25));
                border: 2px solid rgba(14, 165, 233, 0.5);
                border-radius: 18px;
                box-shadow: 0 4px 16px rgba(14, 165, 233, 0.2);
            }
        )");
    }
    
    QWidget *sizeCard2 = contentArea->findChild<QWidget*>("SizeCard2");
    if (sizeCard2) {
        sizeCard2->setStyleSheet(R"(
            QWidget#SizeCard2 {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 rgba(56, 189, 248, 0.3),
                    stop:1 rgba(14, 165, 233, 0.25));
                border: 2px solid rgba(14, 165, 233, 0.5);
                border-radius: 18px;
                box-shadow: 0 4px 16px rgba(14, 165, 233, 0.2);
            }
        )");
    }
    
    // Save Location Card Styling
    QWidget *saveLocationCard = contentArea->findChild<QWidget*>("SaveLocationCard");
    if (saveLocationCard) {
        QString saveLocationCardStyle = QString(R"(
            QWidget#SaveLocationCard {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 rgba(14, 165, 233, 0.2),
                    stop:1 rgba(2, 132, 199, 0.15));
                border: 2px solid %1;
                border-radius: 20px;
                box-shadow: 0 4px 20px rgba(14, 165, 233, 0.2);
            }
        )").arg(colors.cardBorder);
        saveLocationCard->setStyleSheet(saveLocationCardStyle);
    }
    
    // Save Location Button Styling
    if (selectSaveLocationButton) {
        QString saveLocationButtonStyle = QString(R"(
            QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 %1, stop:1 %2);
                color: white;
                font-size: 14px;
                font-weight: bold;
                border-radius: 15px;
                border: 2px solid %3;
                padding: 12px;
                box-shadow: 0 4px 15px rgba(14, 165, 233, 0.3);
            }
            QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 %4, stop:1 %5);
                border: 2px solid %6;
                box-shadow: 0 6px 20px rgba(14, 165, 233, 0.4);
            }
            QPushButton:pressed {
                background: %7;
            }
        )").arg(colors.buttonBg, colors.primaryDark, colors.buttonBorder,
                colors.primary, colors.primaryDark, colors.primaryLight, colors.primaryDark);
        selectSaveLocationButton->setStyleSheet(saveLocationButtonStyle);
    }

    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #0ea5e9;
            font-weight: bold;
            letter-spacing: 3px;
            text-shadow: 0 3px 20px rgba(14, 165, 233, 0.6);
        }
    )");
    
    // Subtitle styling is already set in setupContentArea

    addFileButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #38bdf8, stop:1 #0284c7);
            color: white;
            font-size: 18px;
            font-weight: bold;
            border-radius: 24px;
            border: 3px solid #0ea5e9;
            padding: 16px;
            box-shadow: 0 10px 35px rgba(14, 165, 233, 0.45);
            letter-spacing: 0.5px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #0ea5e9, stop:1 #0284c7);
            border: 3px solid #7dd3fc;
            box-shadow: 0 14px 45px rgba(14, 165, 233, 0.65);
        }
        QPushButton:pressed {
            background: #0284c7;
            border: 3px solid #38bdf8;
            box-shadow: 0 6px 20px rgba(14, 165, 233, 0.4);
        }
    )");

    startButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #0ea5e9, stop:1 #0284c7);
            color: white;
            font-size: 20px;
            font-weight: bold;
            border-radius: 25px;
            border: 3px solid #38bdf8;
            padding: 16px;
            box-shadow: 0 10px 35px rgba(14, 165, 233, 0.45);
            letter-spacing: 0.5px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #38bdf8, stop:1 #0ea5e9);
            border: 3px solid #7dd3fc;
            box-shadow: 0 14px 45px rgba(14, 165, 233, 0.65);
        }
        QPushButton:pressed {
            background: #0284c7;
            border: 3px solid #0ea5e9;
            box-shadow: 0 6px 20px rgba(14, 165, 233, 0.4);
        }
        QPushButton:disabled {
            background-color: rgba(14, 165, 233, 0.3);
            color: rgba(255, 255, 255, 0.5);
            border: 3px solid rgba(14, 165, 233, 0.2);
            box-shadow: none;
        }
    )");

    // Card styling is now done directly in setupContentArea
}

void CompressWindow::setupAnimations()
{
    // Sidebar is immediately visible - no fade animation to ensure it's always shown
    sidebar->show();
    sidebar->raise();

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

    // Animate file info card fade-in
    QWidget *fileInfoCard = contentArea->findChild<QWidget*>("FileInfoCard");
    if (fileInfoCard) {
        QGraphicsOpacityEffect *cardEffect = new QGraphicsOpacityEffect(this);
        cardEffect->setOpacity(0);
        fileInfoCard->setGraphicsEffect(cardEffect);
        
        QPropertyAnimation *cardAnim = new QPropertyAnimation(cardEffect, "opacity", this);
        cardAnim->setDuration(1000);
        cardAnim->setStartValue(0);
        cardAnim->setEndValue(1);
        cardAnim->setEasingCurve(QEasingCurve::OutCubic);
        
        QTimer::singleShot(600, [cardAnim]() {
            cardAnim->start();
        });
    }

}


// =============================================================
// ===================== COMPRESSION LOGIC (FINAL DISPATCH) =====================
// =============================================================
void CompressWindow::compressSelectedFile(const QString &path) {
    QFileInfo fileInfo(path);
    QString extension = fileInfo.suffix().toLower();

    QLocale locale;
    qint64 originalSize = fileInfo.size();

    // --- FILE TYPE CATEGORIZATION ---
    bool isVideo = (extension == "mp4" || extension == "avi" || extension == "mov" || extension == "mkv");
    bool isImage = (extension == "png" || extension == "bmp" || extension == "tiff" || extension == "tif" || extension == "jpg" || extension == "jpeg");
    bool isTextData = (extension == "txt" || extension == "pdf");

    // --- 1. HANDLE IMAGE AND VIDEO FILES (OpenCV Lossy Compression) ---
    if (isImage || isVideo) {

        QString fileType = isVideo ? "Video" : "Image";
        QString outputExtension = isVideo ? "avi" : "jpg";
        // Use selected save location if set, otherwise use file's directory
        QString outputDir = saveLocationPath.isEmpty() ? fileInfo.absolutePath() : saveLocationPath;
        QString outPath = QDir(outputDir).filePath(fileInfo.baseName() + "_compressed." + outputExtension);

        bool success = false;
        int quality = isVideo ? 70 : 85;

        if (isVideo) {
            success = ImageCompressor::compressAndSaveVideo(path.toStdString(), outPath.toStdString(), quality);
        } else {
            success = ImageCompressor::compressAndSaveSingleImage(path.toStdString(), outPath.toStdString(), quality);
        }

        QFile outFile(outPath);
        qint64 compressedSize = outFile.exists() ? outFile.size() : originalSize;

        if (success) {
            if (compressedSize >= originalSize && compressedSize != 0) {
                QWidget *parentWindow = this->window();
                QMessageBox *msgBox = createStyledMessageBox(parentWindow, "ℹ️ " + fileType + " Compression Skipped",
                                         QString("⚠️ %1 compression was skipped.\n\nReason: Output size was not smaller than the original.\n\n📊 Original Size: <b style='color: #0ea5e9;'>%2 Bytes</b>")
                                             .arg(fileType)
                                             .arg(locale.toString(originalSize)), QMessageBox::Warning);
                msgBox->exec();
                delete msgBox;
                updateCompressionChart(originalSize, originalSize);
                QFile::remove(outPath);
                return;
            }

            double ratio = 100.0 * (1.0 - (double)compressedSize / originalSize);

            QWidget *parentWindow = this->window();
            QString originalSizeStr = formatFileSize(originalSize);
            QString compressedSizeStr = formatFileSize(compressedSize);
            QString reductionStr = QString::number(ratio, 'f', 2) + "%";
            QMessageBox *msgBox = createStyledMessageBox(parentWindow, "✅ Success (" + fileType + ")",
                                     QString("<div style='font-size: 20px; font-weight: bold; color: #0ea5e9; margin-bottom: 20px; text-align: center;'>🎉 %1 Compression finished!</div>"
                                             "<div style='margin: 15px 0; padding: 15px; background: rgba(14, 165, 233, 0.1); border-radius: 12px;'>"
                                             "<div style='margin: 10px 0;'><b style='color: #bae6fd;'>📁 Saved to:</b><br><span style='color: #ffffff; font-size: 14px;'>%2</span></div>"
                                             "<div style='margin: 10px 0;'><b style='color: #bae6fd;'>📊 Original Size:</b> <span style='color: #0ea5e9; font-weight: bold; font-size: 16px;'>%3</span></div>"
                                             "<div style='margin: 10px 0;'><b style='color: #bae6fd;'>📊 Compressed Size:</b> <span style='color: #0ea5e9; font-weight: bold; font-size: 16px;'>%4</span></div>"
                                             "<div style='margin: 10px 0;'><b style='color: #bae6fd;'>📈 Reduction:</b> <span style='color: #0ea5e9; font-size: 18px; font-weight: bold;'>%5</span></div>"
                                             "<div style='margin: 10px 0;'><b style='color: #bae6fd;'>⚙️ Quality:</b> <span style='color: #ffffff;'>%6</span></div>"
                                             "</div>")
                                         .arg(fileType)
                                         .arg(outPath)
                                         .arg(originalSizeStr)
                                         .arg(compressedSizeStr)
                                         .arg(reductionStr)
                                         .arg(quality));
            msgBox->exec();
            delete msgBox;

            updateCompressionChart(originalSize, compressedSize);
            
            // Store file path for history
            setProperty("lastCompressedFile", path);
            
            // Emit signal to update dashboard and visualizer
            emit compressionCompleted(originalSize, compressedSize);
            emit compressionCompletedWithType(fileType, originalSize, compressedSize);
        } else {
            QWidget *parentWindow = this->window();
            QMessageBox *msgBox = createStyledMessageBox(parentWindow, "❌ Error",
                                     QString("❌ %1 Compression failed!\n\nPlease check console for OpenCV errors.").arg(fileType), QMessageBox::Critical);
            msgBox->exec();
            delete msgBox;
            updateCompressionChart(originalSize, originalSize);
        }
        return;
    }

    // --- 2. HANDLE TEXT/PDF FILES (Huffman/Fake Compression) ---
    if (isTextData) {

        QFile file(path);
        if (!file.open(QFile::ReadOnly)) {
            QWidget *parentWindow = this->window();
            QMessageBox *msgBox = createStyledMessageBox(parentWindow, "❌ Error",
                                     "❌ Cannot open input file for reading.\n\nPlease check file permissions.", QMessageBox::Critical);
            msgBox->exec();
            delete msgBox;
            return;
        }

        QByteArray data = file.readAll();
        file.close();

        std::vector<uint8_t> inputBytes(data.begin(), data.end());

        if (inputBytes.empty()) {
            QWidget *parentWindow = this->window();
            QMessageBox *msgBox = createStyledMessageBox(parentWindow, "⚠️ Empty File",
                                     "⚠️ The selected file is empty.\n\nNothing to compress.", QMessageBox::Warning);
            msgBox->exec();
            delete msgBox;
            // Reset size labels
            originalSizeLabel->setText("--");
            compressedSizeLabel->setText("--");
            return;
        }

        qint64 compressedSize = 0;
        QString outPath;
        bool successfulCompression = false;
        // Use selected save location if set, otherwise use file's directory
        QString outputDir = saveLocationPath.isEmpty() ? fileInfo.absolutePath() : saveLocationPath;
        outPath = QDir(outputDir).filePath(fileInfo.fileName() + ".huff"); // Consistent output extension


        // --- HUFFMAN LOGIC FOR PDF AND TXT ---
        if (extension == "pdf" || extension == "txt") {
            std::vector<uint8_t> compressed;

            bool ok = Huffman::compressBytes(inputBytes, compressed);

            if (!ok) {
                QWidget *parentWindow = this->window();
                QMessageBox *msgBox = createStyledMessageBox(parentWindow, "❌ Error",
                                         "❌ Huffman Compression failed!\n\nPlease try again.", QMessageBox::Critical);
                msgBox->exec();
                delete msgBox;
                return;
            }

            compressedSize = (qint64)compressed.size();
            successfulCompression = true;

            // CRITICAL CHECK: Only save if size is STRICTLY reduced (Huffman is lossless)
            if (compressedSize >= originalSize) {
                QWidget *parentWindow = this->window();
                QMessageBox *msgBox = createStyledMessageBox(parentWindow, "ℹ️ Compression Skipped",
                                         QString("⚠️ Huffman compression was skipped.\n\nReason: Size did not decrease for .%1 files.\n\n📊 Original Size: <b style='color: #0ea5e9;'>%2 Bytes</b>")
                                             .arg(extension.toUpper())
                                             .arg(locale.toString(originalSize)), QMessageBox::Warning);
                msgBox->exec();
                delete msgBox;
                updateCompressionChart(originalSize, originalSize);
                return;
            }

            // Write actual Huffman output file
            QFile outFile(outPath);
            if (!outFile.open(QFile::WriteOnly)) {
                QWidget *parentWindow = this->window();
                QMessageBox *msgBox = createStyledMessageBox(parentWindow, "❌ Error",
                                         "❌ Cannot open output file for writing.\n\nPlease check file permissions.", QMessageBox::Critical);
                msgBox->exec();
                delete msgBox;
                return;
            }
            outFile.write(reinterpret_cast<const char*>(compressed.data()), compressedSize);
            outFile.close();
        }


        // --- REPORT SUCCESS ---
        if (successfulCompression) {
            double ratio = 100.0 * (1.0 - (double)compressedSize / originalSize);

            QWidget *parentWindow = this->window();
            QString originalSizeStr = formatFileSize(originalSize);
            QString compressedSizeStr = formatFileSize(compressedSize);
            QString reductionStr = QString::number(ratio, 'f', 2) + "%";
            QMessageBox *msgBox = createStyledMessageBox(parentWindow, "✅ Success (Data File)",
                                     QString("<div style='font-size: 20px; font-weight: bold; color: #0ea5e9; margin-bottom: 20px; text-align: center;'>🎉 Huffman Compression finished!</div>"
                                             "<div style='margin: 15px 0; padding: 15px; background: rgba(14, 165, 233, 0.1); border-radius: 12px;'>"
                                             "<div style='margin: 10px 0;'><b style='color: #bae6fd;'>📁 Saved to:</b><br><span style='color: #ffffff; font-size: 14px;'>%1</span></div>"
                                             "<div style='margin: 10px 0;'><b style='color: #bae6fd;'>📊 Original Size:</b> <span style='color: #0ea5e9; font-weight: bold; font-size: 16px;'>%2</span></div>"
                                             "<div style='margin: 10px 0;'><b style='color: #bae6fd;'>📊 Compressed Size:</b> <span style='color: #0ea5e9; font-weight: bold; font-size: 16px;'>%3</span></div>"
                                             "<div style='margin: 10px 0;'><b style='color: #bae6fd;'>📈 Reduction:</b> <span style='color: #0ea5e9; font-size: 18px; font-weight: bold;'>%4</span></div>"
                                             "</div>")
                                         .arg(outPath)
                                         .arg(originalSizeStr)
                                         .arg(compressedSizeStr)
                                         .arg(reductionStr));
            msgBox->exec();
            delete msgBox;

            updateCompressionChart(originalSize, compressedSize);
            
            // Determine file type for text/PDF
            QString fileType = (extension == "pdf") ? "PDF" : "Text";
            
            // Store file path for history
            setProperty("lastCompressedFile", path);
            
            // Emit signal to update dashboard and visualizer
            emit compressionCompleted(originalSize, compressedSize);
            emit compressionCompletedWithType(fileType, originalSize, compressedSize);
        }
        return;
    }

    // --- 3. FALLBACK / UNSUPPORTED FILE TYPE ---
    QWidget *parentWindow = this->window();
    QMessageBox *msgBox = createStyledMessageBox(parentWindow, "⚠️ Unsupported File Type",
                         QString("⚠️ File type '.%1' is not supported.\n\n✅ Supported formats:\n• .txt files\n• .pdf files\n• Images (jpg, png, etc.)\n• Videos (mp4, avi, etc.)")
                             .arg(extension), QMessageBox::Warning);
    msgBox->exec();
    delete msgBox;
    updateCompressionChart(originalSize, originalSize);
}


// =============================================================
// ======================= CHART UPDATE ========================
// =============================================================
void CompressWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateResponsiveSizes();
}

void CompressWindow::applyTheme()
{
    // Re-apply styles with current theme
    applyStyles();
}

void CompressWindow::updateResponsiveSizes() {
    // Scale fonts and component sizes based on window width
    const int windowWidth = width();
    const double scale = qBound(0.60, static_cast<double>(windowWidth) / 1400.0, 1.0);

    // Sidebar width stays bounded but adapts on narrower screens
    const int sidebarWidth = qBound(180, static_cast<int>(210 * scale), 240);
    sidebar->setMinimumWidth(sidebarWidth);
    sidebar->setMaximumWidth(sidebarWidth);

    // Sidebar padding/spacing
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
        const int spacing = qBound(16, static_cast<int>(22 * scale), 30);
        contentLayout->setContentsMargins(margin, margin, margin, margin);
        contentLayout->setSpacing(spacing);
    }

    // Title font
    if (titleLabel) {
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(qBound(28, static_cast<int>(42 * scale), 50));
        titleLabel->setFont(titleFont);
    }

    // Primary CTA/button sizes
    if (addFileButton) {
        addFileButton->setMinimumWidth(qBound(300, static_cast<int>(390 * scale), 620));
        addFileButton->setMaximumWidth(qBound(380, static_cast<int>(620 * scale), 700));
        addFileButton->setFixedHeight(qBound(62, static_cast<int>(78 * scale), 90));
        QFont f = addFileButton->font();
        f.setPointSize(qBound(13, static_cast<int>(17 * scale), 19));
        addFileButton->setFont(f);
    }

    if (startButton) {
        startButton->setMinimumWidth(qBound(210, static_cast<int>(280 * scale), 400));
        startButton->setMaximumWidth(qBound(300, static_cast<int>(450 * scale), 500));
        startButton->setFixedHeight(qBound(58, static_cast<int>(70 * scale), 86));
        QFont f = startButton->font();
        f.setPointSize(qBound(15, static_cast<int>(18 * scale), 21));
        startButton->setFont(f);
    }

    if (selectSaveLocationButton) {
        selectSaveLocationButton->setMinimumWidth(qBound(180, static_cast<int>(220 * scale), 340));
        selectSaveLocationButton->setMaximumWidth(qBound(240, static_cast<int>(340 * scale), 400));
        selectSaveLocationButton->setFixedHeight(qBound(48, static_cast<int>(54 * scale), 68));
        QFont f = selectSaveLocationButton->font();
        f.setPointSize(qBound(12, static_cast<int>(13 * scale), 15));
        selectSaveLocationButton->setFont(f);
    }

    // File info/icon sizing
    if (filePathLabel) {
        QFont f = filePathLabel->font();
        f.setPointSize(qBound(13, static_cast<int>(17 * scale), 20));
        filePathLabel->setFont(f);
    }

    if (originalSizeLabel) {
        QFont f = originalSizeLabel->font();
        f.setPointSize(qBound(12, static_cast<int>(15 * scale), 18));
        originalSizeLabel->setFont(f);
    }

    if (compressedSizeLabel) {
        QFont f = compressedSizeLabel->font();
        f.setPointSize(qBound(12, static_cast<int>(15 * scale), 18));
        compressedSizeLabel->setFont(f);
    }
}

void CompressWindow::updateCompressionChart(qint64 originalSize, qint64 compressedSize)
{
    // Update size labels in cards only (chart removed)
    QString originalSizeStr = formatFileSize(originalSize);
    QString compressedSizeStr = formatFileSize(compressedSize);
    
    originalSizeLabel->setText(originalSizeStr);
    compressedSizeLabel->setText(compressedSizeStr);
}
