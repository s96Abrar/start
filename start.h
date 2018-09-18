/*
Start is CoreApps activity viewer

Start is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see {http://www.gnu.org/licenses/}. */

#ifndef START_H
#define START_H

#include <QWidget>
#include <QResizeEvent>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>
#include <QFont>
#include <QDebug>
#include <QCollator>
#include <QMetaType>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

#include "slidingstackedwidget.h"

#include <cprime/utilities.h>
#include <cprime/settingsmanage.h>
#include <cprime/bookmarkmanage.h>
#include <cprime/globalfunctions.h>

namespace Ui {
class Start;
}

class Start : public QWidget
{
    Q_OBJECT

public:
    explicit Start(QWidget *parent = nullptr);
    ~Start();
    void reload();
    void reload(const QString &path);

private slots:
    void on_recentActivitesL_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_appCollect_itemDoubleClicked(QListWidgetItem *item);
    void on_speedDialB_itemDoubleClicked(QListWidgetItem *item);
    void on_coreApps_clicked();
    void on_speedDial_clicked();
    void on_recentActivites_clicked();
    void on_session_clicked();
    void on_sessionsList_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_rClearActivity_clicked();
    void on_rDeleteSession_clicked();


private:
    Ui::Start *ui;
    SettingsManage sm;
    QFileSystemWatcher *fswStart;

    void loadsettings();
    void loadSpeedDial();
    void loadRecent();
    void loadSession();
    void pageClick(QPushButton *btn, int i, QString windowTitle);
};

#endif // START_H
