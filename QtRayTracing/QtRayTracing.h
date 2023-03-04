#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include "ui_QtRayTracing.h"
#include "QtRayTracing.h"

class QtRayTracing : public QMainWindow
{
    Q_OBJECT

public:
    QtRayTracing(QWidget *parent = nullptr);
    ~QtRayTracing();

private:
    Ui::QtRayTracingClass ui;

public slots:
    void setStatusInfo(QString message_);
};
