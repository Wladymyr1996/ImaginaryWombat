#pragma once

#include <QObject>
#include <QVector>

struct TPoint {
	float x;
	float y;
	float z;

	float &operator[](size_t index) {
		static float TPoint::*coord[] = {
			&TPoint::x,
			&TPoint::y,
			&TPoint::z,
		};
		return this->*coord[index];
	}

	double operator[](size_t index) const {
		static float TPoint::*coord[] = {
			&TPoint::x,
			&TPoint::y,
			&TPoint::z,
		};

		return this->*coord[index];
	}
};

struct TFace {
	QVector<TPoint> point;
};

class T3DModel : public QObject {
	Q_OBJECT
	public:
		explicit T3DModel(QObject *parent = nullptr);
		const QVector<TFace> &getFaces() const;

		void addFace(TPoint p1, TPoint p2, TPoint p3);
		void addFace(TFace face);
		void setName(const QString name);
		QString getName() const;
		void clear();

	private:
		QVector<TFace> faces;
		QString name;

};

