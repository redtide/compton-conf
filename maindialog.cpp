/*
 * picom-conf - GUI configuration tool for picom X composite manager
 * Copyright (C) 2013-2018  Hong Jen Yee (PCMan) and LXQt team (as compton-conf)
 * Copyright (C) 2021 Andrea Zanellato <redtid3@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "maindialog.h"
#include "ui_maindialog.h"
#include "collapsiblepane.hpp"

#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDialogButtonBox>
#include <QColorDialog>
#include <QMessageBox>
#include <QUrl>

// dbus interface of picom
#define PICOM_SERVICE_PREFIX    "com.github.chjj.compton."
#define PICOM_PATH       "/"
#define PICOM_INTERFACE  "com.github.chjj.compton"

enum WinTypes {
    combo = 0,
    desktop,
    dialog,
    dnd,
    dock,
    dropdown_menu,
    menu,
    normal,
    notification,
    popup_menu,
    splash,
    toolbar,
    tooltip,
    unknown,
    utility
};

MainDialog::MainDialog(QString userConfigFile) {
  ui = new Ui::MainDialog;
  ui->setupUi(this);

  if(userConfigFile.isEmpty()) {
    userConfigFile_ = QFile::decodeName(qgetenv("XDG_CONFIG_HOME"));
    if(userConfigFile_.isEmpty()) {
      userConfigFile_ = QDir::homePath();
      userConfigFile_ += QLatin1String("/.config");
    }
    // QDir configDir = QDir(userConfigFile);
    // if(!configDir.exists())
    userConfigFile_ += QLatin1String("/picom.conf");
  }
  else
    userConfigFile_ = userConfigFile;

  config_init(&config_);
  if(config_read_file(&config_, userConfigFile_.toLocal8Bit().constData()) == CONFIG_FALSE) {
    // loading user config file failed
    // try our default example
    qDebug() << "load fail, try " << PICOM_CONF_DATA_DIR << "/picom.conf.example";
    config_read_file(&config_, PICOM_CONF_DATA_DIR "/picom.conf.example");
  }

  // set up signal handlers and initial values of the controls
  connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(onDialogButtonClicked(QAbstractButton*)));
  connect(ui->aboutButton, SIGNAL(clicked(bool)), SLOT(onAboutButtonClicked()));

  connect(ui->buttonBox->button(QDialogButtonBox::Help), &QPushButton::clicked, this, [=](){
    QUrl url(QStringLiteral("https://github.com/redtide/picom-conf/wiki/Picom-documentation"));
    QDesktopServices::openUrl(url);
  });
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &MainDialog::reject);
  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &MainDialog::accept);

  connect(ui->shadow_color, SIGNAL(clicked(bool)), SLOT(onColorButtonClicked()));
  double color;
  shadowColor_.setRedF(config_lookup_float(&config_, "shadow-red", &color) == CONFIG_TRUE ?  color : 0.0);
  shadowColor_.setGreenF(config_lookup_float(&config_, "shadow-green", &color) == CONFIG_TRUE ? color : 0.0);
  shadowColor_.setBlueF(config_lookup_float(&config_, "shadow-blue", &color) == CONFIG_TRUE ? color : 0.0);
  updateShadowColorButton();

  // objectNames are kept the same as config file key names.
  const QList<QWidget*> widgets = findChildren<QWidget*>();
  QList<QWidget*>::const_iterator i;
  for(i = widgets.constBegin(); i != widgets.constEnd(); ++i) {
    QWidget *child = *i;
    if(!child->isWidgetType() || child->objectName().isEmpty())
      continue;
    // objectName uses _ while config file keys uses - as separator.
    QByteArray keyName = child->objectName().replace(QLatin1Char('_'), QLatin1Char('-')).toLatin1(); // generate config key from objectName.
    if(child->inherits("QCheckBox")) {
      int val = -1;
      if(config_lookup_bool(&config_, keyName.constData(), &val) == CONFIG_TRUE)
        static_cast<QCheckBox*>(child)->setChecked((bool)val);
      connect(child, SIGNAL(toggled(bool)), SLOT(onButtonToggled(bool)));
    }
    else if(child->inherits("QDoubleSpinBox")) {
      double val;
      if(config_lookup_float(&config_, keyName.constData(), &val) == CONFIG_TRUE)
        static_cast<QDoubleSpinBox*>(child)->setValue(val);
      connect(child, SIGNAL(valueChanged(double)), SLOT(onSpinValueChanged(double)));
    }
    else if(child->inherits("QSpinBox")) {
      int val;
      if(config_lookup_int(&config_, keyName.constData(), &val) == CONFIG_TRUE)
        static_cast<QSpinBox*>(child)->setValue(val);
      connect(child, SIGNAL(valueChanged(int)), SLOT(onSpinValueChanged(int)));
    }
    else if(child->inherits("QRadioButton")) {
      if(child->parent()->inherits("QGroupBox")) {
        QByteArray groupKeyName = child->parent()->objectName().replace(QLatin1Char('_'), QLatin1Char('-')).toLatin1();
        if(keyName.startsWith(groupKeyName)) {
          const char *val;
          if(config_lookup_string(&config_, groupKeyName.constData(), &val) == CONFIG_TRUE)
              static_cast<QRadioButton*>(child)->setChecked(keyName == groupKeyName.append('-').append(val));
          connect(child, SIGNAL(toggled(bool)), SLOT(onRadioGroupToggled(bool)));
          continue;
        }
      }
      int val = -1;
      if(config_lookup_bool(&config_, keyName.constData(), &val) == CONFIG_TRUE)
        static_cast<QRadioButton*>(child)->setChecked((bool)val);
      connect(child, SIGNAL(toggled(bool)), SLOT(onButtonToggled(bool)));
    }
  }
  createWintypesTab();
}

MainDialog::~MainDialog() {
  config_destroy(&config_);
    delete ui;
}

void MainDialog::onButtonToggled(bool checked) {
  qDebug() << "toggled: " << sender()->objectName();
  // generate config key from objectName.
  QByteArray keyName = sender()->objectName().replace(QLatin1Char('_'), QLatin1Char('-')).toLatin1();
  configSetBool(keyName.constData(), checked);
  // saveConfig();
}

void MainDialog::onSpinValueChanged(double d) {
  qDebug() << "changed: " << sender()->objectName() << ": " << d;
  // generate config key from objectName.
  QByteArray keyName = sender()->objectName().replace(QLatin1Char('_'), QLatin1Char('-')).toLatin1();
  configSetFloat(keyName.constData(), d);
  // saveConfig();
}

void MainDialog::onSpinValueChanged(int i) {
  qDebug() << "changed: " << sender()->objectName() << ": " << i;
  // generate config key from objectName.
  QByteArray keyName = sender()->objectName().replace(QLatin1Char('_'), QLatin1Char('-')).toLatin1();
  configSetInt(keyName.constData(), i);
  // saveConfig();
}

void MainDialog::onRadioGroupToggled(bool checked) {
  if (checked) {
    qDebug() << "toggled: " << sender()->objectName();
    // generate config key from objectName.
    QByteArray keyName = sender()->parent()->objectName().replace(QLatin1Char('_'), QLatin1Char('-')).toLatin1();
    QByteArray val = sender()->objectName().right(sender()->objectName().size() - (keyName.size() + 1)).replace(QLatin1Char('_'), QLatin1Char('-')).toLatin1();
    configSetString(keyName.constData(), val.constData());
    // saveConfig();
  }
}

void MainDialog::saveConfig() {
  // ensure the existance of user config dir
  QString configDir = QFileInfo(userConfigFile_).dir().path();
  QDir().mkpath(configDir);
  qDebug() << userConfigFile_;
  // save the config file
  config_write_file(&config_, userConfigFile_.toLocal8Bit().constData());

  // ask picom to reload the config
  QString displayName = QString::fromLocal8Bit(qgetenv("DISPLAY"));
  for(int i = 0; i < displayName.length(); ++i) {
    if(!displayName[i].isNumber()) // replace non-numeric chars with _
      displayName[i] = QLatin1Char('_');
  }
  QString picomServiceName = QStringLiteral(PICOM_SERVICE_PREFIX) + displayName;
  QDBusInterface iface(picomServiceName, QStringLiteral(PICOM_PATH), QStringLiteral(PICOM_INTERFACE));
  if(iface.isValid()) {
    iface.call(QStringLiteral("reset"));
    // raise ourself to the top again (we'll loosing focus after reloading picom)
    activateWindow();
  }
  // FIXME: dbus interface of picom is not always available and reset() creates
  // much flickers. Maybe we should use internal dbus method set_opts().
  // Or, we can patch picom to do what we want.
}

void MainDialog::done(int res) {
  QDialog::done(res);
}

void MainDialog::onDialogButtonClicked(QAbstractButton* button) {
  if(ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
    saveConfig();
  }
}

void MainDialog::onColorButtonClicked() {
  QColorDialog dlg(shadowColor_);
  dlg.setOption(QColorDialog::ShowAlphaChannel, false);
  if(dlg.exec() == QDialog::Accepted) {
    shadowColor_ = dlg.selectedColor();
    updateShadowColorButton();
    configSetFloat("shadow-red", shadowColor_.redF());
    configSetFloat("shadow-green", shadowColor_.greenF());
    configSetFloat("shadow-blue", shadowColor_.blueF());
  }
}

void MainDialog::onAboutButtonClicked() {
  QMessageBox::about(this, tr("About PicomConf"),
    tr("PicomConf - configuration tool for picom\n\n"
    "Copyright (C) 2013\n"
    "Author: Hong Jen Yee (PCMan) <pcman.tw@gmail.com>\n\n"
    "Copyright (C) 2021\n"
    "Andrea Zanellato <redtid3@gmail.com>"));
}

void MainDialog::updateShadowColorButton() {
  QString qss = QStringLiteral("QPushButton {"
  "background-color:%1;"
  "}").arg(shadowColor_.name());
  ui->shadow_color->setStyleSheet(qss);
}

void MainDialog::configSetInt(const char* key, int val) {
  config_setting_t* setting = config_lookup(&config_, key);
  if(!setting) { // setting not found
    // add a new setting for it
    config_setting_t* root = config_root_setting(&config_);
    setting = config_setting_add(root, key, CONFIG_TYPE_INT);
  }
  config_setting_set_int(setting, val);
}

void MainDialog::configSetFloat(const char* key, double val) {
  config_setting_t* setting = config_lookup(&config_, key);
  if(!setting) { // setting not found
    // add a new setting for it
    config_setting_t* root = config_root_setting(&config_);
    setting = config_setting_add(root, key, CONFIG_TYPE_FLOAT);
  }
  config_setting_set_float(setting, val);
}

void MainDialog::configSetBool(const char* key, bool val) {
  config_setting_t* setting = config_lookup(&config_, key);
  if(!setting) { // setting not found
    // add a new setting for it
    config_setting_t* root = config_root_setting(&config_);
    setting = config_setting_add(root, key, CONFIG_TYPE_BOOL);
  }
  config_setting_set_bool(setting, val);
}

void MainDialog::configSetString(const char *key, const char *val)
{
  config_setting_t* setting = config_lookup(&config_, key);
  if(!setting) { // setting not found
    // add a new setting for it
    config_setting_t* root = config_root_setting(&config_);
    setting = config_setting_add(root, key, CONFIG_TYPE_STRING);
  }
  config_setting_set_string(setting, val);
}

void MainDialog::createWintypesTab()
{
    QScrollArea* scrollArea = new QScrollArea(ui->tabWidget);
    QWidget* scrollContents = new QWidget(scrollArea);
    QVBoxLayout* layout = new QVBoxLayout(scrollContents);

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(scrollContents);
    scrollArea->setWidgetResizable(true);

    CollapsiblePane* collapsiblePane[15];
    QWidget* widget[15];
    QGridLayout* gridLayout[15];
    QLabel* opacity_label[15];

    QDoubleSpinBox* opacity[15];
    QCheckBox* clip_shadow_above[15];
    QCheckBox* fade[15];
    QCheckBox* focus[15];
    QCheckBox* full_shadow[15];
    QCheckBox* redir_ignore[15];
    QCheckBox* shadow[15];

    for (int i = 0; i < 15; ++i) {
        collapsiblePane[i] = new CollapsiblePane(scrollContents);
        scrollContents->layout()->addWidget(collapsiblePane[i]);
        widget[i] = new QWidget(collapsiblePane[i]);
        gridLayout[i] = new QGridLayout(widget[i]);
        gridLayout[i]->setContentsMargins(10, 10, 10, 10);

        opacity_label[i] = new QLabel(tr("Opacity:"), widget[i]);

        opacity[i] = new QDoubleSpinBox(widget[i]);
        opacity[i]->setMaximum(1.00);
        opacity[i]->setSingleStep(0.05);

        clip_shadow_above[i] = new QCheckBox(tr("Clip shadow above"), widget[i]);
        fade[i] = new QCheckBox(tr("Fade"), widget[i]);
        focus[i] = new QCheckBox(tr("Focus"), widget[i]);
        full_shadow[i] = new QCheckBox(tr("Full shadow"), widget[i]);
        redir_ignore[i] = new QCheckBox(tr("Ignore redirection"), widget[i]);
        shadow[i] = new QCheckBox(tr("Shadow"), widget[i]);

        const QString si = QString::number(i);

        opacity[i]->setObjectName(QLatin1String("opacity_") + si);
        clip_shadow_above[i]->setObjectName(QLatin1String("clip-shadow-above_") + si);
        fade[i]->setObjectName(QLatin1String("fade_") + si);
        focus[i]->setObjectName(QLatin1String("focus_") + si);
        full_shadow[i]->setObjectName(QLatin1String("full-shadow_") + si);
        redir_ignore[i]->setObjectName(QLatin1String("redir-ignore_") + si);
        shadow[i]->setObjectName(QLatin1String("shadow_") + si);

        gridLayout[i]->addWidget(fade[i], 0, 0, 1, 1);
        gridLayout[i]->addWidget(focus[i], 0, 1, 1, 1);
        gridLayout[i]->addWidget(redir_ignore[i], 0, 2, 1, 1);
        gridLayout[i]->addWidget(shadow[i], 1, 0, 1, 1);
        gridLayout[i]->addWidget(full_shadow[i], 1, 1, 1, 1);
        gridLayout[i]->addWidget(clip_shadow_above[i], 1, 2, 1, 1);
        gridLayout[i]->addWidget(opacity_label[i], 2, 0, 1, 1);
        gridLayout[i]->addWidget(opacity[i], 2, 1, 1, 1);

        switch(i) {
        #define WINTYPE_CASE(X) \
            case X: {\
                QString s = QLatin1String(#X); \
                widget[i]->setObjectName(s); \
                s = s.at(0).toUpper() \
                    + s.mid(1).replace(QLatin1Char('_'), QLatin1Char(' ')); \
                collapsiblePane[i]->setText(tr(s.toStdString().c_str())); \
                break; \
            }
        WINTYPE_CASE(combo);
        WINTYPE_CASE(desktop);
        WINTYPE_CASE(dialog);
        WINTYPE_CASE(dnd);
        WINTYPE_CASE(dock);
        WINTYPE_CASE(dropdown_menu);
        WINTYPE_CASE(menu);
        WINTYPE_CASE(normal);
        WINTYPE_CASE(notification);
        WINTYPE_CASE(popup_menu);
        WINTYPE_CASE(splash);
        WINTYPE_CASE(toolbar);
        WINTYPE_CASE(tooltip);
        WINTYPE_CASE(unknown);
        WINTYPE_CASE(utility);

        #undef WINTYPE_CASE

        default:
            break;
        }
        collapsiblePane[i]->setWidget(widget[i]);

        QString winType = widget[i]->objectName();

        double value;
        QString path = QLatin1String("wintypes.") + winType
            + QLatin1Char('.') + QLatin1String(".opacity");

        if(config_lookup_float(&config_, path.toUtf8().constData(), &value) == CONFIG_TRUE)
            opacity[i]->setValue(value);

        int val;
        QString type;

        #define SETUP_CHECKBOX(TYPE) \
            val = -1; \
            type = QLatin1String(#TYPE); \
            path = QLatin1String("wintypes.") + winType \
                + QLatin1Char('.') + type.replace(QLatin1Char('_'), QLatin1Char('-')); \
            if(config_lookup_bool(&config_, path.toUtf8().constData(), &val) == CONFIG_TRUE) \
                TYPE[i]->setChecked((bool)val)

        SETUP_CHECKBOX(clip_shadow_above);
        SETUP_CHECKBOX(fade);
        SETUP_CHECKBOX(focus);
        SETUP_CHECKBOX(shadow);
        SETUP_CHECKBOX(redir_ignore);
        SETUP_CHECKBOX(shadow);

        #undef SETUP_CHECKBOX

        connect(opacity[i], QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainDialog::onWintypeOpacityChanged);

        connect(clip_shadow_above[i], &QCheckBox::clicked,
            this, &MainDialog::onWintypeBoolClicked);

        connect(fade[i], &QCheckBox::clicked,
            this, &MainDialog::onWintypeBoolClicked);

        connect(focus[i], &QCheckBox::clicked,
            this, &MainDialog::onWintypeBoolClicked);

        connect(full_shadow[i], &QCheckBox::clicked,
            this, &MainDialog::onWintypeBoolClicked);

        connect(redir_ignore[i], &QCheckBox::clicked,
            this, &MainDialog::onWintypeBoolClicked);

        connect(shadow[i], &QCheckBox::clicked,
            this, &MainDialog::onWintypeBoolClicked);

    }
    ui->tabWidget->insertTab(3, scrollArea, tr("Window Types"));
}

void MainDialog::onWintypeBoolClicked(bool checked)
{
    qDebug() << "toggled: " << sender()->objectName();
    const QString strType = sender()->parent()->objectName();
    const QString strKey = sender()->objectName().split(QLatin1Char('_')).first();
    const QString strPath =
        QLatin1String("wintypes.") + strType + QLatin1Char('.') + strKey;

    config_setting_t* setting = config_lookup(&config_, strPath.toUtf8().constData());

    if(!setting) {
        config_setting_t* root = config_root_setting(&config_);
        config_setting_t* wintypes = config_setting_get_member(root, "wintypes");

        if (!wintypes)
            wintypes = config_setting_add(root, "wintypes", CONFIG_TYPE_GROUP);

        setting = config_setting_get_member(wintypes, strType.toUtf8().constData());
        if (!setting)
            setting = config_setting_add(wintypes, strType.toUtf8().constData(), CONFIG_TYPE_GROUP);

        setting = config_setting_add(setting, strKey.toUtf8().constData(), CONFIG_TYPE_BOOL);
        if (!setting) {
            qDebug() << "Error while creating setting: " << strPath;
            return;
        }
    }
    config_setting_set_bool(setting, checked);
}

void MainDialog::onWintypeOpacityChanged(double value)
{
    qDebug() << "changed: " << sender()->objectName() << ": " << value;
    const QString strType = sender()->parent()->objectName();
    const QString strKey = QLatin1String("opacity");
    const QString strPath =
        QLatin1String("wintypes.") + strType + QLatin1String(".opacity");

    config_setting_t* setting = config_lookup(&config_, strPath.toUtf8().constData());

    if(!setting) {
        config_setting_t* root = config_root_setting(&config_);
        config_setting_t* wintypes = config_setting_get_member(root, "wintypes");

        if (!wintypes)
            wintypes = config_setting_add(root, "wintypes", CONFIG_TYPE_GROUP);

        setting = config_setting_get_member(wintypes, strType.toUtf8().constData());
        if (!setting)
            setting = config_setting_add(wintypes, strType.toUtf8().constData(), CONFIG_TYPE_GROUP);

        setting = config_setting_add(setting, strKey.toUtf8().constData(), CONFIG_TYPE_FLOAT);
        if (!setting) {
            qDebug() << "Error while creating setting: " << strPath;
            return;
        }
    }
    config_setting_set_float(setting, value);
}
