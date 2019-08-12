#pragma once

// thirdparty
#include <DockManager.h>
#include <QMainWindow>
#include <QQuickView>

// standard
#include <memory>
#include <unordered_set>

namespace Ui {
class MainWindow;
}

namespace trid::ui {

class TridexelViewerWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit TridexelViewerWindow(QWidget* parent = nullptr);

private:
    std::shared_ptr<Ui::MainWindow> ui_;

    // The main container for docking
    std::shared_ptr<ads::CDockManager> dock_manager_;
    std::unordered_set<std::shared_ptr<ads::CDockWidget>> dock_widgets_;

    std::unordered_set<std::shared_ptr<QQuickView>> qml_views_;
};

} // namespace trid::ui
