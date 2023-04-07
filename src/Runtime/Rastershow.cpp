#include "rastershow.h"
#include <QMessageBox>
#include <QString>
#include "gdal_priv.h"
#include <QDebug>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMenu>
#include <QPainter>
#include <QObject>
#include "bandchoosedialog.h"
#include "ogrsf_frmts.h"
#include "ogr_spatialref.h"
#include <QTime>

RasterShow::RasterShow(QWidget* parent) :QLabel(parent)
{
    this->setMouseTracking(true);
}

RasterShow::~RasterShow()
{
    if (m_pDataset != NULL)
    {
        GDALClose((GDALDatasetH)m_pDataset);
        m_pDataset = NULL;
    }
}

bool RasterShow::ReadFile(QString filename)
{
    QTime time;
    time.start();
    GDALAllRegister();
    m_strInputRaster = filename;
    QByteArray ba = m_strInputRaster.toLatin1();
    m_pDataset = (GDALDataset*)GDALOpen(ba.data(), GA_ReadOnly);
    if (m_pDataset == NULL)
    {
        //QMessageBox::about(this,"��ʾ", "ָ�����ļ����ܴ򿪣�");
        return false;
    }

    nSizeX = m_pDataset->GetRasterXSize(); // Ӱ��Ŀ�ȣ���Ԫ��Ŀ��
    nSizeY = m_pDataset->GetRasterYSize(); // Ӱ��ĸ߶ȣ���Ԫ��Ŀ��

    qDebug() << nSizeX << " " << nSizeY;

    double m_dScale;//����ͼ����ͼ��ı�ֵ
    nBandCount = m_pDataset->GetRasterCount(); // Ӱ�񲨶���

    double adfGeoTransform[6];
    m_pDataset->GetGeoTransform(adfGeoTransform);
    MoveSpeed = 100 / adfGeoTransform[1];//���ݷֱ��������ٶȲ����ĳ�ʼֵ
    qDebug() << adfGeoTransform[1];

    m_dScale = nSizeY > nSizeX ? nSizeY : nSizeX;
    int iViewHeight = 800;
    m_dScale = iViewHeight / m_dScale;

    iSize = GDALGetDataTypeSize(GDT_Byte) / 8;
    iScaleWidth = static_cast<int>(nSizeX * m_dScale + 0.5);
    iScaleHeight = static_cast<int>(nSizeY * m_dScale + 0.5);

    iScaleWidth = (iScaleWidth * 8 + 31) / 32 * 4;

    double time1 = time.elapsed() / 1000.0;
    qDebug() << time1 << "s";

    int  ret = QMessageBox::question(this, "question", "�Ƿ񹹽���������", QMessageBox::No | QMessageBox::Yes);
    switch (ret) {
    case QMessageBox::Yes:
    {
        time.restart();

        int  anOverviewList[8] = { 2, 4, 8, 16, 32, 64, 128, 256 };
        m_pDataset->BuildOverviews("NEAREST", 7, anOverviewList, 0, nullptr, GDALDummyProgress, nullptr);    //���������

        double time1 = time.elapsed() / 1000.0;
        qDebug() << time1 << "jis";
        break;
    }
    case QMessageBox::No:
        break;
    default:
        break;
    }

    BandChooseDialog getbands;
    connect(&getbands, SIGNAL(signal_sendbands(QByteArray)), this, SLOT(GetBandList_slot(QByteArray)));
    getbands.setBandCount(nBandCount);
    getbands.show();
    getbands.exec();

    ShowRaster();
}

void RasterShow::ShowRaster()
{
    QTime time;
    time.start();

    unsigned char* pBuffer = new unsigned char[iScaleWidth * iScaleHeight * nBandCount];

    CPLErr err = m_pDataset->RasterIO(GF_Read, pX, pY, nSizeX, nSizeY, pBuffer, iScaleWidth, iScaleHeight,
        GDT_Byte, nBandCount, band_list, iSize * nBandCount, iSize * iScaleWidth * nBandCount, iSize);	//��ȡ���в�������

    unsigned char* pDataBuffer = NULL;
    if (nBandCount >= 3)
    {
        pDataBuffer = pBuffer;
    }
    else
    {
        pDataBuffer = new unsigned char[iScaleWidth * iScaleHeight * 3];
        for (int i = 0; i < iScaleWidth * iScaleHeight * 3; i++)
            pDataBuffer[i] = pBuffer[i / 3];

        delete[]pBuffer;
    }
    QImage QImg(pDataBuffer, iScaleWidth, iScaleHeight, QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage(QImg);
    delete[]pDataBuffer;

    this->setPixmap(pixmap);

    double time1 = time.elapsed() / 1000.0;
    qDebug() << time1 << "s";
}

void RasterShow::contextMenuEvent(QContextMenuEvent* event)
{
    QPoint pos = event->pos();
    pos = this->mapToGlobal(pos);
    QMenu* menu = new QMenu(this);

    QAction* loadImage = new QAction(this);
    loadImage->setText("ѡ��ͼƬ");
    connect(loadImage, &QAction::triggered, this, &RasterShow::ReadFile_Slot);
    menu->addAction(loadImage);

    QAction* closeData = new QAction(this);
    closeData->setText("�ر�����");
    connect(closeData, &QAction::triggered, this, &RasterShow::CloseData_slot);
    menu->addAction(closeData);

    menu->addSeparator();//��ӷָ���

    QAction* zoomInAction = new QAction(this);
    zoomInAction->setText("�Ŵ�");
    connect(zoomInAction, &QAction::triggered, this, &RasterShow::ZoomIn);
    menu->addAction(zoomInAction);

    QAction* zoomOutAction = new QAction(this);
    zoomOutAction->setText("��С");
    connect(zoomOutAction, &QAction::triggered, this, &RasterShow::ZoomOut);
    menu->addAction(zoomOutAction);

    QAction* presetAction = new QAction(this);
    presetAction->setText("��ԭ");
    connect(presetAction, &QAction::triggered, this, &RasterShow::RasterReset);
    menu->addAction(presetAction);

    menu->addSeparator();//��ӷָ���

    QAction* getPrjInfo = new QAction(this);
    getPrjInfo->setText("������Ϣ");
    connect(getPrjInfo, &QAction::triggered, this, &RasterShow::GetProjectionInfo);
    menu->addAction(getPrjInfo);

    menu->exec(pos);
}

void RasterShow::wheelEvent(QWheelEvent* event)
{
    int value = event->delta();
    if (value > 0)  //�Ŵ�
        ZoomIn();
    else            //��С
        ZoomOut();
}


void RasterShow::ZoomIn()
{
    if (m_pDataset != NULL)
    {
        pX = pX + nSizeX / 4;
        pY = pY + nSizeY / 4;
        nSizeX = nSizeX / 2;
        nSizeY = nSizeY / 2;
        MoveSpeed = MoveSpeed / 2;
        ShowRaster();
    }
}

void RasterShow::ZoomOut()
{
    if (m_pDataset != NULL)
    {
        pX = pX - nSizeX / 2;
        pY = pY - nSizeY / 2;
        nSizeX = nSizeX * 2;
        nSizeY = nSizeY * 2;
        MoveSpeed = MoveSpeed * 2;
        //�����С��Χ����ͼ����ص�ԭʼ��Сͼ��
        if (pX < 0)
        {
            RasterReset();
        }
        ShowRaster();
    }
}

//�������
void RasterShow::mousePressEvent(QMouseEvent* event)
{
    OldPos = event->pos();
    Pressed = true;
}

//����ɿ�
void RasterShow::mouseReleaseEvent(QMouseEvent* event)
{
    Pressed = false;
    setCursor(Qt::ArrowCursor);
}

//��귢���¼�
void RasterShow::mouseMoveEvent(QMouseEvent* event)
{
    if (m_pDataset != NULL)
    {
        if (Pressed == false) {
            const char* projRef = m_pDataset->GetProjectionRef();
            double adfGeoTransform[6];
            m_pDataset->GetGeoTransform(adfGeoTransform);

            double x, y;
            x = adfGeoTransform[0] + adfGeoTransform[1] * event->x() + adfGeoTransform[2] * event->y();
            y = adfGeoTransform[3] + adfGeoTransform[4] * event->x() + adfGeoTransform[5] * event->y();

            OGRSpatialReference fRef, tRef;
            char* tmp = NULL;
            /* ���projRef��һ�ݿ��� */
            /* ����projRef��const char*,�����һ�����������ܣ�������Ҫת���ɷ�const*/
            tmp = (char*)malloc(strlen(projRef) + 1);
            strcpy_s(tmp, strlen(projRef) + 1, projRef);
            /* ����ԭʼ�������������test.tifһ�� */
            fRef.importFromWkt(&tmp);
            /* ����ת��������� */
            tRef.SetWellKnownGeogCS("WGS84");
            /* �����������ת��������Ϊֹ������Ҫproj����������������������װproj�����޷����� */
            OGRCoordinateTransformation* coordTrans;
            coordTrans = OGRCreateCoordinateTransformation(&fRef, &tRef);

            if (coordTrans != NULL) {
                coordTrans->Transform(1, &x, &y);
                emit ProjSingnal(x, y, 1);
            }
            else {
                emit ProjSingnal(0, 0, 0);
                this->setMouseTracking(false);
            }
        }

        //ͼ�����ι���
        if (Pressed == true) {
            this->setCursor(Qt::SizeAllCursor);
            QPoint pos = event->pos();
            int xPtInterval = pos.x() - OldPos.x();
            int yPtInterval = pos.y() - OldPos.y();

            pX -= xPtInterval * MoveSpeed;
            pY -= yPtInterval * MoveSpeed;

            //����ƶ��Ƿ񵽴�ͼ��߽�
            if (pX < 0)
                pX = 0;
            if (pY < 0)
                pY = 0;
            if (pX > m_pDataset->GetRasterXSize() - nSizeX)
                pX = m_pDataset->GetRasterXSize() - nSizeX;
            if (pY > m_pDataset->GetRasterYSize() - nSizeY)
                pY = m_pDataset->GetRasterYSize() - nSizeY;

            OldPos = pos;
            ShowRaster();
        }
    }
}

void RasterShow::RasterReset()
{
    if (m_pDataset != NULL)
    {
        pX = 0;
        pY = 0;
        nSizeX = m_pDataset->GetRasterXSize();
        nSizeY = m_pDataset->GetRasterYSize();
        double adfGeoTransform[6];
        m_pDataset->GetGeoTransform(adfGeoTransform);
        MoveSpeed = 100 / adfGeoTransform[1];
        ShowRaster();
    }
}

int RasterShow::RasterShow::GetXsize()
{
    return nSizeX;
}

int RasterShow::RasterShow::GetYsize()
{
    return nSizeY;
}

//ͼ������ת��������
bool RasterShow::ImageRowCol2Projection()
{
    //adfGeoTransform[6]  ����adfGeoTransform������Ƿ���任�е�һЩ�������ֱ������
    //adfGeoTransform[0]  ���Ͻ�x����
    //adfGeoTransform[1]  ��������ֱ���
    //adfGeoTransform[2]  ��ת�Ƕ�, 0��ʾͼ�� "��������"
    //adfGeoTransform[3]  ���Ͻ�y����
    //adfGeoTransform[4]  ��ת�Ƕ�, 0��ʾͼ�� "��������"
    //adfGeoTransform[5]  �ϱ�����ֱ���
    double adfGeoTransform[6];
    m_pDataset->GetGeoTransform(adfGeoTransform);
    try
    {
        Xproj = adfGeoTransform[0] + adfGeoTransform[1] * pX + adfGeoTransform[2] * pY;
        Yproj = adfGeoTransform[3] + adfGeoTransform[4] * pX + adfGeoTransform[5] * pY;
        qDebug() << Xproj << " " << Yproj;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

//��������תͼ������
bool RasterShow::Projection2ImageRowCol(double Xpj, double Ypj, int Xsize, int Ysize)
{
    double adfGeoTransform[6];
    m_pDataset->GetGeoTransform(adfGeoTransform);
    try
    {
        double dTemp = adfGeoTransform[1] * adfGeoTransform[5] - adfGeoTransform[2] * adfGeoTransform[4];
        double dCol = 0.0, dRow = 0.0;
        dCol = (adfGeoTransform[5] * (Xpj - adfGeoTransform[0]) -
            adfGeoTransform[2] * (Ypj - adfGeoTransform[3])) / dTemp + 0.5;
        dRow = (adfGeoTransform[1] * (Ypj - adfGeoTransform[3]) -
            adfGeoTransform[4] * (Xpj - adfGeoTransform[0])) / dTemp + 0.5;

        pX = static_cast<int>(dCol);
        pY = static_cast<int>(dRow);
        nSizeX = Xsize;
        nSizeY = Ysize;

        ShowRaster();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void RasterShow::GetProjectionInfo()
{
    const char* projection;
    projection = m_pDataset->GetProjectionRef();

    QMessageBox::about(this, "������Ϣ", projection);
    qDebug() << projection;
}

void RasterShow::ReadFile_Slot()
{
    QString filename;
    if (filename != m_strInputRaster)
    {
        GDALClose((GDALDatasetH)m_pDataset);
        m_pDataset = NULL;
    }
    filename = QFileDialog::getOpenFileName(this, "Open Image", "./", tr("Images (*tif)"));
    ReadFile(filename);
}

//���˫��
void RasterShow::mouseDoubleClickEvent(QMouseEvent* event)
{
    int i = event->x();
    int j = event->y();
    mX = i - 30;
    mY = j - 30;
    update();
    emit doubleClickSignal(mX, mY);
}

//��ɫС����
void RasterShow::paintEvent(QPaintEvent* event)
{
    QLabel::paintEvent(event);
    QPainter painter(this);
    QPen pen;
    pen.setColor(QColor(255, 0, 0));//������ɫ
    pen.setWidth(2);//���ÿ��
    painter.setPen(pen);
    painter.drawRect(mX, mY, 60, 60);//�Զ���ʵ�ַ�������Ͻ�����ͳ���
}



void RasterShow::RedSquare_slot(int x, int y)
{
    mX = x;
    mY = y;
    update();
}

void RasterShow::ZoomIn_Slot()
{
    ZoomIn();
}

void RasterShow::ZoomOut_Slot()
{
    ZoomOut();
}

void RasterShow::ZoomReset_Slot()
{
    RasterReset();
}

void RasterShow::ImageRowCol2Projection_slot()
{
    ImageRowCol2Projection();
}

void RasterShow::GetProjectionInfo_slot()
{
    GetProjectionInfo();
}

void RasterShow::GetBandList_slot(QByteArray datagram)
{

    QDataStream in(&datagram, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_3);
    in >> band_list[0]
        >> band_list[1]
        >> band_list[2];
}

void RasterShow::CloseData_slot()
{
    if (m_pDataset != NULL)
    {
        GDALClose((GDALDatasetH)m_pDataset);
        m_pDataset = NULL;
        this->setText("�Ҽ��ɴ�ͼ��");
    }

}
