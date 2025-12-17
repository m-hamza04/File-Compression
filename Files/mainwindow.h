#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsTransform>
#include <QGraphicsEffect>

// Forward declarations
class MainScreen;
class SelectionScreen;
class CompressWindow;
class DecompressWindow;
class DashboardWindow;
class VisualizerWindow;
class HistoryWindow;
class AboutHelpWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void navigateToMainScreen();
    void navigateToSelectionScreen();
    void navigateToCompress();
    void navigateToDecompress();
    void navigateToDashboard();
    void navigateToVisualizer();
    void navigateToHistory();
    void navigateToAboutHelp();

private:
    void setupConnections();
    void changePage(int index);

    QStackedWidget *stackedWidget;
    
    MainScreen *mainScreen;
    SelectionScreen *selectionScreen;
    CompressWindow *compressWindow;
    DecompressWindow *decompressWindow;
    DashboardWindow *dashboardWindow;
    VisualizerWindow *visualizerWindow;
    HistoryWindow *historyWindow;
    AboutHelpWindow *aboutHelpWindow;
    
    enum PageIndex {
        PAGE_MAIN = 0,
        PAGE_SELECTION = 1,
        PAGE_COMPRESS = 2,
        PAGE_DECOMPRESS = 3,
        PAGE_DASHBOARD = 4,
        PAGE_VISUALIZER = 5,
        PAGE_HISTORY = 6,
        PAGE_ABOUT_HELP = 7
    };
};

#endif // MAINWINDOW_H
