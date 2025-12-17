#include "history.h"
#include "theme.h"
#include <QFont>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QMessageBox>
#include <QTime>
#include <QFileInfo>
#include <algorithm>
#include "styledmessagebox.h"

HistoryWindow::HistoryWindow(QWidget *parent)
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
    
    // Load history AFTER UI is set up
    loadHistory();

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);

    applyStyles();
    setupAnimations();
    
    connect(clearButton, &QPushButton::clicked, this, &HistoryWindow::clearHistory);
}

HistoryWindow::~HistoryWindow() = default;

void HistoryWindow::setupSidebar()
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

    historyButton = new QPushButton("📜 History", sidebar);
    historyButton->setFixedSize(200, 50);
    historyButton->setCursor(Qt::PointingHandCursor);
    historyButton->setObjectName("NavButton");

    aboutButton = new QPushButton("ℹ️ About", sidebar);
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
    
    // Connect navigation signals (buttons created in setupSidebar)
    connect(backButton, &QPushButton::clicked, this, &HistoryWindow::navigateToSelection);
    connect(dashboardButton, &QPushButton::clicked, this, &HistoryWindow::navigateToDashboard);
    connect(compressButton, &QPushButton::clicked, this, &HistoryWindow::navigateToCompress);
    connect(decompressButton, &QPushButton::clicked, this, &HistoryWindow::navigateToDecompress);
    connect(visualizerButton, &QPushButton::clicked, this, &HistoryWindow::navigateToVisualizer);
    connect(historyButton, &QPushButton::clicked, this, &HistoryWindow::navigateToHistory);
    connect(aboutButton, &QPushButton::clicked, this, &HistoryWindow::navigateToAboutHelp);
}

void HistoryWindow::setupContentArea()
{
    contentArea = new QWidget();
    contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(60, 50, 60, 50);
    contentLayout->setSpacing(35);

    // Title with subtitle
    QWidget *titleContainer = new QWidget(contentArea);
    QVBoxLayout *titleLayout = new QVBoxLayout(titleContainer);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(8);
    
    titleLabel = new QLabel("History", titleContainer);
    QFont titleFont("Segoe UI", 52, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    titleLayout->addWidget(titleLabel);
    
    QLabel *subtitleLabel = new QLabel("View your compression and decompression history");
    QFont subtitleFont("Segoe UI", 14);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setStyleSheet("color: #7dd3fc; background: transparent;");
    subtitleLabel->setAlignment(Qt::AlignLeft);
    titleLayout->addWidget(subtitleLabel);
    
    contentLayout->addWidget(titleContainer);
    contentLayout->addSpacing(30);

    // Filter Dropdown Card
    QWidget *filterCard = new QWidget(contentArea);
    filterCard->setObjectName("FilterCard");
    QHBoxLayout *filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setContentsMargins(20, 15, 20, 15);
    filterLayout->setSpacing(15);
    
    QLabel *filterLabel = new QLabel("🔍 Filter by:", filterCard);
    QFont filterLabelFont("Segoe UI", 15, QFont::Bold);
    filterLabel->setFont(filterLabelFont);
    filterLabel->setStyleSheet("color: #ffffff; background: transparent;");
    
    filterComboBox = new QComboBox(filterCard);
    filterComboBox->addItem("📋 All");
    filterComboBox->addItem("📦 Compress");
    filterComboBox->addItem("📤 Decompress");
    filterComboBox->setFixedSize(220, 50);
    filterComboBox->setCursor(Qt::PointingHandCursor);
    filterComboBox->setObjectName("FilterCombo");
    
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(filterComboBox);
    filterLayout->addStretch();
    contentLayout->addWidget(filterCard);
    contentLayout->addSpacing(25);
    
    // Connect filter dropdown
    connect(filterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &HistoryWindow::onFilterChanged);
    currentFilter = "All";

    // History Table Container Card
    QWidget *tableCard = new QWidget(contentArea);
    tableCard->setObjectName("TableCard");
    QVBoxLayout *tableCardLayout = new QVBoxLayout(tableCard);
    tableCardLayout->setContentsMargins(0, 0, 0, 0);
    tableCardLayout->setSpacing(0);
    
    // History Table
    historyTable = new QTableWidget(tableCard);
    historyTable->setColumnCount(7); // Will be adjusted based on filter
    QStringList headers = {"Date & Time", "File Type", "Original Size", "Compressed Size", "Decompressed Size", "Reduction/Expansion", "File Path"};
    historyTable->setHorizontalHeaderLabels(headers);
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setSelectionMode(QAbstractItemView::SingleSelection);
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    historyTable->setAlternatingRowColors(true);
    historyTable->setShowGrid(false);
    historyTable->verticalHeader()->setVisible(false);
    historyTable->horizontalHeader()->setStretchLastSection(true);
    historyTable->setColumnWidth(0, 180);
    historyTable->setColumnWidth(1, 120);
    historyTable->setColumnWidth(2, 130);
    historyTable->setColumnWidth(3, 130);
    historyTable->setColumnWidth(4, 130);
    historyTable->setColumnWidth(5, 140);
    tableCardLayout->addWidget(historyTable);
    contentLayout->addWidget(tableCard);

    // Clear Button
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    clearButton = new QPushButton("🗑️ Clear History", contentArea);
    clearButton->setFixedSize(200, 50);
    clearButton->setCursor(Qt::PointingHandCursor);
    clearButton->setObjectName("ActionButton");
    buttonLayout->addStretch();
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();
    contentLayout->addLayout(buttonLayout);
}

void HistoryWindow::applyStyles()
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
    
    // Highlight the History button (active screen)
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
    
    historyButton->setStyleSheet(activeButtonStyle);

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

    // Table Card Styling
    QWidget *tableCard = contentArea->findChild<QWidget*>("TableCard");
    if (tableCard) {
        QString cardStyle = QString(R"(
            QWidget#TableCard {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 rgba(14, 165, 233, 0.15),
                    stop:1 rgba(2, 132, 199, 0.12));
                border: 2px solid %1;
                border-radius: 20px;
                padding: 15px;
                box-shadow: 0 8px 32px rgba(14, 165, 233, 0.25);
            }
        )").arg(colors.cardBorder);
        tableCard->setStyleSheet(cardStyle);
    }
    
    QString tableStyle = QString(R"(
        QTableWidget {
            background: transparent;
            border: none;
            gridline-color: %1;
            color: %2;
            font-size: 14px;
            selection-background-color: %3;
        }
        QTableWidget::item {
            padding: 12px 8px;
            border: none;
            border-bottom: 1px solid rgba(14, 165, 233, 0.1);
        }
        QTableWidget::item:selected {
            background: %4;
            color: white;
            border-radius: 8px;
        }
        QTableWidget::item:hover {
            background: rgba(14, 165, 233, 0.1);
            border-radius: 8px;
        }
        QHeaderView::section {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 %5,
                stop:1 %6);
            color: %7;
            font-weight: bold;
            font-size: 15px;
            padding: 14px 8px;
            border: none;
            border-bottom: 3px solid %8;
            border-right: 1px solid rgba(14, 165, 233, 0.2);
        }
        QHeaderView::section:first {
            border-top-left-radius: 10px;
        }
        QHeaderView::section:last {
            border-top-right-radius: 10px;
            border-right: none;
        }
        QTableWidget::item:alternate {
            background: rgba(14, 165, 233, 0.05);
        }
    )").arg(colors.sidebarBorder, colors.textTertiary, colors.navButtonActive,
            colors.navButtonActive, colors.navButtonActive, colors.primaryDark,
            colors.primary, colors.primary);
    
    historyTable->setStyleSheet(tableStyle);
    
    // Filter Card Styling
    QWidget *filterCard = contentArea->findChild<QWidget*>("FilterCard");
    if (filterCard) {
        QString filterCardStyle = QString(R"(
            QWidget#FilterCard {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 rgba(14, 165, 233, 0.2),
                    stop:1 rgba(2, 132, 199, 0.15));
                border: 2px solid %1;
                border-radius: 18px;
                box-shadow: 0 4px 20px rgba(14, 165, 233, 0.2);
            }
        )").arg(colors.cardBorder);
        filterCard->setStyleSheet(filterCardStyle);
    }
    
    // Combo Box Styling
    if (filterComboBox) {
        QString comboStyle = QString(R"(
            QComboBox#FilterCombo {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 %1, stop:1 %2);
                color: white;
                border: 2px solid %3;
                border-radius: 15px;
                padding: 10px 20px;
                font-size: 15px;
                font-weight: bold;
                min-width: 180px;
            }
            QComboBox#FilterCombo:hover {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 %4, stop:1 %5);
                border: 2px solid %6;
                box-shadow: 0 4px 15px rgba(14, 165, 233, 0.4);
            }
            QComboBox#FilterCombo::drop-down {
                border: none;
                width: 35px;
                border-left: 1px solid rgba(255, 255, 255, 0.3);
            }
            QComboBox#FilterCombo::down-arrow {
                image: none;
                border-left: 6px solid transparent;
                border-right: 6px solid transparent;
                border-top: 8px solid white;
                width: 0;
                height: 0;
                margin-right: 8px;
            }
            QComboBox#FilterCombo QAbstractItemView {
                background: %7;
                border: 2px solid %8;
                border-radius: 10px;
                selection-background-color: %9;
                selection-color: white;
                color: %10;
                padding: 5px;
            }
            QComboBox#FilterCombo QAbstractItemView::item {
                padding: 10px 15px;
                border-radius: 5px;
            }
            QComboBox#FilterCombo QAbstractItemView::item:hover {
                background: rgba(14, 165, 233, 0.3);
            }
        )").arg(colors.buttonBg, colors.primaryDark, colors.buttonBorder,
                colors.primary, colors.primaryDark, colors.primaryLight,
                colors.cardBg, colors.cardBorder, colors.navButtonActive, colors.textTertiary);
        filterComboBox->setStyleSheet(comboStyle);
    }

    // Clear Button Styling
    if (clearButton) {
        QString clearButtonStyle = QString(R"(
            QPushButton#ActionButton {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 %1, stop:1 %2);
                color: white;
                border-radius: 18px;
                border: 3px solid %3;
                padding: 14px 24px;
                font-weight: bold;
                font-size: 16px;
                box-shadow: 0 8px 32px rgba(14, 165, 233, 0.4);
                letter-spacing: 0.5px;
            }
            QPushButton#ActionButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 %4, stop:1 %5);
                border: 3px solid %6;
                box-shadow: 0 12px 40px rgba(14, 165, 233, 0.6);
                transform: translateY(-2px);
            }
            QPushButton#ActionButton:pressed {
                background: %7;
                transform: translateY(0px);
            }
        )").arg(colors.buttonBg, colors.primaryDark, colors.buttonBorder,
                colors.primary, colors.primaryDark, colors.primaryLight, colors.primaryDark);
        clearButton->setStyleSheet(clearButtonStyle);
    }
}

void HistoryWindow::setupAnimations()
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
}

void HistoryWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateResponsiveSizes();
}

void HistoryWindow::applyTheme()
{
    // Re-apply styles with current theme
    applyStyles();
}

void HistoryWindow::updateResponsiveSizes() {
    int windowWidth = width();
    double scale = qBound(0.7, (double)windowWidth / 1920.0, 1.5);
    
    if (titleLabel) {
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize((int)(52 * scale));
        titleLabel->setFont(titleFont);
    }
}

void HistoryWindow::loadHistory()
{
    QSettings settings;
    int count = settings.value("history/recordCount", 0).toInt();
    
    historyRecords.clear();
    for (int i = 0; i < count; ++i) {
        QString key = QString("history/record%1").arg(i);
        HistoryRecord record;
        record.type = settings.value(key + "/type", "Compress").toString(); // Default to Compress for old records
        record.date = QDate::fromString(settings.value(key + "/date").toString(), Qt::ISODate);
        record.time = QTime::fromString(settings.value(key + "/time").toString(), "hh:mm:ss");
        record.fileType = settings.value(key + "/fileType").toString();
        record.filePath = settings.value(key + "/filePath").toString();
        record.originalSize = settings.value(key + "/originalSize").toLongLong();
        record.compressedSize = settings.value(key + "/compressedSize").toLongLong();
        record.compressionRatio = settings.value(key + "/ratio").toDouble();
        
        if (record.date.isValid() && !record.fileType.isEmpty()) {
            historyRecords.append(record);
        }
    }
    
    // Keep only last 500 records
    if (historyRecords.size() > 500) {
        historyRecords = historyRecords.mid(historyRecords.size() - 500);
        saveHistory();
    }
    
    refreshHistoryTable();
}

void HistoryWindow::saveHistory()
{
    QSettings settings;
    settings.remove("history");
    
    settings.setValue("history/recordCount", historyRecords.size());
    for (int i = 0; i < historyRecords.size(); ++i) {
        QString key = QString("history/record%1").arg(i);
        const HistoryRecord &record = historyRecords[i];
        settings.setValue(key + "/type", record.type);
        settings.setValue(key + "/date", record.date.toString(Qt::ISODate));
        settings.setValue(key + "/time", record.time.toString("hh:mm:ss"));
        settings.setValue(key + "/fileType", record.fileType);
        settings.setValue(key + "/filePath", record.filePath);
        settings.setValue(key + "/originalSize", record.originalSize);
        settings.setValue(key + "/compressedSize", record.compressedSize);
        settings.setValue(key + "/ratio", record.compressionRatio);
    }
}

void HistoryWindow::refreshHistoryTable()
{
    if (!historyTable) return; // Safety check
    
    // Filter records based on currentFilter
    QList<HistoryRecord> filteredRecords;
    for (const HistoryRecord &record : historyRecords) {
        if (currentFilter == "All" || record.type == currentFilter) {
            filteredRecords.append(record);
        }
    }
    
    // Update table headers based on filter - always show all 7 columns for "All"
    QStringList headers = {"Date & Time", "File Type", "Original Size", "Compressed Size", "Decompressed Size", "Reduction/Expansion", "File Path"};
    historyTable->setHorizontalHeaderLabels(headers);
    
    // Show/hide columns based on filter
    if (currentFilter == "Compress") {
        historyTable->hideColumn(4); // Hide Decompressed Size
        historyTable->setColumnWidth(5, 140); // Reduction column
    } else if (currentFilter == "Decompress") {
        historyTable->hideColumn(2); // Hide Original Size
        historyTable->setColumnWidth(5, 140); // Expansion column
    } else {
        // "All" - show all columns
        historyTable->showColumn(2); // Original Size
        historyTable->showColumn(4); // Decompressed Size
        historyTable->setColumnWidth(5, 160); // Reduction/Expansion column
    }
    
    historyTable->setRowCount(filteredRecords.size());
    
    // Sort by date/time (newest first)
    std::sort(filteredRecords.begin(), filteredRecords.end(), [](const HistoryRecord &a, const HistoryRecord &b) {
        if (a.date != b.date) return a.date > b.date;
        return a.time > b.time;
    });
    
    for (int i = 0; i < filteredRecords.size(); ++i) {
        const HistoryRecord &record = filteredRecords[i];
        
        // Date & Time
        QString dateTimeStr = record.date.toString("yyyy-MM-dd") + " " + record.time.toString("hh:mm:ss");
        QTableWidgetItem *dateItem = new QTableWidgetItem(dateTimeStr);
        dateItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        historyTable->setItem(i, 0, dateItem);
        
        // File Type
        QTableWidgetItem *typeItem = new QTableWidgetItem(record.fileType);
        typeItem->setTextAlignment(Qt::AlignCenter);
        historyTable->setItem(i, 1, typeItem);
        
        // Original Size (Column 2)
        QString originalStr;
        if (record.type == "Compress") {
            originalStr = formatFileSize(record.originalSize);
        } else {
            originalStr = "-"; // Decompress records don't have original size
        }
        QTableWidgetItem *origItem = new QTableWidgetItem(originalStr);
        origItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        historyTable->setItem(i, 2, origItem);
        
        // Compressed Size (Column 3) - always shown
        QString compressedStr = formatFileSize(record.originalSize); // For decompress, originalSize stores compressed size
        if (record.type == "Decompress") {
            compressedStr = formatFileSize(record.originalSize);
        } else {
            compressedStr = formatFileSize(record.compressedSize);
        }
        QTableWidgetItem *compItem = new QTableWidgetItem(compressedStr);
        compItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        historyTable->setItem(i, 3, compItem);
        
        // Decompressed Size (Column 4)
        QString decompressedStr;
        if (record.type == "Decompress") {
            decompressedStr = formatFileSize(record.compressedSize); // For decompress, compressedSize stores decompressed size
        } else {
            decompressedStr = "-"; // Compress records don't have decompressed size
        }
        QTableWidgetItem *decompItem = new QTableWidgetItem(decompressedStr);
        decompItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        historyTable->setItem(i, 4, decompItem);
        
        // Ratio (Reduction for compress, Expansion for decompress) - Column 5
        QString ratioStr = QString::number(record.compressionRatio, 'f', 2) + "%";
        QTableWidgetItem *ratioItem = new QTableWidgetItem(ratioStr);
        ratioItem->setTextAlignment(Qt::AlignCenter);
        historyTable->setItem(i, 5, ratioItem);
        
        // File Path - Column 6
        QFileInfo fileInfo(record.filePath);
        QString displayPath = fileInfo.fileName();
        if (displayPath.isEmpty()) displayPath = "N/A";
        QTableWidgetItem *pathItem = new QTableWidgetItem(displayPath);
        pathItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        historyTable->setItem(i, 6, pathItem);
    }
}

void HistoryWindow::onFilterChanged(int index)
{
    // Handle emoji in dropdown items
    if (index == 0) {
        currentFilter = "All";
    } else if (index == 1) {
        currentFilter = "Compress";
    } else if (index == 2) {
        currentFilter = "Decompress";
    }
    refreshHistoryTable();
}

void HistoryWindow::addCompressionRecord(const QString &fileType, qint64 originalSize, qint64 compressedSize, const QString &filePath)
{
    if (originalSize <= 0) return;
    
    HistoryRecord record;
    record.type = "Compress";
    record.date = QDate::currentDate();
    record.time = QTime::currentTime();
    record.fileType = fileType;
    record.filePath = filePath;
    record.originalSize = originalSize;
    record.compressedSize = compressedSize;
    record.compressionRatio = 100.0 * (1.0 - (double)compressedSize / originalSize);
    
    historyRecords.append(record);
    
    // Keep only last 500 records
    if (historyRecords.size() > 500) {
        historyRecords = historyRecords.mid(historyRecords.size() - 500);
    }
    
    saveHistory();
    refreshHistoryTable();
}

void HistoryWindow::addDecompressionRecord(const QString &fileType, qint64 compressedSize, qint64 decompressedSize, const QString &filePath)
{
    if (compressedSize <= 0 || decompressedSize <= 0) return;
    
    HistoryRecord record;
    record.type = "Decompress";
    record.date = QDate::currentDate();
    record.time = QTime::currentTime();
    record.fileType = fileType;
    record.filePath = filePath;
    record.originalSize = compressedSize; // Store compressed size in originalSize field
    record.compressedSize = decompressedSize; // Store decompressed size in compressedSize field
    record.compressionRatio = 100.0 * ((double)decompressedSize / compressedSize - 1.0); // Expansion percentage
    
    historyRecords.append(record);
    
    // Keep only last 500 records
    if (historyRecords.size() > 500) {
        historyRecords = historyRecords.mid(historyRecords.size() - 500);
    }
    
    saveHistory();
    refreshHistoryTable();
}

void HistoryWindow::clearHistory()
{
    QWidget *parentWindow = this->window();
    QMessageBox *confirmBox = createStyledMessageBox(parentWindow, "⚠️ Confirm Clear",
                         "⚠️ Are you sure you want to clear all history?\n\nThis action cannot be undone.", 
                         QMessageBox::Question);
    confirmBox->addButton("Yes", QMessageBox::YesRole);
    confirmBox->addButton("No", QMessageBox::NoRole);
    
    int result = confirmBox->exec();
    delete confirmBox;
    
    if (result == 0) { // Yes clicked
        historyRecords.clear();
        saveHistory();
        refreshHistoryTable();
        
        QMessageBox *msgBox = createStyledMessageBox(parentWindow, "✅ History Cleared",
                             "✅ All history records have been cleared.", QMessageBox::Information);
        msgBox->exec();
        delete msgBox;
    }
}

