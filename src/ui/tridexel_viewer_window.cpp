#include "tridexel_viewer_window.hpp"

// project
#include "ui/styles/dark_orange.hpp"
#include "ui/styles/material_dark.hpp"

// generated
#include "ui_mainwindow.h"

#include <QQmlEngine>
#include <QQuickStyle>
#include <QQuickView>
#include <QQuickWidget>
#include <QWidget>

namespace trid::ui {

TridexelViewerWindow::TridexelViewerWindow(QWidget* parent)
    : QMainWindow(parent), ui_(std::make_shared<Ui::MainWindow>()) {
    ui_->setupUi(this);

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
    dock_manager_ = std::make_shared<ads::CDockManager>(this);
    dock_manager_->setStyleSheet(styles::material_dark());

    auto qml_view = std::make_shared<QQuickView>(QUrl("qrc:/qml/main.qml"));
    qml_view->setResizeMode(QQuickView::SizeRootObjectToView);
    qml_views_.emplace(qml_view);

    // Create a dock widget with the title Label 1 and set the created label
    // as the dock widget content
    auto dock_widget = std::make_shared<ads::CDockWidget>("main");
    dock_widget->setWidget(QWidget::createWindowContainer(qml_view.get()));
    dock_widgets_.emplace(dock_widget);

    // Add the dock widget to the center dock widget area
    dock_manager_->addDockWidget(ads::CenterDockWidgetArea, dock_widget.get());
}

} // namespace trid::ui
