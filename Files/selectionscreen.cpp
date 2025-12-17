#include "selectionscreen.h"
#include "theme.h"
#include <QFont>
#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QDir>
#include <QFileInfo>
#include <QGradient>

SelectionScreen::SelectionScreen(QWidget *parent) : QWidget(parent), backgroundPixmap(nullptr)
{
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    
    // Load background image (if available)
    loadBackgroundImage();
    
    // Fallback gradient if image not found
    if (!backgroundPixmap || backgroundPixmap->isNull()) {
        setStyleSheet(R"(
            QWidget {
                background: qlineargradient(
                    x1: 0, y1: 0, x2: 1, y2: 1,
                    stop: 0 #0f172a,
                    stop: 0.3 #1e293b,
                    stop: 0.6 #0c4a6e,
                    stop: 1 #1e293b
                );
            }
        )");
    }
    
    setupUI();
    updateFontSizes();
    setupAnimations();
}

void SelectionScreen::applyTheme()
{
    // Theme changes will be applied on next paint
    update();
}

void SelectionScreen::updateFontSizes() {
    int baseWidth = 1920;
    int currentWidth = width();
    int currentHeight = height();
    
    double scaleW = (double)currentWidth / baseWidth;
    double scaleH = (double)currentHeight / 1080.0;
    double scale = qMin(scaleW, scaleH);
    scale = qBound(0.5, scale, 2.0);
    
    if (titleLabel) {
        int titleSize = (int)(64 * scale);
        QFont titleFont("Segoe UI", titleSize, QFont::Bold);
        titleLabel->setFont(titleFont);
    }
    
    if (descriptionLabel) {
        int descSize = (int)(26 * scale);
        QFont descFont("Segoe UI", descSize);
        descriptionLabel->setFont(descFont);
    }
    
    if (compressButton) {
        int btnSize = (int)(22 * scale);
        QFont btnFont("Segoe UI", btnSize, QFont::Bold);
        compressButton->setFont(btnFont);
    }
    
    if (decompressButton) {
        int btnSize = (int)(22 * scale);
        QFont btnFont("Segoe UI", btnSize, QFont::Bold);
        decompressButton->setFont(btnFont);
    }
    
    if (backButton) {
        int backSize = (int)(12 * scale);
        QFont backFont("Segoe UI", backSize, QFont::Bold);
        backButton->setFont(backFont);
    }
    
    // Update layout margins and spacing
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(this->layout());
    if (layout) {
        int margin = (int)(60 * scale);
        int spacing = (int)(50 * scale);
        layout->setContentsMargins(margin, margin, margin, margin);
        layout->setSpacing(spacing);
    }
}

void SelectionScreen::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateFontSizes();
}

void SelectionScreen::setupUI()
{
    // Back button at top-left
    backButton = new QPushButton("← Back", this);
    backButton->setFixedSize(120, 45);
    backButton->setCursor(Qt::PointingHandCursor);
    QFont backFont("Segoe UI", 12, QFont::Bold);
    backButton->setFont(backFont);
    backButton->setStyleSheet(R"(
        QPushButton {
            background: rgba(14, 165, 233, 0.2);
            color: #7dd3fc;
            border-radius: 12px;
            border: 2px solid rgba(14, 165, 233, 0.5);
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: rgba(14, 165, 233, 0.4);
            color: white;
            border: 2px solid #0ea5e9;
        }
        QPushButton:pressed {
            background: rgba(14, 165, 233, 0.6);
        }
    )");
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(50);
    layout->setContentsMargins(60, 60, 60, 60);
    
    // Top layout for back button
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(backButton);
    topLayout->addStretch();
    layout->addLayout(topLayout);
    layout->addStretch(1);

    titleLabel = new QLabel("File Optimizer", this);
    QFont titleFont("Segoe UI", 64, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #0ea5e9;
            background: transparent;
            font-weight: bold;
            letter-spacing: 4px;
            text-shadow: 0 0 20px rgba(14, 165, 233, 0.5);
        }
    )");
    titleLabel->setAlignment(Qt::AlignCenter);

    descriptionLabel = new QLabel("Choose an operation", this);
    QFont descriptionFont("Segoe UI", 26);
    descriptionLabel->setFont(descriptionFont);
    descriptionLabel->setStyleSheet(R"(
        QLabel {
            color: #7dd3fc;
            background: transparent;
            font-weight: normal;
            letter-spacing: 1px;
        }
    )");
    descriptionLabel->setAlignment(Qt::AlignCenter);

    compressButton = new QPushButton("📦 Compression", this);
    compressButton->setFixedSize(400, 110);
    compressButton->setCursor(Qt::PointingHandCursor);
    QFont buttonFont("Segoe UI", 22, QFont::Bold);
    compressButton->setFont(buttonFont);
    compressButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #38bdf8, stop:1 #0284c7);
            color: white;
            border-radius: 25px;
            border: 3px solid #0ea5e9;
            padding: 16px;
            font-weight: bold;
            box-shadow: 0 8px 32px rgba(14, 165, 233, 0.4);
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #0ea5e9, stop:1 #0ea5e9);
            border: 3px solid #7dd3fc;
            box-shadow: 0 12px 40px rgba(14, 165, 233, 0.6);
        }
        QPushButton:pressed {
            background: #0284c7;
            border: 3px solid #38bdf8;
        }
    )");

    decompressButton = new QPushButton("📤 Decompression", this);
    decompressButton->setFixedSize(400, 110);
    decompressButton->setCursor(Qt::PointingHandCursor);
    decompressButton->setFont(buttonFont);
    decompressButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #0ea5e9, stop:1 #0284c7);
            color: white;
            border-radius: 25px;
            border: 3px solid #38bdf8;
            padding: 16px;
            font-weight: bold;
            box-shadow: 0 8px 32px rgba(14, 165, 233, 0.4);
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #7dd3fc, stop:1 #0ea5e9);
            border: 3px solid #bae6fd;
            box-shadow: 0 12px 40px rgba(56, 189, 248, 0.6);
        }
        QPushButton:pressed {
            background: #0284c7;
            border: 3px solid #0ea5e9;
        }
    )");

    layout->addStretch(2);
    layout->addWidget(titleLabel);
    layout->addSpacing(15);
    layout->addWidget(descriptionLabel);
    layout->addSpacing(80);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(compressButton);
    buttonLayout->addSpacing(60);
    buttonLayout->addWidget(decompressButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    layout->addStretch(3);

    setLayout(layout);

    connect(compressButton, &QPushButton::clicked, this, &SelectionScreen::transitionToCompress);
    connect(decompressButton, &QPushButton::clicked, this, &SelectionScreen::transitionToDecompress);
    connect(backButton, &QPushButton::clicked, this, &SelectionScreen::transitionToMain);
}

void SelectionScreen::setupAnimations()
{
    // Buttons are immediately visible - no opacity animation to prevent hanging
    // This ensures buttons are always clickable
}

void SelectionScreen::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    if (backgroundPixmap && !backgroundPixmap->isNull()) {
        // Scale image to fill entire screen (1920x1080)
        QPixmap scaledPixmap = backgroundPixmap->scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        
        // Draw image to fill entire widget
        painter.drawPixmap(0, 0, width(), height(), scaledPixmap);
        
        // Add blue-tinted dark overlay for better text readability
        painter.fillRect(rect(), QColor(15, 23, 42, 150));
    } else {
        // Draw gradient background using current theme
        Theme::ThemeColors colors = Theme::getThemeColors(Theme::getCurrentTheme());
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, QColor(colors.bgGradientStart));
        gradient.setColorAt(0.3, QColor(colors.bgGradientMid));
        gradient.setColorAt(0.6, QColor(colors.bgGradientEnd));
        gradient.setColorAt(1, QColor(colors.bgGradientStop3));
        painter.fillRect(rect(), gradient);
    }
    
    QWidget::paintEvent(event);
}

void SelectionScreen::loadBackgroundImage() {
    // Direct path to Selection_bg.jpg in images folder
    QString imagePath = "images/Selection_bg.jpg";
    
    QFileInfo fileInfo(imagePath);
    if (fileInfo.exists()) {
        QPixmap pixmap(imagePath);
        if (!pixmap.isNull()) {
            backgroundPixmap = new QPixmap(pixmap);
            qDebug() << "✓ Background image loaded from: images/Selection_bg.jpg";
            qDebug() << "  Image size:" << pixmap.width() << "x" << pixmap.height();
            return;
        } else {
            qDebug() << "✗ Failed to load image from: images/Selection_bg.jpg";
        }
    } else {
        qDebug() << "✗ Image not found at: images/Selection_bg.jpg";
        qDebug() << "  Full path checked:" << QDir::currentPath() + "/images/Selection_bg.jpg";
    }
    
    // If no image found, create a null pixmap
    backgroundPixmap = nullptr;
    qDebug() << "Using gradient background (no image loaded).";
}

