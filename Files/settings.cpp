#include "settings.h"
#include "theme.h"
#include <QFont>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include "styledmessagebox.h"

SettingsWindow::SettingsWindow(QWidget *parent)
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
    mainLayout->addWidget(contentArea, 1);

    // Load saved settings first (so theme is set)
    loadSettings();
    
    // Apply styles with loaded theme
    applyStyles();
    setupAnimations();
}

SettingsWindow::~SettingsWindow() = default;

void SettingsWindow::setupSidebar()
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

    settingsButton = new QPushButton("⚙️ Settings", sidebar);
    settingsButton->setFixedSize(200, 50);
    settingsButton->setCursor(Qt::PointingHandCursor);
    settingsButton->setObjectName("NavButton");

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
    sidebarLayout->addWidget(settingsButton);
    sidebarLayout->addWidget(historyButton);
    sidebarLayout->addWidget(aboutButton);
    sidebarLayout->addStretch();
    
    // Connect navigation signals (buttons created in setupSidebar)
    connect(backButton, &QPushButton::clicked, this, &SettingsWindow::navigateToSelection);
    connect(dashboardButton, &QPushButton::clicked, this, &SettingsWindow::navigateToDashboard);
    connect(compressButton, &QPushButton::clicked, this, &SettingsWindow::navigateToCompress);
    connect(decompressButton, &QPushButton::clicked, this, &SettingsWindow::navigateToDecompress);
    connect(visualizerButton, &QPushButton::clicked, this, &SettingsWindow::navigateToVisualizer);
    connect(settingsButton, &QPushButton::clicked, this, &SettingsWindow::navigateToSettings);
    connect(historyButton, &QPushButton::clicked, this, &SettingsWindow::navigateToHistory);
    connect(aboutButton, &QPushButton::clicked, this, &SettingsWindow::navigateToAboutHelp);
}

void SettingsWindow::setupContentArea()
{
    contentArea = new QWidget();
    contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(60, 50, 60, 50);
    contentLayout->setSpacing(35);

    // Title with subtitle
    QWidget *titleContainer = new QWidget(this);
    QVBoxLayout *titleLayout = new QVBoxLayout(titleContainer);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(10);
    
    titleLabel = new QLabel("⚙️ Settings", titleContainer);
    QFont titleFont("Segoe UI", 56, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    titleLayout->addWidget(titleLabel);
    
    QLabel *subtitleLabel = new QLabel("Customize your compression preferences and application settings", titleContainer);
    QFont subtitleFont("Segoe UI", 16);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setStyleSheet("color: #7dd3fc; background: transparent;");
    subtitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    titleLayout->addWidget(subtitleLabel);
    
    contentLayout->addWidget(titleContainer);
    contentLayout->addSpacing(35);

    // Scroll Area for settings
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setSpacing(30);

    // Compression Settings Group
    compressionGroup = new QGroupBox("🎯 Compression Settings", scrollContent);
    compressionGroup->setObjectName("SettingsGroup");
    QVBoxLayout *compLayout = new QVBoxLayout(compressionGroup);
    compLayout->setContentsMargins(40, 35, 40, 35);
    compLayout->setSpacing(30);

    // Image Quality
    QHBoxLayout *imgQualLayout = new QHBoxLayout();
    QLabel *imgQualLabel = new QLabel("🖼️ Image Quality:", compressionGroup);
    imgQualLabel->setStyleSheet("color: #7dd3fc; font-size: 17px; font-weight: bold; padding: 5px 0px;");
    imgQualLabel->setMinimumWidth(240);
    imageQualitySpin = new QSpinBox(compressionGroup);
    imageQualitySpin->setRange(1, 100);
    imageQualitySpin->setValue(85);
    imageQualitySpin->setSuffix("%");
    imageQualitySpin->setStyleSheet(R"(
        QSpinBox {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(14, 165, 233, 0.4),
                stop:1 rgba(2, 132, 199, 0.35));
            color: #ffffff;
            border: 2px solid rgba(14, 165, 233, 0.6);
            border-radius: 15px;
            padding: 12px 18px;
            font-size: 16px;
            font-weight: bold;
            min-width: 140px;
        }
        QSpinBox:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(14, 165, 233, 0.5),
                stop:1 rgba(2, 132, 199, 0.45));
            border: 2px solid #0ea5e9;
        }
        QSpinBox:focus {
            border: 2px solid #38bdf8;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            background: rgba(14, 165, 233, 0.3);
            border: none;
            border-radius: 5px;
            width: 25px;
        }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background: rgba(14, 165, 233, 0.5);
        }
    )");
    imgQualLayout->addWidget(imgQualLabel);
    imgQualLayout->addWidget(imageQualitySpin);
    imgQualLayout->addStretch();
    compLayout->addLayout(imgQualLayout);

    // Video Quality
    QHBoxLayout *vidQualLayout = new QHBoxLayout();
    QLabel *vidQualLabel = new QLabel("🎬 Video Quality:", compressionGroup);
    vidQualLabel->setStyleSheet("color: #7dd3fc; font-size: 17px; font-weight: bold; padding: 5px 0px;");
    vidQualLabel->setMinimumWidth(240);
    videoQualitySpin = new QSpinBox(compressionGroup);
    videoQualitySpin->setRange(1, 100);
    videoQualitySpin->setValue(70);
    videoQualitySpin->setSuffix("%");
    videoQualitySpin->setStyleSheet(R"(
        QSpinBox {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(14, 165, 233, 0.4),
                stop:1 rgba(2, 132, 199, 0.35));
            color: #ffffff;
            border: 2px solid rgba(14, 165, 233, 0.6);
            border-radius: 15px;
            padding: 12px 18px;
            font-size: 16px;
            font-weight: bold;
            min-width: 140px;
        }
        QSpinBox:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(14, 165, 233, 0.5),
                stop:1 rgba(2, 132, 199, 0.45));
            border: 2px solid #0ea5e9;
        }
        QSpinBox:focus {
            border: 2px solid #38bdf8;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            background: rgba(14, 165, 233, 0.3);
            border: none;
            border-radius: 5px;
            width: 25px;
        }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background: rgba(14, 165, 233, 0.5);
        }
    )");
    vidQualLayout->addWidget(vidQualLabel);
    vidQualLayout->addWidget(videoQualitySpin);
    vidQualLayout->addStretch();
    compLayout->addLayout(vidQualLayout);

    scrollLayout->addWidget(compressionGroup);

    // General Settings Group
    generalGroup = new QGroupBox("⚙️ General Settings", scrollContent);
    generalGroup->setObjectName("SettingsGroup");
    QVBoxLayout *genLayout = new QVBoxLayout(generalGroup);
    genLayout->setContentsMargins(40, 35, 40, 35);
    genLayout->setSpacing(30);

    // Default Save Path
    QHBoxLayout *pathLayout = new QHBoxLayout();
    QLabel *pathLabel = new QLabel("📁 Default Save Location:", generalGroup);
    pathLabel->setStyleSheet("color: #7dd3fc; font-size: 17px; font-weight: bold; padding: 5px 0px;");
    pathLabel->setMinimumWidth(240);
    defaultPathEdit = new QLineEdit(generalGroup);
    defaultPathEdit->setPlaceholderText("Select default save location...");
    defaultPathEdit->setStyleSheet(R"(
        QLineEdit {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(14, 165, 233, 0.4),
                stop:1 rgba(2, 132, 199, 0.35));
            color: #ffffff;
            border: 2px solid rgba(14, 165, 233, 0.6);
            border-radius: 15px;
            padding: 14px 18px;
            font-size: 16px;
            selection-background-color: rgba(14, 165, 233, 0.6);
            selection-color: #ffffff;
        }
        QLineEdit:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(14, 165, 233, 0.5),
                stop:1 rgba(2, 132, 199, 0.45));
            border: 2px solid #0ea5e9;
        }
        QLineEdit:focus {
            border: 2px solid #38bdf8;
        }
    )");
    browsePathButton = new QPushButton("Browse...", generalGroup);
    browsePathButton->setFixedSize(120, 42);
    browsePathButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #38bdf8, stop:1 #0284c7);
            color: white;
            border: 2px solid #0ea5e9;
            border-radius: 15px;
            padding: 12px 24px;
            font-weight: bold;
            font-size: 15px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #0ea5e9, stop:1 #0ea5e9);
            border: 2px solid #7dd3fc;
        }
        QPushButton:pressed {
            background: #0284c7;
        }
    )");
    pathLayout->addWidget(pathLabel);
    pathLayout->addWidget(defaultPathEdit);
    pathLayout->addWidget(browsePathButton);
    genLayout->addLayout(pathLayout);

    // Theme Selection
    QHBoxLayout *themeLayout = new QHBoxLayout();
    QLabel *themeLabel = new QLabel("🎨 Theme:", generalGroup);
    themeLabel->setStyleSheet("color: #7dd3fc; font-size: 17px; font-weight: bold; padding: 5px 0px;");
    themeLabel->setMinimumWidth(240);
    themeCombo = new QComboBox(generalGroup);
    themeCombo->addItems({"Blue-Teal", "Purple", "Orange", "Dark"});
    themeCombo->setStyleSheet(R"(
        QComboBox {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(14, 165, 233, 0.4),
                stop:1 rgba(2, 132, 199, 0.35));
            color: #ffffff;
            border: 2px solid rgba(14, 165, 233, 0.6);
            border-radius: 15px;
            padding: 12px 18px;
            font-size: 16px;
            font-weight: bold;
            min-width: 180px;
        }
        QComboBox:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(14, 165, 233, 0.5),
                stop:1 rgba(2, 132, 199, 0.45));
            border: 2px solid #0ea5e9;
        }
        QComboBox:focus {
            border: 2px solid #38bdf8;
        }
        QComboBox::drop-down {
            border: none;
            width: 35px;
            background: transparent;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 6px solid transparent;
            border-right: 6px solid transparent;
            border-top: 8px solid #ffffff;
            width: 0;
            height: 0;
        }
        QComboBox QAbstractItemView {
            background: rgba(30, 58, 138, 0.95);
            border: 2px solid rgba(14, 165, 233, 0.6);
            border-radius: 10px;
            selection-background-color: rgba(14, 165, 233, 0.5);
            color: #ffffff;
            padding: 5px;
        }
    )");
    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(themeCombo);
    themeLayout->addStretch();
    genLayout->addLayout(themeLayout);

    scrollLayout->addWidget(generalGroup);

    // Action Buttons Container
    QWidget *buttonContainer = new QWidget(scrollContent);
    buttonContainer->setFixedHeight(100);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(25);
    
    saveButton = new QPushButton("💾 Save Settings", buttonContainer);
    saveButton->setFixedSize(200, 60);
    saveButton->setCursor(Qt::PointingHandCursor);
    saveButton->setObjectName("ActionButton");
    
    resetButton = new QPushButton("🔄 Reset to Defaults", buttonContainer);
    resetButton->setFixedSize(200, 60);
    resetButton->setCursor(Qt::PointingHandCursor);
    resetButton->setObjectName("ActionButton");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();
    
    scrollLayout->addWidget(buttonContainer);
    scrollLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    contentLayout->addWidget(scrollArea);
    
    // Connect save/reset buttons (created in setupContentArea)
    connect(saveButton, &QPushButton::clicked, this, &SettingsWindow::saveSettings);
    connect(resetButton, &QPushButton::clicked, this, &SettingsWindow::resetToDefaults);
    connect(browsePathButton, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Default Save Location", defaultPathEdit->text());
        if (!dir.isEmpty()) {
            defaultPathEdit->setText(dir);
        }
    });
}

void SettingsWindow::applyStyles()
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

    // Sidebar styling (matching About page)
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
    
    // Highlight the Settings button (active screen)
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
    
    settingsButton->setStyleSheet(activeButtonStyle);

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
            text-shadow: 0 4px 20px %2;
            letter-spacing: 3px;
        }
    )").arg(colors.primaryLight, colors.primary);
    
    titleLabel->setStyleSheet(titleStyle);

    QString scrollStyle = QString(R"(
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            background: %1;
            width: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background: %2;
            border-radius: 6px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: %3;
        }
    )").arg(colors.sidebarBg, colors.navButtonActive, colors.primary);
    
    scrollArea->setStyleSheet(scrollStyle);

    QString groupBoxStyle = QString(R"(
        QGroupBox#SettingsGroup {
            background: %1;
            border: 3px solid %2;
            border-radius: 30px;
            margin-top: 25px;
            padding-top: 30px;
        }
        QGroupBox#SettingsGroup::title {
            color: %3;
            font-size: 24px;
            font-weight: bold;
            subcontrol-origin: margin;
            left: 25px;
            padding: 0 20px;
            letter-spacing: 1.5px;
            text-shadow: 0 2px 10px %4;
        }
    )").arg(colors.cardBg, colors.cardBorder, colors.textSecondary, colors.primary);
    
    compressionGroup->setStyleSheet(groupBoxStyle);
    generalGroup->setStyleSheet(groupBoxStyle);

    QString saveButtonStyle = QString(R"(
        QPushButton#ActionButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 %1, stop:1 %2);
            color: white;
            border-radius: 18px;
            border: 3px solid %3;
            padding: 15px;
            font-weight: bold;
            font-size: 17px;
            letter-spacing: 0.5px;
        }
        QPushButton#ActionButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 %4, stop:1 %4);
            border: 3px solid %5;
            transform: scale(1.05);
        }
        QPushButton#ActionButton:pressed {
            background: %2;
            transform: scale(0.98);
        }
    )").arg(colors.buttonBg, colors.primaryDark, colors.buttonBorder, 
            colors.buttonHover, colors.primaryHover);
    
    saveButton->setStyleSheet(saveButtonStyle);

    QString resetButtonStyle = QString(R"(
        QPushButton#ActionButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 %1, stop:1 %2);
            color: white;
            border-radius: 18px;
            border: 3px solid %3;
            padding: 15px;
            font-weight: bold;
            font-size: 17px;
            letter-spacing: 0.5px;
        }
        QPushButton#ActionButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 %4, stop:1 %5);
            border: 3px solid %6;
            transform: scale(1.05);
        }
        QPushButton#ActionButton:pressed {
            background: %2;
            transform: scale(0.98);
        }
    )").arg(colors.navButtonActive, colors.primaryDark, colors.cardBorder,
            colors.navButtonHover, colors.primaryDark, colors.primary);
    
    resetButton->setStyleSheet(resetButtonStyle);
}

void SettingsWindow::setupAnimations()
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

void SettingsWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateResponsiveSizes();
}

void SettingsWindow::updateResponsiveSizes() {
    int windowWidth = width();
    double scale = qBound(0.7, (double)windowWidth / 1920.0, 1.5);
    
    if (titleLabel) {
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize((int)(52 * scale));
        titleLabel->setFont(titleFont);
    }
}

void SettingsWindow::loadSettings()
{
    QSettings settings;
    
    // Load compression settings
    imageQualitySpin->setValue(settings.value("settings/imageQuality", 85).toInt());
    videoQualitySpin->setValue(settings.value("settings/videoQuality", 70).toInt());
    
    // Load general settings
    QString defaultPath = settings.value("settings/defaultPath", QDir::homePath()).toString();
    defaultPathEdit->setText(defaultPath);
    
    int themeIndex = settings.value("settings/theme", 0).toInt();
    themeCombo->setCurrentIndex(themeIndex);
    
    // Load and apply current theme
    Theme::setCurrentTheme(static_cast<Theme::ThemeType>(themeIndex));
}

void SettingsWindow::saveSettings()
{
    QSettings settings;
    
    // Save compression settings
    settings.setValue("settings/imageQuality", imageQualitySpin->value());
    settings.setValue("settings/videoQuality", videoQualitySpin->value());
    
    // Save general settings
    settings.setValue("settings/defaultPath", defaultPathEdit->text());
    settings.setValue("settings/theme", themeCombo->currentIndex());
    
    settings.sync();
    
    // Update theme in Theme class
    Theme::setCurrentTheme(static_cast<Theme::ThemeType>(themeCombo->currentIndex()));
    
    // Emit theme changed signal
    emit themeChanged();
    
    // Show success message
    QWidget *parentWindow = this->window();
    QMessageBox *msgBox = createStyledMessageBox(parentWindow, "✅ Settings Saved",
                         "✅ Your settings have been saved successfully!\n\nTheme changes will be applied immediately.", QMessageBox::Information);
    msgBox->exec();
    delete msgBox;
}

void SettingsWindow::resetToDefaults()
{
    // Reset to default values
    imageQualitySpin->setValue(85);
    videoQualitySpin->setValue(70);
    defaultPathEdit->setText(QDir::homePath());
    themeCombo->setCurrentIndex(0);
    
    // Update theme
    Theme::setCurrentTheme(Theme::BlueTeal);
    emit themeChanged();
    
    // Show confirmation
    QWidget *parentWindow = this->window();
    QMessageBox *msgBox = createStyledMessageBox(parentWindow, "🔄 Reset Complete",
                         "🔄 Settings have been reset to default values.", QMessageBox::Information);
    msgBox->exec();
    delete msgBox;
}

void SettingsWindow::applyTheme()
{
    // Re-apply styles with current theme
    applyStyles();
}

