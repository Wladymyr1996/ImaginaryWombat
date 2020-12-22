#include "tools.h"

TTools::TTools(QWidget *parent) : QWidget(parent) {
	setFixedWidth(250);

	grbForm = new QGroupBox;
	cmbForm = new QComboBox;
	for (int i = 0; i < 2; i++) {
		cmbForm->addItem("");
	}
	cmbForm->setCurrentIndex(settingsHandler->GetProgramSettings().cutRectType);
	QVBoxLayout *formLay = new QVBoxLayout;
	formLay->addWidget(cmbForm);
	grbForm->setLayout(formLay);

	grbHist = new QGroupBox;
	histogramView = new THistogramView;
	btnHistAuto = new QPushButton;
	QVBoxLayout *histLay = new QVBoxLayout;
	histLay->addWidget(histogramView);
	histLay->addWidget(btnHistAuto);
	grbHist->setLayout(histLay);

	QVBoxLayout *lay = new QVBoxLayout;
	lay->addWidget(grbForm);
	lay->addWidget(grbHist);
	lay->addStretch();

	setLayout(lay);

	connect(cmbForm, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &TTools::doFormUpdate);
}

void TTools::updateTextes() {
	grbForm->setTitle(tr("Form"));

	cmbForm->setItemText(TSettingsHandler::TCutRectType::Square, tr("Square"));
	cmbForm->setItemText(TSettingsHandler::TCutRectType::RoundedSquare, tr("Rounded square"));

	grbHist->setTitle(tr("Histogram"));
	btnHistAuto->setText(tr("Auto"));
}

void TTools::doFormUpdate() {
	settingsHandler->GetProgramSettings().cutRectType = cmbForm->currentIndex();

	emit formUpdated();
}
