#pragma once

#include <QObject>
#include <QImage>

class TImageHandler : public QObject {
	Q_OBJECT

	public:
		enum IMG_ERROR {Ok = 0, ParseError, SizeError, RatioError};

		static TImageHandler *GetInstance();
		bool imageIsOpened();
		const QImage &getImage();
		QRect getCutRect();
		void setCutRect(const QRect &rect);
		void setHistagramLevels(int blackLvl, int whiteLvl);
		uchar *getHistogramMapPtr();
		void getHighmap(uchar *map, int widthMap, int heightMap);

	public slots:
		int openImage(QString filename);
        void resolveHistogram();
		void closeImage();

	signals:
		void isClosed();
		void isOpened();
		void isUpdated();
        void histogramChanged(int, int);

	private:
		TImageHandler();
		void convertToGray();
		void calculateHistogramMap();
		void updateImage();

		bool opened;

		QImage origImage;
		QImage tmpImage;
		QRect cutRect;

		int blackLevel = 0;
		int whiteLevel = 255;

		uchar histogramMap[256];

	signals:

};

