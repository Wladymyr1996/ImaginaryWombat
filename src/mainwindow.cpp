#include "mainwindow.h"
#include <QDebug>
#include <QEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setWindowTitle(tr("Imaginary Wombat"));
	setMinimumSize(640, 480);

	loadLanguages();
	createTopMenu();
	setGeometry(settingsHandler->GetProgramSettings().geometry);

	langHandler->SetLanguage(settingsHandler->GetProgramSettings().language);

	connect(imageHandler, &TImageHandler::isClosed, this, &MainWindow::updateFileMenu);
	connect(imageHandler, &TImageHandler::isOpened, this, &MainWindow::updateFileMenu);
}

MainWindow::~MainWindow() {
	settingsHandler->GetProgramSettings().geometry = geometry();
}

void MainWindow::changeEvent(QEvent *event) {
	if (event->type() == QEvent::LanguageChange) {
		//Оновлення текстів
		updateTextes();
		settingsHandler->GetProgramSettings().language = langHandler->getCurrentLangCode();
	} else
		QWidget::changeEvent(event);
}

void MainWindow::clearRecentFailesList() {
	settingsHandler->GetProgramSettings().recentFiles.clear();
	updateRecentMenu();
}

void MainWindow::doOpenImage() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open image"), "", tr("Images", "Open file dialog") + " (*.png *jpg *jpeg);; " + tr("All files", "Open file dialog") + "(*.*)");
	if (filename.isNull()) return;

	if (!imageHandler->openImage(filename)) {
		QMessageBox::critical(nullptr, tr("Open image error"), tr("Can't open image \"") + GlobalFunctions::getFileNameFromFilepath(filename) + "\"!");
		return;
	}

	QVector<QString> &recentFileList = settingsHandler->GetProgramSettings().recentFiles;

	int index = recentFileList.indexOf(filename);
	if (index == -1) {
		recentFileList.push_front(filename);
		if (recentFileList.count() > 5)
			recentFileList.pop_back();
	} else {
		recentFileList.remove(index);
		recentFileList.push_front(filename);
	}

	updateRecentMenu();
}

void MainWindow::doOpenRecentImage(QString filename) {
	if (!imageHandler->openImage(filename)) {
		QMessageBox::critical(nullptr, tr("Open image error"), tr("Can't open image \"") + GlobalFunctions::getFileNameFromFilepath(filename) + "\"!");
		return;
	}

	QVector<QString> &recentFileList = settingsHandler->GetProgramSettings().recentFiles;

	int index = recentFileList.indexOf(filename);
	if (index == -1) {
		recentFileList.push_front(filename);
		if (recentFileList.count() > 5)
			recentFileList.pop_back();

		qDebug() << "MainWindow::doOpenRecentImage(): WTF?!";
	} else {
		recentFileList.remove(index);
		recentFileList.push_front(filename);
	}

	updateRecentMenu();
}

void MainWindow::updateFileMenu() {
	bool active = imageHandler->imageIsOpened();

	saveStlAction->setEnabled(active);
	closeImageAction->setEnabled(active);
}

void MainWindow::updateTextes() {
	fileMenu->setTitle(tr("File"));
	openImageAction->setText(tr("Open image"));
	openRecentFileMenu->setTitle(tr("Open recent image"));
	clearOpenRecentFiles->setText(tr("Clear", "Clear open recent files list"));
	closeImageAction->setText(tr("Close image"));
	saveStlAction->setText(tr("Save 3D model as STL"));\
	exitAction->setText(tr("Exit"));

	settingsMenu->setTitle(tr("Settings"));
	printerSettingsAction->setText(tr("Printer settings"));

	aboutMenu->setTitle(tr("About"));
	aboutAction->setText(tr("About"));
	helpAction->setText(tr("Help"));
}

void MainWindow::updateRecentMenu() {
	if (recentFilesSignalMaper == nullptr) {
		recentFilesActions = new QVector<QAction *>;
		recentFilesSignalMaper = new QSignalMapper(this);

		connect(recentFilesSignalMaper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
				this, &MainWindow::doOpenRecentImage);
	} else {
		openRecentFileMenu->clear();
		for (QAction *it : *recentFilesActions)
			delete it;
		recentFilesActions->clear();
	}

	auto &fileList = settingsHandler->GetProgramSettings().recentFiles;
	for (auto &it : fileList) {
		QAction *act = openRecentFileMenu->addAction(GlobalFunctions::getFileNameFromFilepath(it));
		connect(act, &QAction::triggered, recentFilesSignalMaper,
				static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
		recentFilesSignalMaper->setMapping(act, it);
	}

	if (!fileList.isEmpty()) {
		openRecentFileMenu->addSeparator();
		openRecentFileMenu->addAction(clearOpenRecentFiles);
		openRecentFileMenu->setEnabled(true);
	}

	else openRecentFileMenu->setDisabled(true);
}

void MainWindow::loadLanguages() {
	langHandler->addLanguage("English", "en", "://img/icon/lang/en.png");
	langHandler->addLanguage("Українська", "uk", "://img/icon/lang/uk.png");
	langHandler->addLanguage("Русский", "ru", "://img/icon/lang/ru.png");
}

void MainWindow::createTopMenu() {
	topMenu = new QMenuBar;

	createFileMenu();
	createSettingsMenu();
	createAboutMenu();

	setMenuBar(topMenu);
}

void MainWindow::createFileMenu() {
	createOpenRecentFileMenu();

	fileMenu = new QMenu();
	openImageAction = fileMenu->addAction("");
	fileMenu->addMenu(openRecentFileMenu);
	openRecentFileMenu->setDisabled(openRecentFileMenu->isEmpty());
	closeImageAction = fileMenu->addAction("");
	fileMenu->addSeparator();
	saveStlAction = fileMenu->addAction("");
	fileMenu->addSeparator();
	exitAction = fileMenu->addAction("");

	topMenu->addMenu(fileMenu);

	connect(openImageAction, &QAction::triggered, this, &MainWindow::doOpenImage);
	connect(closeImageAction, &QAction::triggered, imageHandler, &TImageHandler::closeImage);
	connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

	updateFileMenu();
}

void MainWindow::createOpenRecentFileMenu() {
	openRecentFileMenu = new QMenu();
	clearOpenRecentFiles = new QAction(this);

	updateRecentMenu();

	connect(clearOpenRecentFiles, &QAction::triggered, this, &MainWindow::clearRecentFailesList);
}

void MainWindow::createSettingsMenu() {
	createLanguageMenu();

	settingsMenu = new QMenu();
	printerSettingsAction = settingsMenu->addAction("");
	settingsMenu->addSeparator();
	settingsMenu->addMenu(languageMenu);

	topMenu->addMenu(settingsMenu);
}

void MainWindow::createLanguageMenu() {
	languageMenu = new QMenu(tr("Language"));

	if (languageMenuSignalMapper == nullptr) {
		languageActions = new QVector<QAction *>;
		languageMenuSignalMapper = new QSignalMapper(this);
		connect(languageMenuSignalMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
				langHandler, &TLanguageHandler::SetLanguage);
	} else {
		languageMenu->clear();
		for (QAction *it : *languageActions)
			delete it;
		languageActions->clear();
	}

	auto langList = langHandler->getLangList();
	for (auto &it : langList) {
		QAction *act = languageMenu->addAction(it.name);

		connect(act, &QAction::triggered, languageMenuSignalMapper,
				static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
		languageMenuSignalMapper->setMapping(act, it.code);
	}
}

void MainWindow::createAboutMenu() {
	aboutMenu = new QMenu();
	helpAction = aboutMenu->addAction("");
	aboutMenu->addSeparator();
	aboutAction = aboutMenu->addAction("");

	topMenu->addMenu(aboutMenu);
}

