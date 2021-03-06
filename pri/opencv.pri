win32{
	INCLUDEPATH += C:/projects/opencv/include
	LIBS += -LC:/projects/opencv/x64/vc14/lib
	contains( DEFINES, DEBUG ){
		OPEN_CV_LIB_SUFFIX="d"
	}
	OPEN_CV_VERSION=2412
}
unix{
	OPEN_CV_VERSION=
}
LIBS += -lopencv_flann$${OPEN_CV_VERSION}$${OPEN_CV_LIB_SUFFIX}
LIBS += -lopencv_imgproc$${OPEN_CV_VERSION}$${OPEN_CV_LIB_SUFFIX}
LIBS += -lopencv_features2d$${OPEN_CV_VERSION}$${OPEN_CV_LIB_SUFFIX}
LIBS += -lopencv_highgui$${OPEN_CV_VERSION}$${OPEN_CV_LIB_SUFFIX}
LIBS += -lopencv_legacy$${OPEN_CV_VERSION}$${OPEN_CV_LIB_SUFFIX}
LIBS += -lopencv_core$${OPEN_CV_VERSION}$${OPEN_CV_LIB_SUFFIX}
LIBS += -lopencv_video$${OPEN_CV_VERSION}$${OPEN_CV_LIB_SUFFIX}
LIBS += -lopencv_contrib$${OPEN_CV_VERSION}$${OPEN_CV_LIB_SUFFIX}
LIBS += -lopencv_superres$${OPEN_CV_VERSION}$${OPEN_CV_LIB_SUFFIX}

