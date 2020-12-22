#include "settingshandler.h"
#include <QDebug>

TSettingsHandler::~TSettingsHandler() {
	SavePrinterSettings();
	SaveProgramSettings();
	delete fileSettings;
}

TSettingsHandler *TSettingsHandler::GetInstance() {
	static TSettingsHandler instance;

	return &instance;
}

TSettingsHandler::TPrinterSettings &TSettingsHandler::GetPrinterSettings() {
	return printerSettings;
}

const TSettingsHandler::TPrinterSettings &TSettingsHandler::GetDefaultPrinterSettings() {
	return defaultPrinterSettings;
}

TSettingsHandler::TProgramSettings &TSettingsHandler::GetProgramSettings() {
	return programSettngs;
}

void TSettingsHandler::LoadProgramSettings() {
	fileSettings->beginGroup("/Program");

	programSettngs.geometry = fileSettings->value("/Geometry", QRect(100, 100, 640, 480)).toRect();
	programSettngs.language = fileSettings->value("/Language", "en").toString();
	programSettngs.cutRectType = fileSettings->value("/CutRectType", Square).toInt();

	programSettngs.recentFiles.clear();
	int recentFilesCount = fileSettings->value("/RecentFilesCount", 0).toInt();
	if (recentFilesCount > 0) {
		fileSettings->beginGroup("/RecentFiles");

		for (int i = 0; i < recentFilesCount; i++)
			programSettngs.recentFiles.push_back(fileSettings->value("File" + QString::number(i), "").toString());

		fileSettings->endGroup();
	}

	fileSettings->endGroup();
}

void TSettingsHandler::LoadPrinterSettings() {
	fileSettings->beginGroup("/Printer");

	printerSettings.baseThickness = fileSettings->value("/BaseThickness", defaultPrinterSettings.baseThickness).toFloat();
	printerSettings.fullThickness = fileSettings->value("/FullThickness", defaultPrinterSettings.fullThickness).toFloat();
	printerSettings.unit = fileSettings->value("/Units", defaultPrinterSettings.unit).toString();

	fileSettings->endGroup();
}

QList<QString> TSettingsHandler::GetUnitsList() {
	return units.keys();
}

float TSettingsHandler::GetUnitCoef(QString unit) {
	if (unit.isEmpty())
		return units[printerSettings.unit];

	if (!units.contains(unit)) {
		qCritical() << "Unit " << unit << " not found";
		return units[printerSettings.unit];
	}

	return units[unit];
}

void TSettingsHandler::SaveProgramSettings() {
	fileSettings->beginGroup("/Program");

	fileSettings->setValue("/Geometry", programSettngs.geometry);
	fileSettings->setValue("/Language", programSettngs.language);
	fileSettings->setValue("/RecentFilesCount", programSettngs.recentFiles.count());
	fileSettings->setValue("/CutRectType", programSettngs.cutRectType);

	if (programSettngs.recentFiles.count() > 0) {
		fileSettings->beginGroup("/RecentFiles");

		for (int i = 0; i < programSettngs.recentFiles.count(); i++)
			fileSettings->setValue("/File" + QString::number(i), programSettngs.recentFiles.at(i));

		fileSettings->endGroup();
	}

	fileSettings->endGroup();
}

void TSettingsHandler::SavePrinterSettings() {
	fileSettings->beginGroup("/Printer");

	fileSettings->setValue("/BaseThickness", printerSettings.baseThickness);
	fileSettings->setValue("/FullThickness", printerSettings.fullThickness);
	fileSettings->setValue("/Units", printerSettings.unit);

	fileSettings->endGroup();
}

TSettingsHandler::TSettingsHandler() {
	fileSettings = new QSettings("ShaiTech", "ImaginaryWombat");

	units["inch"] = 0.0393700787f;
	units["mm"] = 1.0f;

	LoadPrinterSettings();
	LoadProgramSettings();
}
