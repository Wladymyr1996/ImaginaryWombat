#include "globalfunctions.h"

QString GlobalFunctions::getFileNameFromFilepath(const QString &filepath) {
#ifdef _WIN32
	return filepath.split("\\").last());
#elif __linux__
	return filepath.split("/").last();
#else
	#error OS not support or not tested
#endif
}
