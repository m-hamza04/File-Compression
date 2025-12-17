#include "mainwindow.h"
#include "mainscreen.h"
#include "selectionscreen.h"
#include "compress.h"
#include "decompression.h"
#include "dashboard.h"
#include "visualizer.h"
#include "history.h"
#include "abouthelp.h"
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QGraphicsBlurEffect>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QGraphicsView>
#include <QTimer>
#include <QRect>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("File Optimizer");
    // Set a reasonable minimum size for smaller screens
    setMinimumSize(1024, 600);
    // Set initial size to screen size or default
    resize(1920, 1080);
    // Show maximized on startup
    showMaximized();
    
    // Create stacked widget as central widget
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(stackedWidget);
    
    // Create all screens
    mainScreen = new MainScreen(this);
    selectionScreen = new SelectionScreen(this);
    compressWindow = new CompressWindow(this);
    decompressWindow = new DecompressWindow(this);
    dashboardWindow = new DashboardWindow(this);
    visualizerWindow = new VisualizerWindow(this);
    historyWindow = new HistoryWindow(this);
    aboutHelpWindow = new AboutHelpWindow(this);
    
    // Ensure widgets are visible and have proper background
    // Note: setAutoFillBackground can conflict with custom paintEvent, so we skip it for screens with custom backgrounds
    compressWindow->setAutoFillBackground(true);
    decompressWindow->setAutoFillBackground(true);
    dashboardWindow->setAutoFillBackground(true);
    visualizerWindow->setAutoFillBackground(true);
    historyWindow->setAutoFillBackground(true);
    aboutHelpWindow->setAutoFillBackground(true);
    
    // Add screens to stacked widget
    stackedWidget->addWidget(mainScreen);         // Index 0
    stackedWidget->addWidget(selectionScreen);    // Index 1
    stackedWidget->addWidget(compressWindow);     // Index 2
    stackedWidget->addWidget(decompressWindow);   // Index 3
    stackedWidget->addWidget(dashboardWindow);    // Index 4
    stackedWidget->addWidget(visualizerWindow);   // Index 5
    stackedWidget->addWidget(historyWindow);       // Index 6
    stackedWidget->addWidget(aboutHelpWindow);    // Index 7
    
    // Start with main screen
    stackedWidget->setCurrentIndex(PAGE_MAIN);
    
    // Ensure current widget is shown and updated
    if (mainScreen) {
        mainScreen->show();
        mainScreen->update();
    }
    
    // Setup all connections
    setupConnections();
}

MainWindow::~MainWindow() {}

void MainWindow::setupConnections()
{
    // Connect MainScreen to SelectionScreen
    connect(mainScreen, &MainScreen::transitionToCompress,
            this, &MainWindow::navigateToSelectionScreen);
    
    // Connect SelectionScreen to Compress/Decompress/Back
    connect(selectionScreen, &SelectionScreen::transitionToCompress,
            this, &MainWindow::navigateToCompress);
    connect(selectionScreen, &SelectionScreen::transitionToDecompress,
            this, &MainWindow::navigateToDecompress);
    connect(selectionScreen, &SelectionScreen::transitionToMain,
            this, &MainWindow::navigateToMainScreen);
    
    // Connect CompressWindow navigation
    connect(compressWindow, &CompressWindow::navigateToSelection,
            this, &MainWindow::navigateToSelectionScreen);
    connect(compressWindow, &CompressWindow::navigateToDashboard,
            this, &MainWindow::navigateToDashboard);
    connect(compressWindow, &CompressWindow::navigateToCompress,
            this, &MainWindow::navigateToCompress);
    connect(compressWindow, &CompressWindow::navigateToDecompress,
            this, &MainWindow::navigateToDecompress);
    connect(compressWindow, &CompressWindow::navigateToVisualizer,
            this, &MainWindow::navigateToVisualizer);
    connect(compressWindow, &CompressWindow::navigateToHistory,
            this, &MainWindow::navigateToHistory);
    connect(compressWindow, &CompressWindow::navigateToAboutHelp,
            this, &MainWindow::navigateToAboutHelp);
    
    // Connect compression completion to dashboard stats update
    connect(compressWindow, &CompressWindow::compressionCompleted,
            dashboardWindow, &DashboardWindow::updateStats);
    
    // Connect compression completion with type to visualizer
    connect(compressWindow, &CompressWindow::compressionCompletedWithType,
            visualizerWindow, &VisualizerWindow::addCompressionData);
    
    // Connect compression completion to history
    connect(compressWindow, &CompressWindow::compressionCompletedWithType,
            historyWindow, [this](const QString &fileType, qint64 originalSize, qint64 compressedSize) {
                if (compressWindow && historyWindow) {
                    QString filePath = compressWindow->property("lastCompressedFile").toString();
                    historyWindow->addCompressionRecord(fileType, originalSize, compressedSize, filePath);
                }
            });
    
    // Connect decompression completion to history
    connect(decompressWindow, &DecompressWindow::decompressionCompleted,
            historyWindow, [this](const QString &fileType, qint64 compressedSize, qint64 decompressedSize) {
                if (decompressWindow && historyWindow) {
                    QString filePath = decompressWindow->property("lastDecompressedFile").toString();
                    historyWindow->addDecompressionRecord(fileType, compressedSize, decompressedSize, filePath);
                }
            });
    
    // Connect DecompressWindow navigation
    connect(decompressWindow, &DecompressWindow::navigateToSelection,
            this, &MainWindow::navigateToSelectionScreen);
    connect(decompressWindow, &DecompressWindow::navigateToDashboard,
            this, &MainWindow::navigateToDashboard);
    connect(decompressWindow, &DecompressWindow::navigateToCompress,
            this, &MainWindow::navigateToCompress);
    connect(decompressWindow, &DecompressWindow::navigateToDecompress,
            this, &MainWindow::navigateToDecompress);
    connect(decompressWindow, &DecompressWindow::navigateToVisualizer,
            this, &MainWindow::navigateToVisualizer);
    connect(decompressWindow, &DecompressWindow::navigateToHistory,
            this, &MainWindow::navigateToHistory);
    connect(decompressWindow, &DecompressWindow::navigateToAboutHelp,
            this, &MainWindow::navigateToAboutHelp);
    
    // Connect DashboardWindow navigation
    connect(dashboardWindow, &DashboardWindow::navigateToSelection,
            this, &MainWindow::navigateToSelectionScreen);
    connect(dashboardWindow, &DashboardWindow::navigateToDashboard,
            this, &MainWindow::navigateToDashboard);
    connect(dashboardWindow, &DashboardWindow::navigateToCompress,
            this, &MainWindow::navigateToCompress);
    connect(dashboardWindow, &DashboardWindow::navigateToDecompress,
            this, &MainWindow::navigateToDecompress);
    connect(dashboardWindow, &DashboardWindow::navigateToVisualizer,
            this, &MainWindow::navigateToVisualizer);
    connect(dashboardWindow, &DashboardWindow::navigateToHistory,
            this, &MainWindow::navigateToHistory);
    connect(dashboardWindow, &DashboardWindow::navigateToAboutHelp,
            this, &MainWindow::navigateToAboutHelp);
    
    // Connect VisualizerWindow navigation
    connect(visualizerWindow, &VisualizerWindow::navigateToSelection,
            this, &MainWindow::navigateToSelectionScreen);
    connect(visualizerWindow, &VisualizerWindow::navigateToDashboard,
            this, &MainWindow::navigateToDashboard);
    connect(visualizerWindow, &VisualizerWindow::navigateToCompress,
            this, &MainWindow::navigateToCompress);
    connect(visualizerWindow, &VisualizerWindow::navigateToDecompress,
            this, &MainWindow::navigateToDecompress);
    connect(visualizerWindow, &VisualizerWindow::navigateToVisualizer,
            this, &MainWindow::navigateToVisualizer);
    connect(visualizerWindow, &VisualizerWindow::navigateToHistory,
            this, &MainWindow::navigateToHistory);
    connect(visualizerWindow, &VisualizerWindow::navigateToAboutHelp,
            this, &MainWindow::navigateToAboutHelp);
    
    // Connect HistoryWindow navigation
    connect(historyWindow, &HistoryWindow::navigateToSelection,
            this, &MainWindow::navigateToSelectionScreen);
    connect(historyWindow, &HistoryWindow::navigateToDashboard,
            this, &MainWindow::navigateToDashboard);
    connect(historyWindow, &HistoryWindow::navigateToCompress,
            this, &MainWindow::navigateToCompress);
    connect(historyWindow, &HistoryWindow::navigateToDecompress,
            this, &MainWindow::navigateToDecompress);
    connect(historyWindow, &HistoryWindow::navigateToVisualizer,
            this, &MainWindow::navigateToVisualizer);
    connect(historyWindow, &HistoryWindow::navigateToHistory,
            this, &MainWindow::navigateToHistory);
    connect(historyWindow, &HistoryWindow::navigateToAboutHelp,
            this, &MainWindow::navigateToAboutHelp);
    
    // Connect AboutHelpWindow navigation
    connect(aboutHelpWindow, &AboutHelpWindow::navigateToSelection,
            this, &MainWindow::navigateToSelectionScreen);
    connect(aboutHelpWindow, &AboutHelpWindow::navigateToCompress,
            this, &MainWindow::navigateToCompress);
    connect(aboutHelpWindow, &AboutHelpWindow::navigateToDecompress,
            this, &MainWindow::navigateToDecompress);
    connect(aboutHelpWindow, &AboutHelpWindow::navigateToDashboard,
            this, &MainWindow::navigateToDashboard);
    connect(aboutHelpWindow, &AboutHelpWindow::navigateToVisualizer,
            this, &MainWindow::navigateToVisualizer);
    connect(aboutHelpWindow, &AboutHelpWindow::navigateToHistory,
            this, &MainWindow::navigateToHistory);
    connect(aboutHelpWindow, &AboutHelpWindow::navigateToAboutHelp,
            this, &MainWindow::navigateToAboutHelp);
    
}

void MainWindow::changePage(int index)
{
    if (index < 0 || index >= stackedWidget->count()) return;
    
    QWidget *currentWidget = stackedWidget->currentWidget();
    QWidget *nextWidget = stackedWidget->widget(index);
    
    if (!nextWidget || currentWidget == nextWidget) {
        return;
    }
    
    // Clean up any existing effects immediately
    if (currentWidget) {
        QGraphicsEffect *oldEffect = currentWidget->graphicsEffect();
        if (oldEffect) {
            currentWidget->setGraphicsEffect(nullptr);
            oldEffect->deleteLater();
        }
    }
    
    if (nextWidget) {
        QGraphicsEffect *oldEffect = nextWidget->graphicsEffect();
        if (oldEffect) {
            nextWidget->setGraphicsEffect(nullptr);
            oldEffect->deleteLater();
        }
    }
    
    // Switch widget immediately (no animation to prevent hanging)
    stackedWidget->setCurrentIndex(index);
    
    // Ensure next widget is visible and properly rendered
    if (nextWidget) {
        nextWidget->show();
        nextWidget->raise();
        nextWidget->update();
        nextWidget->repaint();
        nextWidget->setFocus();
        nextWidget->activateWindow();
    }
}

void MainWindow::navigateToMainScreen()
{
    changePage(PAGE_MAIN);
}

void MainWindow::navigateToSelectionScreen()
{
    changePage(PAGE_SELECTION);
}

void MainWindow::navigateToCompress()
{
    changePage(PAGE_COMPRESS);
}

void MainWindow::navigateToDecompress()
{
    changePage(PAGE_DECOMPRESS);
}

void MainWindow::navigateToDashboard()
{
    changePage(PAGE_DASHBOARD);
}

void MainWindow::navigateToVisualizer()
{
    changePage(PAGE_VISUALIZER);
}

void MainWindow::navigateToHistory()
{
    changePage(PAGE_HISTORY);
}

void MainWindow::navigateToAboutHelp()
{
    changePage(PAGE_ABOUT_HELP);
}

