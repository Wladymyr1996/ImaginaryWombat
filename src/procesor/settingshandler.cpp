#include "settingshandler.h"

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

	printerSettings.firstLayThickness = fileSettings->value("/FirstLayThickness", 10).toInt();
	printerSettings.layerThickness = fileSettings->value("/LayerThickness", 0.06f).toFloat();
	printerSettings.numberOfLayer = fileSettings->value("/NumberOfLayer", 40).toInt();
	printerSettings.nuzzleSize = fileSettings->value("/NuzzleSize", 0.4f).toFloat();

	fileSettings->endGroup();
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

	fileSettings->setValue("/FirstLayThickness", printerSettings.firstLayThickness);
	fileSettings->setValue("/LayerThickness", printerSettings.layerThickness);
	fileSettings->setValue("/NumberOfLayer", printerSettings.numberOfLayer);
	fileSettings->setValue("/NuzzleSize", printerSettings.nuzzleSize);

	fileSettings->endGroup();
}

TSettingsHandler::TSettingsHandler() {
	fileSettings = new QSettings("ShaiTech", "ImaginaryWombat");

	LoadPrinterSettings();
	LoadProgramSettings();
}
