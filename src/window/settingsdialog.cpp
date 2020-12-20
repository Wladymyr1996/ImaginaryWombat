#include "settingsdialog.h"

#include <QDebug>
#include <QtMath>

TSettingsDialog::TSettingsDialog(QWidget *parent, Qt::WindowFlags flags) : QDialog(nullptr, flags) {
	parentPtr = parent;

	QVBoxLayout *lay = new QVBoxLayout;
	QFormLayout *formLay = new QFormLayout;
	QHBoxLayout *btnLay = new QHBoxLayout;

	lblNuzzleSize = new QLabel;
	spbNuzzleSize = new QDoubleSpinBox;
	spbNuzzleSize->setValue(settingsHandler->GetPrinterSettings().nuzzleSize);
	spbNuzzleSize->setRange(0, 2);
	spbNuzzleSize->setSingleStep(0.1f);

	lblFirstLayThickness = new QLabel;
	spbFirstLayThickness = new QSpinBox;
	spbFirstLayThickness->setValue(settingsHandler->GetPrinterSettings().firstLayThickness);
	spbFirstLayThickness->setSingleStep(1);

	lblNumberOfLayer = new QLabel;
	spbNumberOfLayer = new QSpinBox;
	spbNumberOfLayer->setValue(settingsHandler->GetPrinterSettings().numberOfLayer);
	spbNumberOfLayer->setSingleStep(1);

	lblLayerThickness = new QLabel;
	spbLayerThickness = new QDoubleSpinBox;
	spbLayerThickness->setValue(settingsHandler->GetPrinterSettings().layerThickness);
	spbLayerThickness->setRange(0, 1);
	spbLayerThickness->setSingleStep(0.1f);

	formLay->addRow(lblNuzzleSize, spbNuzzleSize);
	formLay->addRow(lblFirstLayThickness, spbFirstLayThickness);
	formLay->addRow(lblNumberOfLayer, spbNumberOfLayer);
	formLay->addRow(lblLayerThickness, spbLayerThickness);

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

	connect(spbNuzzleSize, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &TSettingsDialog::doModed);
	connect(spbFirstLayThickness, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &TSettingsDialog::doModed);
	connect(spbNumberOfLayer, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &TSettingsDialog::doModed);
	connect(spbLayerThickness, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &TSettingsDialog::doModed);

	doModed();

	setFixedSize(480, 320);
	//setWindowFlags(flags);
}

void TSettingsDialog::updateTextes() {
	// TODO: add support for inch
	QString unit = tr("mm");

	lblNuzzleSize->setText(tr("Nuzzle size:"));
	lblFirstLayThickness->setText(tr("First lay thickness:"));
	lblNumberOfLayer->setText(tr("Layers number:"));
	lblLayerThickness->setText(tr("Layer thickness:"));

	spbNuzzleSize->setSuffix(" " + unit);
	spbLayerThickness->setSuffix(" " + unit);

	pbtSave->setText(tr("Apply"));
	pbtDefault->setText(tr("Default"));
	pbtReset->setText(tr("Reset"));

	setWindowTitle(tr("Printer settings", "Title of settings window dialog"));
}

void TSettingsDialog::doSave() {
	settingsHandler->GetPrinterSettings().firstLayThickness = spbFirstLayThickness->value();
	settingsHandler->GetPrinterSettings().layerThickness = spbLayerThickness->value();
	settingsHandler->GetPrinterSettings().numberOfLayer = spbNumberOfLayer->value();
	settingsHandler->GetPrinterSettings().nuzzleSize = spbNuzzleSize->value();
}

void TSettingsDialog::doDefault() {
	spbNuzzleSize->setValue(0.4f);
	spbFirstLayThickness->setValue(10);
	spbNumberOfLayer->setValue(40);
	spbLayerThickness->setValue(0.06f);
}

void TSettingsDialog::doReset() {
	spbNuzzleSize->setValue(settingsHandler->GetPrinterSettings().nuzzleSize);
	spbFirstLayThickness->setValue(settingsHandler->GetPrinterSettings().firstLayThickness);
	spbNumberOfLayer->setValue(settingsHandler->GetPrinterSettings().numberOfLayer);
	spbLayerThickness->setValue(settingsHandler->GetPrinterSettings().layerThickness);
}

void TSettingsDialog::doModed() {
	bool moded = false;

	if (settingsHandler->GetPrinterSettings().firstLayThickness != spbFirstLayThickness->value()) moded = true;
	if (qRound(settingsHandler->GetPrinterSettings().layerThickness * 100) != qRound(spbLayerThickness->value() * 100)) moded = true;
	if (settingsHandler->GetPrinterSettings().numberOfLayer != spbNumberOfLayer->value()) moded = true;
	if (qRound(settingsHandler->GetPrinterSettings().nuzzleSize * 100) != qRound(spbNuzzleSize->value() * 100)) moded = true;

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
