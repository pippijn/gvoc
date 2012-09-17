#-------------------------------------------------
#
# Project created by QtCreator 2012-09-11T10:17:30
#
#-------------------------------------------------

QT       += core gui network

#windows: {
    QT += multimedia
#}
#unix: {
    CONFIG += mobility
    MOBILITY += multimedia
#}

TARGET = gvoc
TEMPLATE = app

INCLUDEPATH += audio cache data managers models network sgml testsuite trainer utility

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
    TranslationModel.cpp \
    TreeItem.cpp \
    Trainer.cpp \
    OptionsDialog.cpp \
    LanguageChooser.cpp \
    WordListEditor.cpp \
    TextToSpeechEditor.cpp \
    PhoneticsEditor.cpp \
    network/TranslationDownloader.cpp \
    managers/TranslationManager.cpp \
    cache/PersistentMap.cpp \
    cache/PersistentCache.cpp \
    data/DictionaryPersister.cpp \
    data/Translation.cpp \
    data/Dictionary.cpp \
    network/PhoneticsDownloader.cpp \
    managers/PhoneticsManager.cpp \
    data/PrimaryTranslation.cpp \
    data/DictionarySection.cpp \
    network/AudioDownloader.cpp \
    managers/AudioManager.cpp \
    audio/mp3/minimp3.c \
    audio/AudioPlayer.cpp \
    audio/TextToSpeech.cpp \
    trainer/Vocabulary.cpp \
    utility/ClassRegistry.cpp \
    trainer/Vocabulary_en_zh.cpp \
    trainer/TrainingStatus.cpp \
    trainer/TrainingController.cpp \
    cache/SerialVersion.cpp \
    network/Downloader.cpp \
    sgml/QSgmlTag.cpp \
    sgml/QSgml.cpp \
    network/CaptchaHandler.cpp \
    network/CaptchaPageDownloader.cpp \
    network/CaptchaImageDownloader.cpp \
    network/CaptchaDialog.cpp \
    network/CaptchaReplyUploader.cpp \
    testsuite/TestSuite.cpp \
    LanguageManager.cpp \
    models/WordListModel.cpp \
    models/AbstractTreeModel.cpp \
    WordListView.cpp \
    PhoneticsDialog.cpp

HEADERS  += MainWindow.h \
    Translator.h \
    TranslationModel.h \
    TreeItem.h \
    Trainer.h \
    OptionsDialog.h \
    LanguageChooser.h \
    WordListEditor.h \
    TextToSpeechEditor.h \
    PhoneticsEditor.h \
    network/TranslationDownloader.h \
    managers/TranslationManager.h \
    cache/PersistentMap.h \
    cache/PersistentCache.h \
    data/DictionaryPersister.h \
    data/Translation.h \
    data/Dictionary.h \
    network/PhoneticsDownloader.h \
    managers/PhoneticsManager.h \
    data/PrimaryTranslation.h \
    data/DictionarySection.h \
    network/AudioDownloader.h \
    managers/AudioManager.h \
    audio/mp3/libc.h \
    audio/mp3/minimp3.h \
    audio/AudioPlayer.h \
    audio/TextToSpeech.h \
    trainer/Vocabulary.h \
    utility/ClassRegistry.h \
    trainer/Vocabulary_en_zh.h \
    trainer/TrainingStatus.h \
    trainer/TrainingController.h \
    cache/SerialVersion.h \
    network/Downloader.h \
    sgml/QSgmlTag.h \
    sgml/QSgml.h \
    network/CaptchaHandler.h \
    network/CaptchaPageDownloader.h \
    network/CaptchaImageDownloader.h \
    network/CaptchaDialog.h \
    network/CaptchaReplyUploader.h \
    testsuite/TestSuite.h \
    LanguageManager.h \
    models/WordListModel.h \
    models/AbstractTreeModel.h \
    WordListView.h \
    PhoneticsDialog.h

FORMS    += MainWindow.ui \
    Trainer.ui \
    OptionsDialog.ui \
    LanguageChooser.ui \
    WordListEditor.ui \
    TextToSpeechEditor.ui \
    PhoneticsEditor.ui \
    network/CaptchaDialog.ui \
    WordListView.ui \
    PhoneticsDialog.ui

RESOURCES += \
    gvoc.qrc

OTHER_FILES += \
    wordlists/words.en.zh-CN/hsk1.csv \
    wordlists/words.en.zh-CN/hsk2.csv \
    wordlists/words.en.zh-CN/extra.csv \
    wordlists/words.en.zh-CN/words-hsk.csv \
    wordlists/words.en.zh-CN/my-hsk2.csv \
    wordlists/words.en.zh-CN/my-hsk1.csv \
    wordlists/words.en.zh-CN/hsk4-simp.csv \
    wordlists/words.en.zh-CN/hsk3-simp.csv \
    wordlists/words.en.zh-CN/hsk2-simp.csv \
    wordlists/hints.en.zh-CN/hsk.txt \
    wordlists/words.en.zh-CN/simplified-hsk.csv
