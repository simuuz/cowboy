#pragma once
#include <QWidget>
#include <QPaintEvent>
#include "common.h"

class RenderWidget : public QWidget
{
private:
    QImage image;
public:
    explicit RenderWidget(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent* event) override;
    bool maintain_aspect_ratio = true;
public slots:
    void DrawFrame(byte* buffer, int w, int h, int sw, int sh);
};