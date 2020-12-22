#pragma once

#include <QObject>
#include <QVector>
#include <QPoint>

struct TPoint {
	float x;
	float y;
	float z;

	TPoint() {};

	TPoint(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	TPoint(QPointF xy, float z) {
		*this = xy;
		this->z = z;
	}

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

	TPoint &operator+=(const QPointF &rhs) {
		x += rhs.x();
		y += rhs.y();

		return *this;
	}

	TPoint &operator=(const QPointF &rhs) {
		x = rhs.x();
		y = rhs.y();
		return *this;
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
		void addFace(TPoint *point, int count);
		void addFace(TFace face);
		void setName(const QString name);
		QString getName() const;
		void clear();
		void translate(TPoint p);

	private:
		QVector<TFace> faces;
		QString name;

};

