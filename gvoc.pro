#-------------------------------------------------
#
# Project created by QtCreator 2012-09-11T10:17:30
#
#-------------------------------------------------

QT       += core gui network multimedia

TARGET = gvoc
TEMPLATE = app

windows: {
    LIBS += -lqjson0
}
unix: {
    LIBS += -lqjson
}
LIBS += -L../qjson-build/lib -Wl,-rpath,../qjson-build/lib
INCLUDEPATH += ../qjson/src

SOURCES += main.cpp\
        MainWindow.cpp \
    Translator.cpp \
    Translation.cpp \
    TranslationModel.cpp \
    TreeItem.cpp \
    Trainer.cpp \
    OptionsDialog.cpp \
    LanguageChooser.cpp \
    mp3/minimp3.c \
    AudioPlayer.cpp \
    TextToSpeech.cpp \
    WordListEditor.cpp \
    TextToSpeechEditor.cpp \
    PhoneticsEditor.cpp

HEADERS  += MainWindow.h \
    Translator.h \
    Translation.h \
    TranslationModel.h \
    TreeItem.h \
    Trainer.h \
    OptionsDialog.h \
    LanguageChooser.h \
    mp3/minimp3.h \
    mp3/libc.h \
    AudioPlayer.h \
    TextToSpeech.h \
    WordListEditor.h \
    TextToSpeechEditor.h \
    PhoneticsEditor.h

FORMS    += MainWindow.ui \
    Trainer.ui \
    OptionsDialog.ui \
    LanguageChooser.ui \
    WordListEditor.ui \
    TextToSpeechEditor.ui \
    PhoneticsEditor.ui

RESOURCES += \
    gvoc.qrc
