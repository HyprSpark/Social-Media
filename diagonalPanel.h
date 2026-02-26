#pragma once
#include <QWidget>

class DiagonalPanel : public QWidget
{
    Q_OBJECT
public:
    explicit DiagonalPanel(QWidget* parent = nullptr) : QWidget(parent) {}

protected:
    void paintEvent(QPaintEvent*) override;
};