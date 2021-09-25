#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>
#include <QAction>

#include "procesor/settingshandler.h"
#include "widgets/histogramview.h"

class TTools : public QWidget {
	Q_OBJECT

	public:
		explicit TTools(QWidget *parent = nullptr);
		void updateForms();

	public slots:
		void updateTextes();

	signals:
		void formUpdated();

	private:
		void doFormUpdate();
		void addForm(QString name);

		QComboBox *cmbForm;
		QPushButton *btnHistAuto;
		THistogramView *histogramView;

		QGroupBox *grbForm;
		QGroupBox *grbHist;

		QVector<QAction *> formList;

		TSettingsHandler *settingsHandler = TSettingsHandler::GetInstance();
        TImageHandler *imageHandler = TImageHandler::GetInstance();
};

