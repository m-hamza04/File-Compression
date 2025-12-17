#ifndef THEME_H
#define THEME_H

#include <QString>
#include <QColor>
#include <QMap>

class Theme
{
public:
    enum ThemeType {
        BlueTeal = 0,
        Purple = 1,
        Orange = 2,
        Dark = 3
    };

    struct ThemeColors {
        // Background gradients
        QString bgGradientStart;
        QString bgGradientMid;
        QString bgGradientEnd;
        QString bgGradientStop3;
        
        // Primary colors
        QString primary;
        QString primaryLight;
        QString primaryDark;
        QString primaryHover;
        
        // Secondary colors
        QString secondary;
        QString secondaryLight;
        
        // Text colors
        QString textPrimary;
        QString textSecondary;
        QString textTertiary;
        
        // Sidebar colors
        QString sidebarBg;
        QString sidebarBorder;
        QString navButtonBg;
        QString navButtonHover;
        QString navButtonActive;
        
        // Card/Group colors
        QString cardBg;
        QString cardBorder;
        
        // Button colors
        QString buttonBg;
        QString buttonHover;
        QString buttonBorder;
    };

    static ThemeColors getThemeColors(ThemeType type);
    static ThemeType getCurrentTheme();
    static void setCurrentTheme(ThemeType type);
    static QString getThemeName(ThemeType type);
};

#endif // THEME_H

