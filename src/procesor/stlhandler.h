#pragma once

#include <QObject>

#include "procesor/settingshandler.h"
#include "procesor/3dmodel.h"

class TStlHandler : public QObject
{
	Q_OBJECT
	public:
		const QByteArray &getStl(QString name, uchar *highMap);
		static TStlHandler *getInstance();

	private:
		TStlHandler();

		void generateModel(T3DModel &model, uchar *highMap);
		void generateStlFileData(const T3DModel &model, QByteArray &data);
		double calculateZ(int value);

		TSettingsHandler *settingsHandler = TSettingsHandler::GetInstance();
		T3DModel model;
		QByteArray fileData;
};

