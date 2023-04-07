#pragma once
#include <QLabel>
struct GDALDataset;

class RasterShow :public QLabel
{
    Q_OBJECT
public:
    explicit RasterShow(QWidget* parent = 0);
    ~RasterShow();
    bool ReadFile(QString filename);
    void ZoomIn();
    void ZoomOut();
    void RasterReset();
    void ShowRaster();
    void mousePressEvent(QMouseEvent* event);          //鼠标摁下
    void mouseMoveEvent(QMouseEvent* event);           //鼠标松开
    void mouseReleaseEvent(QMouseEvent* event);        //鼠标发射事件
    void wheelEvent(QWheelEvent* event);               //鼠标滚轮滚动
    void contextMenuEvent(QContextMenuEvent* event);   //右键菜单
    void mouseDoubleClickEvent(QMouseEvent* event);  //鼠标双击事件
    void paintEvent(QPaintEvent* event);            //绘制方框
    void Transform();
    bool Projection2ImageRowCol(double Xpj, double Ypj, int Xsize, int Ysize);
    bool ImageRowCol2Projection();
    int GetXsize();
    int GetYsize();
    void GetProjectionInfo();
    double Xproj, Yproj;

signals:
    void doubleClickSignal(int a, int b);
    void ProjSingnal(double x, double y, bool c);

public slots:
    void RedSquare_slot(int a, int b);

private slots:
    void ReadFile_Slot();
    void ZoomIn_Slot();
    void ZoomOut_Slot();
    void ZoomReset_Slot();
    void ImageRowCol2Projection_slot();
    void GetProjectionInfo_slot();
    void GetBandList_slot(QByteArray datagram);
    void CloseData_slot();

private:
    QString m_strInputRaster = NULL;     //文件名
    GDALDataset* m_pDataset = NULL;
    int pX = 0, pY = 0;
    int nSizeX, nSizeY;
    int iSize;
    int iScaleWidth;
    int iScaleHeight;
    int nBandCount;
    QPoint OldPos;          //旧的鼠标位置
    bool Pressed = false;   //鼠标是否被摁压
    double MoveSpeed;
    int mX = 0, mY = 0;
    int	band_list[3] = { 1,2,3 };
};
