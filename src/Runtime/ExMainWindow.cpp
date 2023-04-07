#include "ExMainWindow.h"
#include "ui_ExMainWindow.h"
#include <QDir>
#include <QFileDialog>
#include <QMdiSubWindow>
#include "ExMDI.h"
ExMainWindow::ExMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ExMainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->mdiArea);
    setWindowState(Qt::WindowMaximized);
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

ExMainWindow::~ExMainWindow()
{
    delete ui;
}

void ExMainWindow::on_actNew_triggered()
{
    ExMDI* mdi = new ExMDI(this);
    ui->mdiArea->addSubWindow(mdi);
    mdi->show();

    ui->actCopy->setEnabled(true);
    ui->actCut->setEnabled(true);
    ui->actPaste->setEnabled(true);
    ui->actFont->setEnabled(true);
}

void ExMainWindow::on_actQuit_triggered()
{
    close();
}

void ExMainWindow::on_actFont_triggered()
{
    ExMDI* mdi = (ExMDI*)ui->mdiArea->activeSubWindow()->widget();
    mdi->setEditFont();
}

void ExMainWindow::on_actCut_triggered()
{
    ExMDI* mdi = (ExMDI*)ui->mdiArea->activeSubWindow()->widget();
    mdi->textCut();
}

void ExMainWindow::on_actCopy_triggered()
{
    ExMDI* mdi = (ExMDI*)ui->mdiArea->activeSubWindow()->widget();
    mdi->textCopy();
}

void ExMainWindow::on_actPaste_triggered()
{
    ExMDI* mdi = (ExMDI*)ui->mdiArea->activeSubWindow()->widget();
    mdi->textPaste();
}

void ExMainWindow::on_actView_triggered(bool checked)
{
    if (checked)
    {
        ui->mdiArea->setViewMode(QMdiArea::TabbedView);
        ui->mdiArea->setTabsClosable(true);
        ui->actCascade->setEnabled(false);
        ui->actTile->setEnabled(false);
    }
    else
    {
        ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
        ui->actCascade->setEnabled(true);
        ui->actTile->setEnabled(true);
    }
}

void ExMainWindow::on_actCascade_triggered()
{
    ui->mdiArea->cascadeSubWindows();
}

void ExMainWindow::on_actTile_triggered()
{
    ui->mdiArea->tileSubWindows();
}

void ExMainWindow::on_actClose_triggered()
{
    ui->mdiArea->closeAllSubWindows();
}

void ExMainWindow::on_actOpen_triggered()
{
    bool needNew = false;
    ExMDI* mdi = nullptr;

    if (ui->mdiArea->subWindowList().count() > 0) {  //如果有打开的主窗口，获取活动窗口
        mdi = (ExMDI*)ui->mdiArea->activeSubWindow()->widget();
        needNew = mdi->isFileOpended();   //文件已经打开，需要新建窗口
    }
    else {
        needNew = true;
    }

    QString curPath = QDir::currentPath();
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开一个文件"), curPath, QString::fromLocal8Bit("C程序文件(*.h *cpp);;文本文件(*.txt);;所有文件(*.*)"));
    if (fileName.isEmpty()) return;

    if (needNew)
    {
        mdi = new ExMDI(this);
        ui->mdiArea->addSubWindow(mdi);
    }
    

    mdi->loadFromFile(fileName);
    mdi->show();

    ui->actCopy->setEnabled(true);
    ui->actCut->setEnabled(true);
    ui->actPaste->setEnabled(true);
    ui->actFont->setEnabled(true);
}

