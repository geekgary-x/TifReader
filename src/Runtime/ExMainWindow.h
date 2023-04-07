#ifndef EXMAINWINDOW_H
#define EXMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class ExMainWindow;
}

class ExMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExMainWindow(QWidget *parent = nullptr);
    ~ExMainWindow();

private slots:
    void on_actOpen_triggered();
    void on_actNew_triggered();
    void on_actQuit_triggered();   //退出程序
    void on_actFont_triggered();   //设置字体
    void on_actCut_triggered();    //剪切文本
    void on_actCopy_triggered();   //复制文本
    void on_actPaste_triggered();  //粘贴文本
    void on_actView_triggered(bool checked);  //MDI显示： Table 和 子窗口页面显示
    void on_actCascade_triggered();//级联显示
    void on_actTile_triggered();   //平铺显示
    void on_actClose_triggered();  //关闭所有子窗口
private:
    Ui::ExMainWindow *ui;
};

#endif // EXMAINWINDOW_H
