#include "QtRayTracing.h"

QtRayTracing::QtRayTracing(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.widget, &RayTracingWidget::putStatusInfo, this, &QtRayTracing::setStatusInfo);
}

QtRayTracing::~QtRayTracing()
{
}

void QtRayTracing::setStatusInfo(QString message_)
{
    ui.statusBar->showMessage(message_);
}
