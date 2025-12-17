#include "mainscreen.h"
#include "theme.h"
#include <QFont>
#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QPixmap>
#include <QPainter>
#include <QDir>
#include <QFileInfo>
#include <QGradient>

MainScreen::MainScreen(QWidget *parent) : QWidget(parent), backgroundPixmap(nullptr) {
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

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    titleLabel = new QLabel("File Optimizer", this);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
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

    descriptionLabel = new QLabel("Ready to optimize your files like a pro! 🚀", this);
    descriptionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    descriptionLabel->setStyleSheet(R"(
        QLabel {
            color: #7dd3fc;
            background: transparent;
            font-weight: normal;
            letter-spacing: 1px;
        }
    )");
    descriptionLabel->setAlignment(Qt::AlignCenter);

    instructionLabel = new QLabel("Press Enter to start", this);
    instructionLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    instructionLabel->setStyleSheet(R"(
        QLabel {
            color: #ffffff;
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(14, 165, 233, 0.4),
                stop:1 rgba(2, 132, 199, 0.4));
            border: 3px solid #0ea5e9;
            border-radius: 25px;
            padding: 20px 60px;
            font-weight: bold;
            box-shadow: 0 8px 32px rgba(14, 165, 233, 0.3);
        }
    )");
    instructionLabel->setAlignment(Qt::AlignCenter);

    layout->addStretch(2);
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);
    layout->addSpacing(0);
    layout->addWidget(descriptionLabel, 0, Qt::AlignCenter);
    layout->addStretch(3);
    layout->addWidget(instructionLabel, 0, Qt::AlignCenter);
    layout->addStretch(1);

    setLayout(layout);

    setFocusPolicy(Qt::StrongFocus);
    
    // Set initial font sizes based on initial size
    updateFontSizes();

    setupAnimations();
}

void MainScreen::applyTheme()
{
    // Theme changes will be applied on next paint
    update();
}

void MainScreen::updateFontSizes() {
    int baseWidth = 1920;
    int baseHeight = 1080;
    int currentWidth = width();
    int currentHeight = height();
    
    // Calculate scale factor (use minimum to maintain aspect)
    double scaleW = (double)currentWidth / baseWidth;
    double scaleH = (double)currentHeight / baseHeight;
    double scale = qMin(scaleW, scaleH);
    
    // Clamp scale to reasonable bounds
    scale = qBound(0.5, scale, 2.0);
    
    // Update title font
    int titleSize = (int)(72 * scale);
    QFont titleFont("Segoe UI", titleSize, QFont::Bold);
    titleLabel->setFont(titleFont);
    
    // Update description font
    int descSize = (int)(28 * scale);
    QFont descFont("Segoe UI", descSize);
    descriptionLabel->setFont(descFont);
    
    // Update instruction font
    int instrSize = (int)(20 * scale);
    QFont instrFont("Segoe UI", instrSize, QFont::Bold);
    instructionLabel->setFont(instrFont);
    
    // Update layout spacing and margins
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(this->layout());
    if (layout) {
        int spacing = (int)(50 * scale);
        int margin = (int)(60 * scale);
        layout->setSpacing(spacing);
        layout->setContentsMargins(margin, margin, margin, margin);
        
        // Update spacing between title and description
        if (layout->count() > 3) {
            layout->insertSpacing(2, (int)(10 * scale));
        }
    }
}

void MainScreen::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateFontSizes();
}

void MainScreen::setupAnimations() {
    // All elements are immediately visible - no fade-in animations to prevent hanging
    // Instruction label pulsing animation only
    QGraphicsOpacityEffect *instrEffect = new QGraphicsOpacityEffect(this);
    instrEffect->setOpacity(1.0);
    instructionLabel->setGraphicsEffect(instrEffect);
    
    // Pulsing animation
    QTimer::singleShot(500, [instrEffect, this]() {
        QPropertyAnimation *pulseAnim = new QPropertyAnimation(instrEffect, "opacity", this);
        pulseAnim->setDuration(2000);
        pulseAnim->setStartValue(0.8);
        pulseAnim->setKeyValueAt(0.5, 1.0);
        pulseAnim->setEndValue(0.8);
        pulseAnim->setLoopCount(-1);
        pulseAnim->setEasingCurve(QEasingCurve::InOutSine);
        pulseAnim->start();
    });
}

void MainScreen::paintEvent(QPaintEvent *event) {
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

void MainScreen::loadBackgroundImage() {
    // Direct path to Main_bg.jpg in images folder
    QString imagePath = "images/Main_bg.jpg";
    
    QFileInfo fileInfo(imagePath);
    if (fileInfo.exists()) {
        QPixmap pixmap(imagePath);
        if (!pixmap.isNull()) {
            backgroundPixmap = new QPixmap(pixmap);
            qDebug() << "✓ Background image loaded from: images/Main_bg.jpg";
            qDebug() << "  Image size:" << pixmap.width() << "x" << pixmap.height();
            return;
        } else {
            qDebug() << "✗ Failed to load image from: images/Main_bg.jpg";
        }
    } else {
        qDebug() << "✗ Image not found at: images/Main_bg.jpg";
        qDebug() << "  Full path checked:" << QDir::currentPath() + "/images/Main_bg.jpg";
    }
    
    // If no image found, create a null pixmap
    backgroundPixmap = nullptr;
    qDebug() << "Using gradient background (no image loaded).";
}

void MainScreen::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {

        emit transitionToCompress();
        qDebug() << "Enter pressed - Emitting transitionToCompress signal";
    }
    QWidget::keyPressEvent(event);
}
