/*
    Copyright (c) 2021-2023 Andrea Zanellato
    SPDX-License-Identifier: MIT
*/
#include "collapsiblepane.hpp"

#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>

#include <QToolButton>

class ArrowButton : public QToolButton {
    Q_OBJECT

public:
    explicit ArrowButton(QWidget* parent = nullptr);

private:
    void paintEvent(QPaintEvent*) override;
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
};

#include "collapsiblepane.moc"

ArrowButton::ArrowButton(QWidget* parent)
    : QToolButton(parent)
{
    /*
        Drawing arrow and borders using paintEvent, strange issues when using css:
        1. set the border color on :pressed or :checked selectors doesn't work
        2. the arrow is painted at the center over the label
        3. using images for arrows with state/color changes is not great
    */
    setCheckable(true);
    setStyleSheet(QLatin1String("border: none"));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

void ArrowButton::paintEvent(QPaintEvent* event)
{
    QToolButton::paintEvent(event);

    QPainter painter(this);
    QStyleOption option;
    option.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);

    painter.setRenderHint(QPainter::Antialiasing);

    int arrowSize = fontMetrics().height() / 2;
    QPalette pal = palette();
    QPolygon arrow;
    QRect r = rect();
    QRect arrowRect(r.x() + arrowSize, (r.height() - arrowSize) / 2, arrowSize, arrowSize);
    QPoint p1 = arrowRect.topLeft(), p2, p3;

    if (isChecked()) {
        p2 = arrowRect.topRight();
        p3 = (arrowRect.bottomLeft() + arrowRect.bottomRight()) / 2;
    } else {
        p2 = (arrowRect.topRight() + arrowRect.bottomRight()) / 2;
        p3 = arrowRect.bottomLeft();
    }

    painter.setPen(pal.color(QPalette::Light));
    painter.drawLine(r.topLeft(), r.topRight());
    painter.drawLine(r.topRight(), r.bottomRight());
    painter.setPen(pal.color(QPalette::Dark));
    painter.drawLine(r.bottomLeft(), r.bottomRight());
    painter.drawLine(r.topLeft(), r.bottomLeft());

    QColor c = pal.color(pal.currentColorGroup(), QPalette::ButtonText);
    painter.setBrush(c);
    painter.setPen(c);

    arrow.clear();
    arrow << p1 << p2 << p3;

    painter.drawPolygon(arrow);
}

QSize ArrowButton::minimumSizeHint() const
{
    int arrowSize = fontMetrics().height() / 2;
    return QSize(fontMetrics().horizontalAdvance(text()) + (arrowSize * 5), arrowSize * 3);
}

QSize ArrowButton::sizeHint() const
{
    return minimumSizeHint();
}

CollapsiblePane::CollapsiblePane(QWidget* parent)
    : QWidget(parent)
    , layout_(new QVBoxLayout(this))
    , button_(new ArrowButton(this))
    , widget_(nullptr)
{
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);
    layout_->addWidget(button_);
    setLayout(layout_);
}

void CollapsiblePane::setText(const QString& text)
{
    button_->setText(text);
}

void CollapsiblePane::setWidget(QWidget* widget)
{
    if (widget_) {
        disconnect(button_, &QAbstractButton::toggled, this, &CollapsiblePane::onExpandWidget);
        layout_->removeWidget(widget_);
        widget_->deleteLater();
    }
    if (widget) {
        layout_->addWidget(widget);
        widget->hide();
        connect(button_, &QAbstractButton::toggled, this, &CollapsiblePane::onExpandWidget);
    }
    widget_ = widget;
}

void CollapsiblePane::onExpandWidget(bool visible)
{
    widget_->setVisible(visible);
}
