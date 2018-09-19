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

#include "start.h"
#include "ui_start.h"


Start::Start(QWidget *parent) :QWidget(parent),ui(new Ui::Start)
{
    ui->setupUi(this);

    // set stylesheet from style.qrc
    setStyleSheet(Utilities::getStylesheetFileContent(Utilities::StyleAppName::StartStyle));

    // set window size
    int x = static_cast<int>(Utilities::screensize().width()  * .8);
    int y = static_cast<int>(Utilities::screensize().height()  * .7);
    this->resize(x, y);

    // Configure Settings
    loadsettings();

    // Set coreapps page as active page
    on_coreApps_clicked();

    // Configure Speed Dial
    loadSpeedDial();

    // Configure Recent Activity
    if (!sm.getShowRecent()) {
        QString raFile = QDir::homePath() + "/.config/coreBox/RecentActivity";
        QFile file(raFile);
        if (!file.exists()) {
            // You can get error
            // Need a check here
            file.open(QIODevice::ReadWrite | QIODevice::Text);
            file.close();
        }
        loadRecent();
    }

    loadSession();

    fswStart = new QFileSystemWatcher();
    connect(fswStart, &QFileSystemWatcher::fileChanged, [=](const QString &path) {
        qDebug() << path;
        reload(path);
    });

    fswStart->addPaths(QStringList() << QString(QDir::homePath() + "/.config/coreBox/RecentActivity"));
                                     /*<< QString(QDir::homePath() + "/.config/coreBox/CoreBoxBook")
                                     << QString(QDir::homePath() + "/.config/coreBox/Sessions"));*/
}

Start::~Start()
{
    delete ui;
}

// ======== Core Apps ==========
void Start::on_appCollect_itemDoubleClicked(QListWidgetItem *item) // open SpeedDial on CoreApps
{
    GlobalFunc::systemAppOpener(item->text(),"");
}
// =============================


// ======== Speed Dial ==========

void Start::on_speedDialB_itemDoubleClicked(QListWidgetItem *item) // open SpeedDial on doubleclick
{
    BookmarkManage bk;
    // Function from utilities.cpp
    GlobalFunc::appSelectionEngine(bk.bookmarkPath("Speed Dial", item->text()),this);
}

void Start::loadSpeedDial() // populate SpeedDial list
{
    ui->speedDialB->clear();

    BookmarkManage bk;
    QStringList list = bk.getBookNames("Speed Dial");
    QStringList mList;
    mList.clear();

    QStringList dateTimeList;
    dateTimeList.clear();

    foreach (QString s, list) {
        dateTimeList.append(bk.bookingTime("Speed Dial", s));
    }
    Utilities::sortDateTime(dateTimeList);

    int count = list.count();
    int reverse = count - 1;
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < count; j++) {
            QString bTime = bk.bookingTime("Speed Dial", list.at(j));
            if (bTime.contains(dateTimeList.at(i))) {
                mList.insert(reverse, list.at(j));
                reverse--;
            }
        }
    }
    dateTimeList.clear();
    list.clear();

    for (int i = 0; i < mList.count(); ++i) {
        if (i == 15) {
            return;
        } else {
            QListWidgetItem *item = new QListWidgetItem(Utilities::getFileIcon(bk.bookmarkPath("Speed Dial", mList.at(i))), mList.at(i));
            item->setSizeHint(QSize(126, 109));
            ui->speedDialB->addItem(item);
        }
    }
}
// =============================


// ========== Recent activity ===========
void Start::on_recentActivitesL_itemDoubleClicked(QTreeWidgetItem *item, int column) // Open Recent activity on double click
{
    if (!item->text(column).contains("\t\t\t"))
        return;

    QStringList s = item->text(column).split("\t\t\t");

    QString appName = s.at(0);
    QString path = s.at(1);

    GlobalFunc::systemAppOpener(appName,path);

    // Function from utilities.cpp
    QString mess = appName + " opening " ;
    Utilities::messageEngine(mess, Utilities::MessageType::Info);
}

void Start::loadRecent() // populate RecentActivity list
{
    ui->recentActivitesL->clear();
    QSettings recentActivity(QDir::homePath() + "/.config/coreBox/RecentActivity", QSettings::IniFormat);
    QStringList topLevel = recentActivity.childGroups();
    Utilities::sortDate(topLevel);
    foreach (QString group, topLevel) {
        QTreeWidgetItem *topTree = new QTreeWidgetItem();
        QString groupL = Utilities::sentDateText(group);
        topTree->setText(0, groupL);
        recentActivity.beginGroup(group);
        QStringList keys = recentActivity.childKeys();
        Utilities::sortTime(keys);
        Utilities::sortTime(keys, Utilities::sortOrder::DESCENDING);

        foreach (QString key, keys) {
            QTreeWidgetItem *child = new QTreeWidgetItem();
            QString value = recentActivity.value(key).toString();
            child->setText(0, value);
            child->setIcon(0, Utilities::getAppIcon(value.split("\t\t\t").at(0)));
            topTree->addChild(child);
        }
        recentActivity.endGroup();
        ui->recentActivitesL->insertTopLevelItem(0, topTree);
    }

    if (topLevel.count())
        (ui->recentActivitesL->setExpanded(ui->recentActivitesL->model()->index(0, 0), true));
}

void Start::on_rClearActivity_clicked()
{
    ui->recentActivitesL->clear();
    QFile(QDir::homePath() + "/.config/coreBox/RecentActivity").remove();
}

// =================================

// ================= Session Activity =============
void Start::on_sessionsList_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QStringList nameList;
    QSettings session(QDir::homePath() + "/.config/coreBox/Sessions", QSettings::IniFormat);
    QStringList group = session.childGroups();
    foreach (QString s, group) {
        session.beginGroup(s);
        QStringList gl = session.childGroups();
        foreach (QString s, gl) {
            nameList.append(s);
        }
        session.endGroup();
    }

    QString selected = item->text(column);

    if (nameList.contains(selected)) {
        for (int i = 0; i < item->childCount(); i++) {

            QTreeWidgetItem *midChildT = item->child(i);
            if (midChildT->childCount()) {
                for (int j = 0; j < midChildT->childCount(); j++) {
                    GlobalFunc::systemAppOpener(midChildT->text(0), midChildT->child(j)->text(0));
                }
            } else {
                // Need to fix session
                GlobalFunc::systemAppOpener(midChildT->text(0));
            }
        }
        Utilities::messageEngine("Session restored successfully", Utilities::MessageType::Info);
    }
}

void Start::loadSession()
{
    ui->sessionsList->clear();
    QFuture<void> f = QtConcurrent::run([this]() {
    QSettings session(QDir::homePath() + "/.config/coreBox/Sessions", QSettings::IniFormat);

    // Date list
    QStringList topLevel = session.childGroups();
    Utilities::sortDate(topLevel, Utilities::sortOrder::DESCENDING);

    foreach (QString group, topLevel) {
        QTreeWidgetItem *topTree = new QTreeWidgetItem(ui->sessionsList);
        QString groupL = Utilities::sentDateText(group);

        topTree->setText(0, groupL);

        session.beginGroup(group);

        QStringList nameList = session.childGroups();
        Utilities::sortList(nameList);

        foreach (QString name, nameList) {
            QTreeWidgetItem *nameTree = new QTreeWidgetItem;
            nameTree->setText(0, name);

            session.beginGroup(name);

            QStringList midLevel = session.childGroups();

            foreach (QString gKey, midLevel) {
                session.beginGroup(gKey);

                if (session.childKeys().count() >= 1) {
                    QTreeWidgetItem *midChildT = new QTreeWidgetItem;
                    midChildT->setText(0, gKey);
                    midChildT->setIcon(0, Utilities::getAppIcon(gKey));

                    QStringList keys = session.childKeys();
                    Utilities::sortTime(keys, Utilities::sortOrder::DESCENDING, "hh.mm.ss.zzz");

                    foreach (QString key, keys) {
                        QString value = session.value(key).toString();
                        if (value.count()) {
                            QTreeWidgetItem *child = new QTreeWidgetItem;
                            child->setText(0, value);
                            child->setIcon(0, value.count() ? Utilities::getAppIcon(value) : Utilities::getAppIcon(gKey));
                            midChildT->addChild(child);
                        }
                    }

                    nameTree->addChild(midChildT);
                }

                session.endGroup();
            }

            topTree->addChild(nameTree);
            session.endGroup();
        }

        session.endGroup();
        ui->sessionsList->insertTopLevelItem(0, topTree);
    }
    });

    qRegisterMetaType<QVector<int>>("QVector<int>");

    QFutureWatcher<void>* r = new QFutureWatcher<void>();
    r->setFuture(f);
    connect(r, &QFutureWatcher<void>::finished, [this]() {
        if (ui->sessionsList->model()->hasIndex(0, 0))
            ui->sessionsList->setExpanded(ui->sessionsList->model()->index(0, 0), true);
    });
}

void Start::on_rDeleteSession_clicked()
{
    if (ui->sessionsList->currentItem()) {
        QStringList nameList;
        QSettings session(QDir::homePath() + "/.config/coreBox/Sessions", QSettings::IniFormat);
        QStringList group = session.childGroups();
        foreach (QString s, group) {
            session.beginGroup(s);
            QStringList gl = session.childGroups();
            foreach (QString s, gl) {
                nameList.append(s);
            }
            gl.clear();
            session.endGroup();
        }
        group.clear();

        QString selected = ui->sessionsList->currentItem()->text(0);

        if (nameList.contains(selected)) {
            QStringList g = session.childGroups();

            foreach (QString s, g) {
                session.beginGroup(s);
                if (session.childGroups().contains(selected)) {
                    session.remove(selected);
                }
                session.endGroup();
            }
            ui->sessionsList->currentItem()->parent()->removeChild(ui->sessionsList->currentItem());
        }
    }
}
// ======================

void Start::loadsettings() // load settings
{
    // Check is recent disabled or not
    if (sm.getShowRecent()) {
        ui->recentActivites->setVisible(0);
        ui->recentActivitesL->clear();
        ui->pages->setCurrentIndex(0);
    } else {
        ui->recentActivites->setVisible(1);
        loadRecent();
    }
}

void Start::pageClick(QPushButton *btn, int i, QString windowTitle)
{
    // all button checked false
    for (QPushButton *b : ui->pageButtons->findChildren<QPushButton*>())
        b->setChecked(false);
    btn->setChecked(true);
    ui->pages->slideInIdx(i);
    this->setWindowTitle(windowTitle + " - Start");

    if(btn == ui->recentActivites){
        ui->rClearActivity->setVisible(1);
        ui->rDeleteSession->setVisible(0);
    } else if(btn == ui->session){
        ui->rClearActivity->setVisible(0);
        ui->rDeleteSession->setVisible(1);
    } else{
        ui->rClearActivity->setVisible(0);
        ui->rDeleteSession->setVisible(0);
    }
}

void Start::on_coreApps_clicked()
{
    pageClick(ui->coreApps, 0, "CoreApps");
}

void Start::on_speedDial_clicked()
{
    pageClick(ui->speedDial, 1, "Speed Dial");
}

void Start::on_recentActivites_clicked()
{
    pageClick(ui->recentActivites, 2, "Recent Activites");
}

void Start::on_session_clicked()
{
    pageClick(ui->session, 3, "Sessions");
}

void Start::reload()
{
    loadSpeedDial();
    loadSession();
    loadsettings();
    if (!sm.getShowRecent())
        loadRecent();
    else on_coreApps_clicked();
}

void Start::reload(const QString &path)
{
    QFileInfo fi(path);
    if (fi.fileName() == "RecentActivity") {
        if (!sm.getShowRecent())
            loadRecent();
        // Hopefully this is not needed
        else on_coreApps_clicked();
    } else if (fi.fileName() == "CoreBoxBook") {
        qDebug() << path;
        loadSpeedDial();
    } else if (fi.fileName() == "Sessions") {
        loadSession();
    }
}

