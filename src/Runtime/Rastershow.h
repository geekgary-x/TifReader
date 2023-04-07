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
    void mousePressEvent(QMouseEvent* event);          //�������
    void mouseMoveEvent(QMouseEvent* event);           //����ɿ�
    void mouseReleaseEvent(QMouseEvent* event);        //��귢���¼�
    void wheelEvent(QWheelEvent* event);               //�����ֹ���
    void contextMenuEvent(QContextMenuEvent* event);   //�Ҽ��˵�
    void mouseDoubleClickEvent(QMouseEvent* event);  //���˫���¼�
    void paintEvent(QPaintEvent* event);            //���Ʒ���
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
    QString m_strInputRaster = NULL;     //�ļ���
    GDALDataset* m_pDataset = NULL;
    int pX = 0, pY = 0;
    int nSizeX, nSizeY;
    int iSize;
    int iScaleWidth;
    int iScaleHeight;
    int nBandCount;
    QPoint OldPos;          //�ɵ����λ��
    bool Pressed = false;   //����Ƿ���ѹ
    double MoveSpeed;
    int mX = 0, mY = 0;
    int	band_list[3] = { 1,2,3 };
};
