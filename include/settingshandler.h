#pragma once
#include <QObject>
#include <QSettings>
#include <QRect>

class TSettingsHandler : public QObject {
	Q_OBJECT

	public:
		struct TPrinterSettings {
			float nuzzleSize;
			unsigned firstLayThickness;
			unsigned numberOfLayer;
			float layerThickness;
		};

		struct TProgramSettings {
			QString language;
			QRect geometry;
			QVector<QString> recentFiles;
		};

		~TSettingsHandler();

		static TSettingsHandler *GetInstance();
		TPrinterSettings &GetPrinterSettings();
		TProgramSettings &GetProgramSettings();

		void LoadProgramSettings();
		void LoadPrinterSettings();

	public slots:
		void SaveProgramSettings();
		void SavePrinterSettings();

	private:
		TSettingsHandler();

		TPrinterSettings printerSettings;
		TProgramSettings programSettngs;

		QSettings *fileSettings;

};
