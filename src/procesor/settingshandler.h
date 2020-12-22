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
			double baseThickness;
			double fullThickness;
			QString unit;
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
		// Отримати налаштування принтеру за замовчуванням
		const TPrinterSettings &GetDefaultPrinterSettings();
		// Отримати налаштування програми
		TProgramSettings &GetProgramSettings();

		// Завантажити налаштування програми із пам'яті
		void LoadProgramSettings();
		// Завантажити налаштування програми із пам'яті
		void LoadPrinterSettings();

		QList<QString> GetUnitsList();
		float GetUnitCoef(QString unit = "");

	public slots:
		// Записати налаштування програми
		void SaveProgramSettings();
		// Записати налаштування принтеру
		void SavePrinterSettings();

	private:
		TSettingsHandler();

		const TPrinterSettings defaultPrinterSettings = {0.2f, 3.0f, "mm"};
		TPrinterSettings printerSettings;
		TProgramSettings programSettngs;

		QSettings *fileSettings;
		QMap<QString, float> units;

};
