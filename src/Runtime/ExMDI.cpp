#include "ExMDI.h"
#include "ui_ExMDI.h"

#include <QTextStream>
#include <QFileInfo>
#include <QFontDialog>

ExMDI::ExMDI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExMDI)
{
    ui->setupUi(this);
}

ExMDI::~ExMDI()
{
    delete ui;
}

void ExMDI::loadFromFile(const QString& filename)
{
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        ui->plainTextEdit->clear();
        ui->plainTextEdit->setPlainText(stream.readAll());
        file.close();

        m_currentFile = filename;
        QFileInfo fileInfo(filename);
        setWindowTitle(fileInfo.fileName());
        m_fileOpened = true;
    }
}

bool ExMDI::isFileOpended()
{
    return m_fileOpened;
}

void ExMDI::setEditFont()
{
    QFont font = ui->plainTextEdit->font();
    bool ok;
    font = QFontDialog::getFont(&ok, font);
    ui->plainTextEdit->setFont(font);
}

void ExMDI::textCut()
{
    ui->plainTextEdit->cut();
}

void ExMDI::textCopy()
{
    ui->plainTextEdit->copy();
}

void ExMDI::textPaste()
{
    ui->plainTextEdit->paste();
}
