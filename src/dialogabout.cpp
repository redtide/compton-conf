/*
    MIT License

    Copyright (c) 2021-2023 Andrea Zanellato <redtid3@gmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/
#include "dialogabout.hpp"
#include "ui_dialogabout.h"

#include <QAbstractButton>
#include <QFile>
#include <QDebug>
#include <QTextStream>

Qtilities::DialogAbout::DialogAbout(QWidget *parent)
    : QDialog(parent)
    , ui(new Qtilities::Ui::DialogAbout)
{
    ui->setupUi(this);
    ui->tabInfo->setLayout(ui->layTabInfo);
    ui->tabAuthors->setLayout(ui->layTabAuthors);
    ui->tabLicense->setLayout(ui->layTabLicense);

    QStringList list = {":/info", ":/authors", ":/license"};
    QStringList texts;

    for (const QString &item : list) {
        QFile f(item);
        if (!f.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "Error loading about file" << '\n';
            return;
        }
        QTextStream in(&f);
        texts.append(in.readAll());
        f.close();
    }
    QString toTranslate = texts.at(0);
    ui->txtInfo->setMarkdown(toTranslate.replace("__AUTHOR__", tr("Author")));
    ui->txtAuthors->setMarkdown(texts.at(1));
    ui->txtLicense->setMarkdown(texts.at(2));

    setWindowIcon(QIcon::fromTheme("help-about", QIcon(":/help-about")));
    setWindowTitle(tr("About"));

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &Qtilities::DialogAbout::close);
}

Qtilities::DialogAbout::~DialogAbout() { delete ui; }
