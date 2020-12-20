#include "settingsdialog.h"

#include <QDebug>
#include <QtMath>

TSettingsDialog::TSettingsDialog(QWidget *parent, Qt::WindowFlags flags) : QDialog(nullptr, flags) {
	parentPtr = parent;

	QVBoxLayout *lay = new QVBoxLayout;
	QFormLayout *formLay = new QFormLayout;
	QHBoxLayout *btnLay = new QHBoxLayout;

	lblBaseThickness = new QLabel;
	spbBaseThickness = new QDoubleSpinBox;
	spbBaseThickness->setValue(settingsHandler->GetPrinterSettings().baseThickness);
	spbBaseThickness->setMinimum(0);
	spbBaseThickness->setSingleStep(0.1f);

	lblFullThickness = new QLabel;
	spbFullThickness = new QDoubleSpinBox;
	spbFullThickness->setValue(settingsHandler->GetPrinterSettings().fullThickness);
	spbFullThickness->setMinimum(spbBaseThickness->value());
	spbFullThickness->setSingleStep(0.1f);

	formLay->addRow(lblBaseThickness, spbBaseThickness);
	formLay->addRow(lblFullThickness, spbFullThickness);

	pbtSave = new QPushButton;
	pbtReset = new QPushButton;
	pbtDefault = new QPushButton;

	btnLay->addWidget(pbtDefault);
	btnLay->addStretch();
	btnLay->addWidget(pbtReset);
	btnLay->addWidget(pbtSave);

	lay->addLayout(formLay);
	lay->addLayout(btnLay);

	setWindowFlags(Qt::WindowStaysOnTopHint);

	setLayout(lay);

	connect(pbtSave, &QPushButton::clicked, this, &TSettingsDialog::doSave);
	connect(pbtReset, &QPushButton::clicked, this, &TSettingsDialog::doReset);
	connect(pbtDefault, &QPushButton::clicked, this, &TSettingsDialog::doDefault);

	connect(spbBaseThickness, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &TSettingsDialog::doModed);
	connect(spbFullThickness, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &TSettingsDialog::doModed);

	doModed();

	setFixedSize(480, 320);
}

void TSettingsDialog::updateTextes() {
	// TODO: add support for inch
	QString unit = tr("mm");

	lblBaseThickness->setText(tr("Nuzzle size:"));
	lblFullThickness->setText(tr("First lay thickness:"));

	spbBaseThickness->setSuffix(" " + unit);
	spbFullThickness->setSuffix(" " + unit);

	pbtSave->setText(tr("Apply"));
	pbtDefault->setText(tr("Default"));
	pbtReset->setText(tr("Reset"));

	setWindowTitle(tr("Printer settings", "Title of settings window dialog"));
}

void TSettingsDialog::doSave() {
	settingsHandler->GetPrinterSettings().baseThickness = spbBaseThickness->value();
	settingsHandler->GetPrinterSettings().fullThickness = spbFullThickness->value();
}

void TSettingsDialog::doDefault() {
	spbBaseThickness->setValue(0.1f);
	spbFullThickness->setValue(3.0f);
}

void TSettingsDialog::doReset() {
	spbBaseThickness->setValue(settingsHandler->GetPrinterSettings().baseThickness);
	spbFullThickness->setValue(settingsHandler->GetPrinterSettings().fullThickness);
}

void TSettingsDialog::doModed() {
	bool moded = false;

	if (qRound(settingsHandler->GetPrinterSettings().baseThickness * 100) != qRound(spbBaseThickness->value() * 100)) moded = true;
	if (qRound(settingsHandler->GetPrinterSettings().fullThickness * 100) != qRound(spbFullThickness->value() * 100)) moded = true;

	if (moded)
		spbFullThickness->setMinimum(spbBaseThickness->value() + 0.01f);

	pbtSave->setEnabled(moded);
}

void TSettingsDialog::showEvent(QShowEvent *) {
	if (parentPtr != nullptr)
		setGeometry(QRect(parentPtr->geometry().center() - QPoint(width() / 2, height() / 2), size()));
}
/*
void TSettingsDialog::closeEvent(QCloseEvent *event) {

}
*/
