#include "tridexel_viewer_window.hpp"

// project
#include "ui/styles/dark_orange.hpp"
#include "ui/styles/material_dark.hpp"

// generated
#include "ui_mainwindow.h"

namespace trid::ui {

TridexelViewerWindow::TridexelViewerWindow(QWidget* parent)
    : QMainWindow(parent), ui_(std::make_shared<Ui::MainWindow>()) {
    ui_->setupUi(this);

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
    dock_manager_ = std::make_shared<ads::CDockManager>(this);
    dock_manager_->setStyleSheet(styles::dark_orange());

    dock_manager_->addDockWidget(ads::LeftDockWidgetArea, add_qml_widget(QUrl("qrc:/qml/strategy.qml"), "Strategy"));
    dock_manager_->addDockWidget(ads::RightDockWidgetArea, add_qml_widget(QUrl("qrc:/qml/machining.qml"), "Machining"));
    dock_manager_->addDockWidget(ads::BottomDockWidgetArea, add_qml_widget(QUrl("qrc:/qml/dataflow.qml"), "Dataflow"));
}

ads::CDockWidget* TridexelViewerWindow::add_qml_widget(QUrl url, const QString& title) {
    auto qml_view = std::make_shared<QQuickView>(std::move(url));
    qml_view->setResizeMode(QQuickView::SizeRootObjectToView);
    qml_views_.emplace(qml_view);

    // Create a dock widget with the title Label 1 and set the created label
    // as the dock widget content
    auto dock_widget = std::make_shared<ads::CDockWidget>(title);
    dock_widget->setWidget(QWidget::createWindowContainer(qml_view.get()));
    dock_widgets_.emplace(dock_widget);

    return dock_widget.get();
}

} // namespace trid::ui
