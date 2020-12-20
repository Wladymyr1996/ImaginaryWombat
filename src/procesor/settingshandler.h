#pragma once

#include <QObject>
#include <QSettings>
#include <QRect>
#include <QVector>

class TSettingsHandler : public QObject {
	Q_OBJECT

	public:
		// Структура даних принтеру
		struct TPrinterSettings {
			float nuzzleSize;
			int firstLayThickness;
			int numberOfLayer;
			float layerThickness;
		};

		// Структура даних програми
		struct TProgramSettings {
			QString language;
			QRect geometry;
			int cutRectType;
			QVector<QString> recentFiles;
		};

		enum TCutRectType {
			Square = 0,
			RoundedSquare = 1
		};

		~TSettingsHandler();

		static TSettingsHandler *GetInstance();

		// Отримати налаштування принтера
		TPrinterSettings &GetPrinterSettings();
		// Отримати налаштування програми
		TProgramSettings &GetProgramSettings();

		// Завантажити налаштування програми із пам'яті
		void LoadProgramSettings();
		// Завантажити налаштування програми із пам'яті
		void LoadPrinterSettings();

	public slots:
		// Записати налаштування програми
		void SaveProgramSettings();
		// Записати налаштування принтеру
		void SavePrinterSettings();

	private:
		TSettingsHandler();

		TPrinterSettings printerSettings;
		TProgramSettings programSettngs;

		QSettings *fileSettings;

};
