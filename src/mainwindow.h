#pragma once

#include <QFileDialog>
#include <QMainWindow>
#include <QSignalMapper>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QHBoxLayout>

#include "procesor/langhandler.h"
#include "procesor/settingshandler.h"
#include "procesor/imagehandler.h"
#include "procesor/globalfunctions.h"
#include "view/imageview.h"
#include "view/tools.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = nullptr);
		~MainWindow();

	protected:
		void changeEvent(QEvent *);

	public slots:
		void clearRecentFailesList();
		void doOpenImage();
		void doOpenRecentImage(QString filename);
		void updateFileMenu();

	private:
		bool openImage(QString filename);

		void updateTextes();
		void updateRecentMenu();
		void loadLanguages();

		void createTopMenu();
		void createFileMenu();
		void createOpenRecentFileMenu();
		void createSettingsMenu();
		void createLanguageMenu();
		void createAboutMenu();

		// Меню
		QMenuBar *topMenu;

		// Меню файлів
		QMenu *fileMenu;
		QMenu *openRecentFileMenu;
		QMenu *settingsMenu;
		QMenu *languageMenu;
		QMenu *aboutMenu;

		// Дії у файловому меню
		QAction *openImageAction;
		QVector<QAction *> *recentFilesActions;
		QAction *clearOpenRecentFiles;
		QAction *saveStlAction;
		QAction *closeImageAction;
		QAction *exitAction;

		// Дії в налагштуваннях
		QAction *printerSettingsAction;
		QAction *selectLanguageAction;
		QVector<QAction *> *languageActions;

		// Дії в допомозі
		QAction *helpAction;
		QAction *aboutAction;

		QSignalMapper *recentFilesSignalMaper = nullptr;
		QSignalMapper *languageMenuSignalMapper = nullptr;
		TLanguageHandler *langHandler = TLanguageHandler::GetInstance();
		TSettingsHandler *settingsHandler = TSettingsHandler::GetInstance();
		TImageHandler *imageHandler = TImageHandler::GetInstance();
		TImageView *imageViewer;
		TTools *toolsViewer;
};
