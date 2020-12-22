#pragma once

#include <QDialog>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>

#include "procesor/settingshandler.h"
#include "widgets/tunitdoublespinbox.h"

class TSettingsDialog : public QDialog {
	Q_OBJECT
	public:
		explicit TSettingsDialog(QWidget *parent, Qt::WindowFlags);

	public slots:
		void updateTextes();

	private slots:
		void doSave();
		void doDefault();
		void doReset();
		void doChangeUnit();
		void doModed();

	protected:
		//void closeEvent(QCloseEvent *event) override;
		void showEvent(QShowEvent *) override;

	private:
		QLabel *lblUnits;
		QLabel *lblBaseThickness;
		QLabel *lblFullThickness;

		QGroupBox *grbGeneralSettings;
		QComboBox *cmbUnits;

		QGroupBox *grbPrinterSettings;
		TUnitDoubleSpinBox *spbBaseThickness;
		TUnitDoubleSpinBox *spbFullThickness;

		QPushButton *pbtSave;
		QPushButton *pbtDefault;
		QPushButton *pbtReset;

		TSettingsHandler *settingsHandler = TSettingsHandler::GetInstance();

		QWidget *parentPtr;

		QMap<QString, QString> unitsNames;
};

