#include "theme.h"
#include <QSettings>

Theme::ThemeColors Theme::getThemeColors(ThemeType type)
{
    ThemeColors colors;
    
    switch (type) {
        case BlueTeal:
            colors.bgGradientStart = "#0f172a";
            colors.bgGradientMid = "#1e293b";
            colors.bgGradientEnd = "#0c4a6e";
            colors.bgGradientStop3 = "#1e293b";
            
            colors.primary = "#0ea5e9";
            colors.primaryLight = "#38bdf8";
            colors.primaryDark = "#0284c7";
            colors.primaryHover = "#7dd3fc";
            
            colors.secondary = "#14b8a6";
            colors.secondaryLight = "#5eead4";
            
            colors.textPrimary = "#0ea5e9";
            colors.textSecondary = "#7dd3fc";
            colors.textTertiary = "#bae6fd";
            
            colors.sidebarBg = "rgba(30, 58, 138, 240)";
            colors.sidebarBorder = "rgba(14, 165, 233, 0.3)";
            colors.navButtonBg = "rgba(14, 165, 233, 0.2)";
            colors.navButtonHover = "rgba(14, 165, 233, 0.4)";
            colors.navButtonActive = "rgba(14, 165, 233, 0.5)";
            
            colors.cardBg = "rgba(14, 165, 233, 0.35)";
            colors.cardBorder = "rgba(14, 165, 233, 0.7)";
            
            colors.buttonBg = "#38bdf8";
            colors.buttonHover = "#0ea5e9";
            colors.buttonBorder = "#0ea5e9";
            break;
            
        case Purple:
            colors.bgGradientStart = "#1e1b4b";
            colors.bgGradientMid = "#312e81";
            colors.bgGradientEnd = "#4c1d95";
            colors.bgGradientStop3 = "#312e81";
            
            colors.primary = "#a855f7";
            colors.primaryLight = "#c084fc";
            colors.primaryDark = "#9333ea";
            colors.primaryHover = "#d8b4fe";
            
            colors.secondary = "#8b5cf6";
            colors.secondaryLight = "#a78bfa";
            
            colors.textPrimary = "#a855f7";
            colors.textSecondary = "#c084fc";
            colors.textTertiary = "#e9d5ff";
            
            colors.sidebarBg = "rgba(88, 28, 135, 240)";
            colors.sidebarBorder = "rgba(168, 85, 247, 0.3)";
            colors.navButtonBg = "rgba(168, 85, 247, 0.2)";
            colors.navButtonHover = "rgba(168, 85, 247, 0.4)";
            colors.navButtonActive = "rgba(168, 85, 247, 0.5)";
            
            colors.cardBg = "rgba(168, 85, 247, 0.35)";
            colors.cardBorder = "rgba(168, 85, 247, 0.7)";
            
            colors.buttonBg = "#c084fc";
            colors.buttonHover = "#a855f7";
            colors.buttonBorder = "#a855f7";
            break;
            
        case Orange:
            colors.bgGradientStart = "#7c2d12";
            colors.bgGradientMid = "#9a3412";
            colors.bgGradientEnd = "#c2410c";
            colors.bgGradientStop3 = "#9a3412";
            
            colors.primary = "#f97316";
            colors.primaryLight = "#fb923c";
            colors.primaryDark = "#ea580c";
            colors.primaryHover = "#fdba74";
            
            colors.secondary = "#f97316";
            colors.secondaryLight = "#fb923c";
            
            colors.textPrimary = "#f97316";
            colors.textSecondary = "#fb923c";
            colors.textTertiary = "#fed7aa";
            
            colors.sidebarBg = "rgba(154, 52, 18, 240)";
            colors.sidebarBorder = "rgba(249, 115, 22, 0.3)";
            colors.navButtonBg = "rgba(249, 115, 22, 0.2)";
            colors.navButtonHover = "rgba(249, 115, 22, 0.4)";
            colors.navButtonActive = "rgba(249, 115, 22, 0.5)";
            
            colors.cardBg = "rgba(249, 115, 22, 0.35)";
            colors.cardBorder = "rgba(249, 115, 22, 0.7)";
            
            colors.buttonBg = "#fb923c";
            colors.buttonHover = "#f97316";
            colors.buttonBorder = "#f97316";
            break;
            
        case Dark:
            colors.bgGradientStart = "#0a0a0a";
            colors.bgGradientMid = "#1a1a1a";
            colors.bgGradientEnd = "#0f0f0f";
            colors.bgGradientStop3 = "#1a1a1a";
            
            colors.primary = "#ffffff";
            colors.primaryLight = "#e5e5e5";
            colors.primaryDark = "#d4d4d4";
            colors.primaryHover = "#f5f5f5";
            
            colors.secondary = "#a3a3a3";
            colors.secondaryLight = "#d4d4d4";
            
            colors.textPrimary = "#ffffff";
            colors.textSecondary = "#e5e5e5";
            colors.textTertiary = "#d4d4d4";
            
            colors.sidebarBg = "rgba(30, 30, 30, 240)";
            colors.sidebarBorder = "rgba(255, 255, 255, 0.2)";
            colors.navButtonBg = "rgba(255, 255, 255, 0.1)";
            colors.navButtonHover = "rgba(255, 255, 255, 0.2)";
            colors.navButtonActive = "rgba(255, 255, 255, 0.3)";
            
            colors.cardBg = "rgba(255, 255, 255, 0.1)";
            colors.cardBorder = "rgba(255, 255, 255, 0.3)";
            
            colors.buttonBg = "#404040";
            colors.buttonHover = "#525252";
            colors.buttonBorder = "#ffffff";
            break;
    }
    
    return colors;
}

Theme::ThemeType Theme::getCurrentTheme()
{
    // Always return BlueTeal theme
    return BlueTeal;
}

void Theme::setCurrentTheme(ThemeType type)
{
    // Theme is fixed to BlueTeal, do nothing
    Q_UNUSED(type);
}

QString Theme::getThemeName(ThemeType type)
{
    switch (type) {
        case BlueTeal: return "Blue-Teal";
        case Purple: return "Purple";
        case Orange: return "Orange";
        case Dark: return "Dark";
        default: return "Blue-Teal";
    }
}

