#ifndef MAINSCREEN_H
#define MAINSCREEN_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>

class MainScreen : public QWidget {
    Q_OBJECT

public:
    explicit MainScreen(QWidget *parent = nullptr);
    void applyTheme();

signals:
    void transitionToCompress();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *titleLabel;
    QLabel *descriptionLabel;
    QLabel *instructionLabel;
    QPixmap *backgroundPixmap;
    void setupAnimations();
    void loadBackgroundImage();
    void updateFontSizes();
};

#endif

