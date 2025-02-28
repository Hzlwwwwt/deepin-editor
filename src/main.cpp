// SPDX-FileCopyrightText: 2011-2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "environments.h"
#include "utils.h"
#include "window.h"
#include "urlinfo.h"
#include "editorapplication.h"
#include "performancemonitor.h"
#include "eventlogutils.h"

#include <DApplication>
#include <DMainWindow>
#include <DWidgetUtil>
#include <DLog>

#include <QApplication>
#include <QCommandLineParser>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <iostream>
#include <DApplicationSettings>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    using namespace Dtk::Core;

    PerformanceMonitor::initializeAppStart();
    if (!QString(qgetenv("XDG_CURRENT_DESKTOP")).toLower().startsWith("deepin")) {

        setenv("XDG_CURRENT_DESKTOP", "Deepin", 1);
    }

    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "kwayland-shell");
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    //QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    EditorApplication app(argc, argv);
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    //save theme
    DApplicationSettings savetheme;

    // Parser input arguments.
    QCommandLineParser parser;
    const QCommandLineOption newWindowOption("w", "Open file in new window");
    const QCommandLineOption helpOption = parser.addHelpOption();
    parser.addOption(newWindowOption);
    parser.process(app);

    QStringList urls;
    QStringList arguments = parser.positionalArguments();

    for (const QString &path : arguments) {
        UrlInfo info(path);
        urls << info.url.toLocalFile();
    }

    bool hasWindowFlag = parser.isSet(newWindowOption);

    QDBusConnection dbus = QDBusConnection::sessionBus();
    // Start editor process if not found any editor use DBus.
    if (dbus.registerService("com.deepin.Editor")) {
        StartManager *startManager = StartManager::instance();
        //埋点记录启动数据
        QJsonObject objStartEvent{
            {"tid", Eventlogutils::StartUp},
            {"vsersion", VERSION},
            {"mode", 1},
        };
        Eventlogutils::GetInstance()->writeLogs(objStartEvent);

        if (hasWindowFlag) {
            startManager->openFilesInWindow(urls);
        } else {
            startManager->openFilesInTab(urls);
        }

        dbus.registerObject("/com/deepin/Editor", startManager, QDBusConnection::ExportScriptableSlots);

        PerformanceMonitor::initializAppFinish();
        return app.exec();
    }
    // Just send dbus message to exist editor process.
    else {
        QDBusInterface notification("com.deepin.Editor",
                                    "/com/deepin/Editor",
                                    "com.deepin.Editor",
                                    QDBusConnection::sessionBus());

        QList<QVariant> args;
        args << urls;

        if (hasWindowFlag) {
            notification.callWithArgumentList(QDBus::AutoDetect, "openFilesInWindow", args);
        } else {
            notification.callWithArgumentList(QDBus::AutoDetect, "openFilesInTab", args);
        }
    }

    return 0;
}
