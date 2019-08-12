// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ui/tridexel_viewer_window.hpp"

#include <DockManager.h>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QSurfaceFormat>

namespace {

void setSurfaceFormat() {
    QSurfaceFormat format;
    format.setVersion(4, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);

    format.setDepthBufferSize(24);
    format.setSamples(4);
    format.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);
}

} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    setSurfaceFormat();

    trid::ui::TridexelViewerWindow window;
    window.show();

    return QApplication::exec();
}
