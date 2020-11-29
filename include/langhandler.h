#pragma once

#include <QObject>
#include <QVector>
#include <QTranslator>

class TLanguageHandler : public QObject {
	Q_OBJECT

	public:
		struct TLanguage {
			QString name;
			QString code;
			QString icon;
		};

		~TLanguageHandler();
		static TLanguageHandler *GetInstance();
		// Додає нову мову до переліку
		void addLanguage(QString name, QString code, QString icon);
		// Повертає структуру мови за її кодом
		const TLanguage &getLanguage(const QString &code);
		// Повертає весь список мов
		const QVector<TLanguage> &getLangList();

	public slots:
		// Встановлює мову програми
		void SetLanguage(const QString &code);

	private:
		TLanguageHandler();

		QTranslator *translator = nullptr;
		QVector<TLanguage> *languageList;
		const TLanguage empty = {"Unkonwn", "", ""};
};
