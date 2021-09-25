#pragma once
\
#include <QDialog>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>

class TAboutDialog : public QDialog {
    Q_OBJECT
    public:
        explicit TAboutDialog(QWidget *parent, Qt::WindowFlags);

    private:
        QLabel *title;
        QTextEdit *description;
};
