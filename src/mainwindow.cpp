#include "mainwindow.h"
#include <QDebug>
#include <QEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setWindowTitle(tr("Imaginary Wombat"));
	setMinimumSize(640, 480);

	settingsDialog = new TSettingsDialog(this, Qt::WindowStaysOnTopHint);

	loadLanguages();
	createTopMenu();
	setGeometry(settingsHandler->GetProgramSettings().geometry);

	imageViewer = new TImageView;
	toolsViewer = new TTools;
	QHBoxLayout *lay = new QHBoxLayout;
	lay->addWidget(toolsViewer);
	lay->addWidget(imageViewer);

	QWidget *wdgt = new QWidget;
	wdgt->setLayout(lay);
	setCentralWidget(wdgt);

    langHandler->SetLanguage(settingsHandler->GetProgramSettings().language);

	connect(imageHandler, &TImageHandler::isClosed, this, &MainWindow::updateFileMenu);
	connect(imageHandler, &TImageHandler::isOpened, this, &MainWindow::updateFileMenu);
	connect(toolsViewer, &TTools::formUpdated, imageViewer, static_cast<void (TImageView::*)()>(&TImageView::repaint));

	connect(this, &MainWindow::langUpadated, this, &MainWindow::updateTextes);
	connect(this, &MainWindow::langUpadated, toolsViewer, &TTools::updateTextes);
	connect(this, &MainWindow::langUpadated, imageViewer, &TImageView::updateTextes);
	connect(this, &MainWindow::langUpadated, settingsDialog, &TSettingsDialog::updateTextes);
}

MainWindow::~MainWindow() {
	settingsHandler->GetProgramSettings().geometry = geometry();
}

void MainWindow::changeEvent(QEvent *event) {
	if (event->type() == QEvent::LanguageChange) {
		//Оновлення текстів
		settingsHandler->GetProgramSettings().language = langHandler->getCurrentLangCode();
		emit langUpadated();
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

	if (!openImage(filename)) return;

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
	if (!openImage(filename)) return;

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

void MainWindow::doOpenPrinterSettings() {
	setEnabled(false);
	settingsDialog->exec();
	setEnabled(true);
}

void MainWindow::doSaveStl() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save STL model"), QDir::homePath(), "STL (*stl)");

	if (filename.isEmpty())
		return;

	QFileInfo fi(filename);

	if (fi.suffix() != "stl")
		filename += ".stl";

	QFile stlFile(filename);
	if (!stlFile.open(QFile::WriteOnly)) {
		QMessageBox::critical(this, tr("Can't open file"), tr("Can't open for writing file") + fi.fileName());
		return;
	}

	uchar highMap[250 * 250];
	imageHandler->getHighmap(highMap, 250, 250);
	stlFile.write(stlHandler->getStl(fi.baseName(), highMap));
	stlFile.close();
}

void MainWindow::updateFileMenu() {
	bool active = imageHandler->imageIsOpened();

	saveStlAction->setEnabled(active);
	closeImageAction->setEnabled(active);
}

bool MainWindow::openImage(QString filename) {
	int openError = imageHandler->openImage(filename);
	if (openError != TImageHandler::IMG_ERROR::Ok) {
		QString msg;

		switch (openError) {
			case TImageHandler::IMG_ERROR::ParseError:
				msg = tr("Can't open file as image!"); break;
			case TImageHandler::IMG_ERROR::SizeError:
				msg = tr("Image is too small!"); break;
			case TImageHandler::IMG_ERROR::RatioError:
				msg = tr("Image aspect ratio error!"); break;
			default:
				msg = tr("Unknown error");
		}

		QMessageBox::critical(centralWidget(), tr("Open image error"), msg);
		return false;
	}
	return true;
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
//	helpAction->setText(tr("Help"));
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
		QAction *act = openRecentFileMenu->addAction(QIcon("://icons/menu/file.png"), GlobalFunctions::getFileNameFromFilepath(it));
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
	langHandler->addLanguage("English", "en", "://icons/flags/us.png");
	langHandler->addLanguage("Українська", "uk", "://icons/flags/ua.png");
//	langHandler->addLanguage("Русский", "ru", "://icons/flags/ru.png");
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
	openImageAction = fileMenu->addAction(QIcon("://icons/menu/open.png"), "");
	fileMenu->addMenu(openRecentFileMenu);
	openRecentFileMenu->setDisabled(openRecentFileMenu->isEmpty());
	closeImageAction = fileMenu->addAction(QIcon("://icons/menu/closeImage.png"), "");
	fileMenu->addSeparator();
	saveStlAction = fileMenu->addAction(QIcon("://icons/menu/save3D.png"), "");
	fileMenu->addSeparator();
	exitAction = fileMenu->addAction(QIcon("://icons/menu/exit.png"), "");

	topMenu->addMenu(fileMenu);

	connect(openImageAction, &QAction::triggered, this, &MainWindow::doOpenImage);
	connect(closeImageAction, &QAction::triggered, imageHandler, &TImageHandler::closeImage);
	connect(saveStlAction, &QAction::triggered, this, &MainWindow::doSaveStl);
	connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

	updateFileMenu();
}

void MainWindow::createOpenRecentFileMenu() {
	openRecentFileMenu = new QMenu();
	openRecentFileMenu->setIcon(QIcon("://icons/menu/openHistory.png"));
	clearOpenRecentFiles = new QAction(QIcon("://icons/menu/clear.png"), "", this);

	updateRecentMenu();

	connect(clearOpenRecentFiles, &QAction::triggered, this, &MainWindow::clearRecentFailesList);
}

void MainWindow::createSettingsMenu() {
	createLanguageMenu();

	settingsMenu = new QMenu();
	printerSettingsAction = settingsMenu->addAction(QIcon("://icons/menu/setting.png"), "");
	settingsMenu->addSeparator();
	settingsMenu->addMenu(languageMenu);

	topMenu->addMenu(settingsMenu);

	connect(printerSettingsAction, &QAction::triggered, this, &MainWindow::doOpenPrinterSettings);
}

void MainWindow::createLanguageMenu() {
	languageMenu = new QMenu(tr("Language"));
	languageMenu->setIcon(QIcon("://icons/menu/lang.png"));

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
		QAction *act = languageMenu->addAction(QIcon(it.icon), it.name);

		connect(act, &QAction::triggered, languageMenuSignalMapper,
				static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
		languageMenuSignalMapper->setMapping(act, it.code);
	}
}

void MainWindow::createAboutMenu() {
	aboutMenu = new QMenu();
//	helpAction = aboutMenu->addAction(QIcon("://icons/menu/help.png"), "");
//	aboutMenu->addSeparator();
	aboutAction = aboutMenu->addAction(QIcon("://icons/menu/help.png"), "");

	topMenu->addMenu(aboutMenu);
}

