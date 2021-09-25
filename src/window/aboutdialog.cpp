#include "aboutdialog.h"
#include <QVBoxLayout>

TAboutDialog::TAboutDialog(QWidget *parent, Qt::WindowFlags flags) : QDialog(nullptr, flags) {
    setWindowTitle(tr("About"));
    setFixedSize(320, 220);

    setGeometry(QRect(parent->geometry().center() - QPoint(width() / 2, height() / 2), size()));

    title = new QLabel("<h1>" + tr("ImaginaryWombat") + "</h1>", this);
    description = new QTextEdit(this);
    description->setText(tr(
        "ImaginaryWombat — this is open source software for creating Lithophane\n"
        "\n"
        "Author: Stadnyk Volodymyr\n"
        "Tranlator: *none*\n"
    ));
    description->setReadOnly(true);

    QVBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(title);
    lay->addWidget(description);

    setLayout(lay);
}
