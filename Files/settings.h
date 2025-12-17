#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QSettings>
#include <QResizeEvent>

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow() override;

signals:
    void navigateToDashboard();
    void navigateToCompress();
    void navigateToDecompress();
    void navigateToVisualizer();
    void navigateToSettings();
    void navigateToHistory();
    void navigateToAboutHelp();
    void navigateToSelection();
    void themeChanged();

public:
    void applyTheme();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupSidebar();
    void setupContentArea();
    void applyStyles();
    void setupAnimations();
    void updateResponsiveSizes();
    void loadSettings();
    void saveSettings();
    void resetToDefaults();

    // Core UI Components
    QWidget *mainCentralWidget;
    QHBoxLayout *mainLayout;

    // Sidebar Components
    QWidget *sidebar;
    QVBoxLayout *sidebarLayout;
    QPushButton *dashboardButton;
    QPushButton *compressButton;
    QPushButton *decompressButton;
    QPushButton *visualizerButton;
    QPushButton *settingsButton;
    QPushButton *historyButton;
    QPushButton *aboutButton;
    QPushButton *backButton;

    // Content Area Components
    QWidget *contentArea;
    QVBoxLayout *contentLayout;
    QLabel *titleLabel;
    QScrollArea *scrollArea;
    QWidget *scrollContent;

    // Settings Controls
    QGroupBox *compressionGroup;
    QSpinBox *imageQualitySpin;
    QSpinBox *videoQualitySpin;
    
    QGroupBox *generalGroup;
    QLineEdit *defaultPathEdit;
    QPushButton *browsePathButton;
    QComboBox *themeCombo;
    
    QPushButton *saveButton;
    QPushButton *resetButton;
};

#endif // SETTINGS_H

