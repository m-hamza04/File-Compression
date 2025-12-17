#ifndef STYLEDMESSAGEBOX_H
#define STYLEDMESSAGEBOX_H

#include <QMessageBox>
#include <QWidget>
#include <QString>
#include <Qt>
#include <QScreen>
#include <QGuiApplication>

// Helper function to format file size (B, KB, MB, GB)
inline QString formatFileSize(qint64 size) {
    if (size == 0) {
        return "0 B";
    } else if (size < 1024) {
        return QString::number(size) + " B";
    } else if (size < 1024 * 1024) {
        return QString::number(size / 1024.0, 'f', 2) + " KB";
    } else if (size < 1024 * 1024 * 1024) {
        return QString::number(size / (1024.0 * 1024.0), 'f', 2) + " MB";
    } else {
        return QString::number(size / (1024.0 * 1024.0 * 1024.0), 'f', 2) + " GB";
    }
}

// Helper function to create styled message box with modern blue-teal theme
inline QMessageBox* createStyledMessageBox(QWidget *parent, const QString &title, const QString &text, QMessageBox::Icon icon = QMessageBox::Information)
{
    QMessageBox *msgBox = new QMessageBox(parent);
    msgBox->setWindowTitle(title);

    // Enable HTML/Rich text rendering
    msgBox->setTextFormat(Qt::RichText);
    msgBox->setText(text);
    msgBox->setIcon(icon);

    // Apply modern blue-teal theme styling with enhanced design
    // Major changes: Deeper background, softer internal box, improved button hover/press effects.
    msgBox->setStyleSheet(R"(
        QMessageBox {
            /* Main Dialog Background: Deep, dark gradient */
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #0f172a,
                stop:0.5 #1e293b,
                stop:1 #0c4a6e);
            color: #f0f0f0;
            font-family: "Segoe UI", "Tahoma", sans-serif;
            font-size: 16px;
            /* Outer Glow/Border effect */
            border: 3px solid rgba(14, 165, 233, 0.4);
            border-radius: 30px; /* Increased border radius for smoother look */
        }

        QMessageBox QLabel {
            color: #ffffff;
            background: transparent;
            font-size: 16px;
            font-weight: 500;
            padding: 0px;
            line-height: 1.8;
            letter-spacing: 0.2px;
        }

        /* Message Label Area (The internal colored box) */
        QMessageBox QLabel#qt_msgbox_label {
            color: #bae6fd; /* Lighter text for contrast */
            /* Internal box background: Subtle blue-teal tint */
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(14, 165, 233, 0.1),
                stop:1 rgba(20, 184, 166, 0.05));
            border-radius: 20px;
            padding: 30px 35px; /* Increased padding */
            border: 2px solid rgba(14, 165, 233, 0.5); /* Stronger inner border */
            min-height: 100px;
            font-size: 17px;
        }

        /* Detailed Text Label (if used) */
        QMessageBox QLabel#qt_msgboxex_label {
            color: #7dd3fc; /* Bright blue for detailed text */
            background: transparent;
            font-size: 14px;
            padding: 10px 0px;
        }

        /* Buttons */
        QMessageBox QPushButton {
            /* Button Gradient: Brighter, smoother blue-teal */
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #0ea5e9, stop:1 #0284c7);
            color: white;
            font-size: 18px; /* Slightly larger font */
            font-weight: bold;
            border-radius: 22px; /* Slightly smoother corners */
            border: 3px solid #38bdf8;
            padding: 15px 40px;
            min-width: 160px; /* Wider buttons */
            min-height: 50px;
            letter-spacing: 0.8px;
            margin-top: 15px; /* Added margin above button */
        }

        QMessageBox QPushButton:hover {
            /* Hover Effect: Darker center, prominent border glow */
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #38bdf8, stop:1 #0ea5e9);
            border: 3px solid #ffffff; /* White border on hover */
            box-shadow: 0 0 15px rgba(14, 165, 233, 0.9);
        }

        QMessageBox QPushButton:pressed {
            /* Press Effect: Solid, dark blue */
            background: #0369a1;
            border: 3px solid #7dd3fc;
            box-shadow: none;
        }

        QMessageBox QPushButton:focus {
            outline: none;
            border: 3px solid #7dd3fc;
        }
    )");

    // Set window flags for modern look (No changes here)
    msgBox->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    // Adjust size for better appearance
    msgBox->setMinimumSize(550, 250);
    msgBox->resize(600, 300); // Slightly larger default size

    // Center the message box on the screen
    if (parent == nullptr) {
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect screenGeometry = screen->geometry();
            int x = (screenGeometry.width() - msgBox->width()) / 2;
            int y = (screenGeometry.height() - msgBox->height()) / 2;
            msgBox->move(x, y);
        }
    }

    return msgBox;
}

#endif // STYLEDMESSAGEBOX_H
