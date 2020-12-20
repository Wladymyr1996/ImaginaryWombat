#pragma once

#include <QDialog>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include "procesor/settingshandler.h"

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
		void doModed();

	protected:
		//void closeEvent(QCloseEvent *event) override;
		void showEvent(QShowEvent *) override;

	private:
		QLabel *lblBaseThickness;
		QLabel *lblFullThickness;

		QDoubleSpinBox *spbBaseThickness;
		QDoubleSpinBox *spbFullThickness;

		QPushButton *pbtSave;
		QPushButton *pbtDefault;
		QPushButton *pbtReset;

		TSettingsHandler *settingsHandler = TSettingsHandler::GetInstance();

		QWidget *parentPtr;
};

