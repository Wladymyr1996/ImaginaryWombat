#include "langhandler.h"
#include <QMessageBox>
#include <QApplication>
#include <QDebug>

static const QString className = "TLanguageHandler: ";

//Отримання посилання на єдиний екземпляр класу
TLanguageHandler *TLanguageHandler::GetInstance() {
	static TLanguageHandler instance;

	return &instance;
}

void TLanguageHandler::addLanguage(QString name, QString code, QString icon) {
	languageList->push_back(TLanguage{name, code, icon});
}

const TLanguageHandler::TLanguage &TLanguageHandler::getLanguage(const QString &code) {
	for (auto &it : *languageList) {
		if (it.code == code)
			return it;
	}

	return empty;
}

const QVector<TLanguageHandler::TLanguage> &TLanguageHandler::getLangList() {
	return *languageList;
}

void TLanguageHandler::SetLanguage(const QString &code) {
	if (translator == nullptr) {
		qCritical() << className << "translator is null";
		return;
	}

	QString langPath = "://lang/" + code + ".qm";
	if (!translator->load(langPath)) {
		QMessageBox::critical(nullptr, tr("Language error"), tr("Can't load language \"") + getLanguage(code).name + "\"");
	}
	return;
}

TLanguageHandler::TLanguageHandler() {
	languageList = new QVector<TLanguage>;
	translator = new QTranslator;

	QApplication::instance()->installTranslator(translator);
}

TLanguageHandler::~TLanguageHandler() {
	delete languageList;
	delete translator;
}
