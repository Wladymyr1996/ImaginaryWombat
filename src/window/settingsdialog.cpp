#include "settingsdialog.h"

#include <QDebug>
#include <QtMath>

TSettingsDialog::TSettingsDialog(QWidget *parent, Qt::WindowFlags flags) : QDialog(nullptr, flags) {
	parentPtr = parent;

	QVBoxLayout *lay = new QVBoxLayout;
	QFormLayout *formGeneralLay = new QFormLayout;
	QFormLayout *formPrinterLay = new QFormLayout;
	QHBoxLayout *btnLay = new QHBoxLayout;

	lblUnits = new QLabel;
	cmbUnits = new QComboBox;
	for (auto &it : settingsHandler->GetUnitsList()) {
		cmbUnits->addItem(it, it);
	}
	cmbUnits->setCurrentText(settingsHandler->GetPrinterSettings().unit);

	formGeneralLay->addRow(lblUnits, cmbUnits);

	grbGeneralSettings = new QGroupBox;
	grbGeneralSettings->setLayout(formGeneralLay);

	lblBaseThickness = new QLabel;
	spbBaseThickness = new TUnitDoubleSpinBox(settingsHandler->GetUnitCoef());
	spbBaseThickness->setDecimals(3);
	spbBaseThickness->setCoefValue(settingsHandler->GetPrinterSettings().baseThickness);
	spbBaseThickness->setMinimum(0);
	spbBaseThickness->setSingleStep(1);
	spbBaseThickness->setStepType(TUnitDoubleSpinBox::AdaptiveDecimalStepType);

	lblFullThickness = new QLabel;
	spbFullThickness = new TUnitDoubleSpinBox(settingsHandler->GetUnitCoef());
	spbFullThickness->setDecimals(3);
	spbFullThickness->setCoefValue(settingsHandler->GetPrinterSettings().fullThickness);
	spbFullThickness->setMinimum(spbBaseThickness->value());
	spbFullThickness->setSingleStep(1);
	spbFullThickness->setStepType(TUnitDoubleSpinBox::AdaptiveDecimalStepType);


	formPrinterLay->addRow(lblBaseThickness, spbBaseThickness);
	formPrinterLay->addRow(lblFullThickness, spbFullThickness);

	grbPrinterSettings = new QGroupBox;
	grbPrinterSettings->setLayout(formPrinterLay);

	pbtSave = new QPushButton;
	pbtReset = new QPushButton;
	pbtDefault = new QPushButton;

	btnLay->addWidget(pbtDefault);
	btnLay->addStretch();
	btnLay->addWidget(pbtReset);
	btnLay->addWidget(pbtSave);

	lay->addWidget(grbGeneralSettings);
	lay->addWidget(grbPrinterSettings);
	lay->addStretch();
	lay->addLayout(btnLay);

	setWindowFlags(Qt::WindowStaysOnTopHint);

	setLayout(lay);

	connect(pbtSave, &QPushButton::clicked, this, &TSettingsDialog::doSave);
	connect(pbtSave, &QPushButton::clicked, this, &TSettingsDialog::doModed);
	connect(pbtReset, &QPushButton::clicked, this, &TSettingsDialog::doReset);
	connect(pbtDefault, &QPushButton::clicked, this, &TSettingsDialog::doDefault);
	connect(cmbUnits, &QComboBox::currentTextChanged, this, &TSettingsDialog::doChangeUnit);

	connect(spbBaseThickness, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &TSettingsDialog::doModed);
	connect(spbFullThickness, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &TSettingsDialog::doModed);

	doModed();

	setFixedSize(480, 320);
}

void TSettingsDialog::updateTextes() {
	unitsNames["mm"] = tr("mm");
	unitsNames["inch"] = tr("inch");
	for (auto &key : unitsNames.keys())
		cmbUnits->setItemText(cmbUnits->findData(key), unitsNames[key]);

	QString unit = unitsNames[settingsHandler->GetPrinterSettings().unit];

	grbGeneralSettings->setTitle(tr("General settings"));
	grbPrinterSettings->setTitle(tr("Printer settings", "as value"));

	lblUnits->setText(tr("Units", "Settings dialog"));
	lblBaseThickness->setText(tr("Base thickness:"));
	lblFullThickness->setText(tr("Full thickness:"));

	spbBaseThickness->setSuffix(" " + unit);
	spbFullThickness->setSuffix(" " + unit);

	pbtSave->setText(tr("Apply"));
	pbtDefault->setText(tr("Default"));
	pbtReset->setText(tr("Reset"));

	setWindowTitle(tr("Printer settings", "Title of settings window dialog"));
}

void TSettingsDialog::doSave() {
	settingsHandler->GetPrinterSettings().baseThickness = spbBaseThickness->coefValue();
	settingsHandler->GetPrinterSettings().fullThickness = spbFullThickness->coefValue();
	settingsHandler->GetPrinterSettings().unit = cmbUnits->currentData().toString();
}

void TSettingsDialog::doDefault() {
	cmbUnits->setCurrentIndex(cmbUnits->findData(settingsHandler->GetDefaultPrinterSettings().unit));
	spbBaseThickness->setCoefValue(settingsHandler->GetDefaultPrinterSettings().baseThickness);
	spbFullThickness->setCoefValue(settingsHandler->GetDefaultPrinterSettings().fullThickness);
}

void TSettingsDialog::doReset() {
	cmbUnits->setCurrentIndex(cmbUnits->findData(settingsHandler->GetPrinterSettings().unit));
	spbBaseThickness->setCoefValue(settingsHandler->GetPrinterSettings().baseThickness);
	spbFullThickness->setCoefValue(settingsHandler->GetPrinterSettings().fullThickness);
}

void TSettingsDialog::doChangeUnit() {
	QString suffix = " " + unitsNames[cmbUnits->currentText()];
	double coef = settingsHandler->GetUnitCoef(cmbUnits->currentText());

	spbBaseThickness->setMinimum(0);
	spbFullThickness->setMinimum(0);
	if (coef < 1) {
		spbBaseThickness->setCoefficient(coef);
		spbFullThickness->setCoefficient(coef);
	} else {
		spbFullThickness->setCoefficient(coef);
		spbBaseThickness->setCoefficient(coef);
	}
	spbBaseThickness->setSuffix(suffix);
	spbFullThickness->setSuffix(suffix);

	doModed();
}

void TSettingsDialog::doModed() {
	bool moded = false;

	if (settingsHandler->GetPrinterSettings().unit != cmbUnits->currentText()) moded = true;
	if (qRound(settingsHandler->GetPrinterSettings().baseThickness * 100) != qRound(spbBaseThickness->coefValue() * 100)) moded = true;
	if (qRound(settingsHandler->GetPrinterSettings().fullThickness * 100) != qRound(spbFullThickness->coefValue() * 100)) moded = true;

	if (moded) {
		spbFullThickness->setMinimum(spbBaseThickness->value() + 0.01f);
	}

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
