#ifndef SELECTIONSCREEN_H
#define SELECTIONSCREEN_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>

class SelectionScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SelectionScreen(QWidget *parent = nullptr);
    void applyTheme();

signals:
    void transitionToCompress();
    void transitionToDecompress();
    void transitionToMain();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();
    void setupAnimations();
    void loadBackgroundImage();
    void updateFontSizes();

    QLabel *titleLabel;
    QLabel *descriptionLabel;
    QPushButton *compressButton;
    QPushButton *decompressButton;
    QPushButton *backButton;
    QPixmap *backgroundPixmap;
};

#endif // SELECTIONSCREEN_H

