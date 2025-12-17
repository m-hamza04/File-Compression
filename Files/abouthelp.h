#ifndef ABOUTHELP_H
#define ABOUTHELP_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QResizeEvent>

class AboutHelpWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AboutHelpWindow(QWidget *parent = nullptr);

signals:
    void navigateToSelection();
    void navigateToDashboard();
    void navigateToCompress();
    void navigateToDecompress();
    void navigateToVisualizer();
    void navigateToHistory();
    void navigateToAboutHelp();

public:
    void applyTheme();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();
    void setupSidebar();
    void setupContentArea();
    void applyStyles();
    void setupAnimations();
    void updateResponsiveSizes();

    QWidget *mainCentralWidget;
    QHBoxLayout *mainLayout;
    QWidget *sidebar;
    QWidget *contentArea;
    QScrollArea *scrollArea;
    QLabel *titleLabel;
    QPushButton *backButton;
    QPushButton *aboutHelpButton;
};

#endif // ABOUTHELP_H

