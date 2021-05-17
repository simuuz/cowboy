#include "renderwidget.h"
#include <QPainter>
#include <QImageWriter>

RenderWidget::RenderWidget(QWidget* parent) : QWidget(parent)
{
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::white);
    setPalette(palette);
    setAutoFillBackground(true);
}

void RenderWidget::DrawFrame(byte* buffer, int w, int h, int sw, int sh)
{
    if(!buffer || !w || !h)
    {
        return;
    }

    image = QImage(buffer, w, h, QImage::Format_RGB888).scaled(sw, sh, Qt::KeepAspectRatio, Qt::FastTransformation);
    update();
}

void RenderWidget::paintEvent(QPaintEvent* event)
{
    event->accept();

    QPainter painter(this);

    QRect widget_size(0, 0, painter.device()->width(), painter.device()->height());

    QImage image = this->image.scaled(widget_size.width(), widget_size.height(),
                                      maintain_aspect_ratio ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio,
                                      Qt::SmoothTransformation);

    QRect src_rect{image.rect()};

    src_rect.moveCenter(widget_size.center());
    painter.drawImage(src_rect.topLeft(), image);
}