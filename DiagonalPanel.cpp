#include "DiagonalPanel.h"
#include <QPainter>

void DiagonalPanel::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);

    QPolygon poly;
    poly << QPoint(0, 0)
        << QPoint(width(), 0)
        << QPoint(width() - 120, height())
        << QPoint(0, height());

    p.drawPolygon(poly);
}