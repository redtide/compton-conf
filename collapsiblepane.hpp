/*
    Copyright (c) 2021 Andrea Zanellato
    SPDX-License-Identifier: MIT
*/
#pragma once

#include <QWidget>

class ArrowButton;
class QVBoxLayout;

class CollapsiblePane : public QWidget {
    Q_OBJECT

public:
    explicit CollapsiblePane(QWidget* parent = nullptr);

    void setText(const QString&);
    void setWidget(QWidget*);

private:
    void onExpandWidget(bool);

    QVBoxLayout* layout_;
    ArrowButton* button_;
    QWidget* widget_;
};
