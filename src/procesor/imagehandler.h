#pragma once

#include <QObject>
#include <QImage>

class TImageHandler : public QObject {
	Q_OBJECT

	public:
		static TImageHandler *GetInstance();
		bool imageIsOpened();
		const QImage &getImage();

	public slots:
		bool openImage(QString filename);
		void closeImage();

	signals:
		void isClosed();
		void isOpened();

	private:
		TImageHandler();

		bool opened;
		QImage image;

	signals:

};

