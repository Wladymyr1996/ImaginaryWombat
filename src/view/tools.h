#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>
#include <QAction>

#include "procesor/settingshandler.h"
#include "view/histogramview.h"

class TTools : public QWidget {
	Q_OBJECT

	public:
		explicit TTools(QWidget *parent = nullptr);

		void updateTextes();
		void updateForms();

	signals:
		void formUpdated();

	private:
		void doFormUpdate();

	private:
		void addForm(QString name);

		QComboBox *cmbForm;
		QPushButton *btnHistAuto;
		THistogramView *histogramView;

		QGroupBox *grbForm;
		QGroupBox *grbHist;

		QVector<QAction *> formList;

		TSettingsHandler *settingsHandler = TSettingsHandler::GetInstance();
};

