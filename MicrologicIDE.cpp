#include "aboutdialog.h"
#include "MicrologicIDE.h"
#include "ui_MicrologicIDE.h"
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QColorDialog>
#include <QFontDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QDateTime>
#include <QProcess>

MicrologicIDE::MicrologicIDE(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MicrologicIDE)
{
    ui->setupUi(this);
    ui->textEdit->setDocumentTitle("untitled -- notepad");
    setWindowTitle(ui->textEdit->documentTitle());

    connect(ui->newFileAction, &QAction::triggered, this, &MicrologicIDE::newFile);
    connect(ui->openFileAction, &QAction::triggered, this, &MicrologicIDE::openFile);
    connect(ui->saveFileAction, &QAction::triggered, this, &MicrologicIDE::saveFile);
    connect(ui->exitNotepadAction, &QAction::triggered, this, &MicrologicIDE::exitNotepad);
    connect(ui->saveAsFileAction, &QAction::triggered, this, &MicrologicIDE::saveAsFile);
    connect(ui->aboutAction, &QAction::triggered, this, &MicrologicIDE::doForAboutAction);

    connect(ui->getHelpAction, &QAction::triggered, this, &MicrologicIDE::seeHelp);
    connect(ui->dateAction, &QAction::triggered, this, &MicrologicIDE::setDateTime);

    connect(ui->copyAction, &QAction::triggered, ui->textEdit, &QTextEdit::copy);
    connect(ui->cutAction, &QAction::triggered, ui->textEdit, &QTextEdit::cut);
    connect(ui->pasteAction, &QAction::triggered, ui->textEdit, &QTextEdit::paste);
    connect(ui->undoAction, &QAction::triggered, ui->textEdit, &QTextEdit::undo);
    connect(ui->selectAllAction, &QAction::triggered, ui->textEdit, &QTextEdit::selectAll);

    connect(ui->runAction, &QAction::triggered, this, &MicrologicIDE::runFile);

}

MicrologicIDE::~MicrologicIDE()
{
    delete ui;
}

//新建文件
void MicrologicIDE::newFile(void)
{
    if (ui->textEdit->document()->isModified()) //编辑区内容是否改变
    {
        qDebug() << "document is modified";
        switch (requestIsSave())               //是否要保存
        {
            case QMessageBox::Yes :            //保存
                saveDocumentText();

                if (!currentfileName.isEmpty())
                {
                    currentfileName.clear();
                }

                setDocumentTitile("untitled -- notepad");
                ui->textEdit->document()->clear();
                break;
            case QMessageBox::No :
                setDocumentTitile("untitled -- notepad");
                ui->textEdit->document()->clear();
                break;
            case QMessageBox::Cancel :
                qDebug() << "取消";
                break;
            default:
                break;
        }
    }
    else
    {
        qDebug() << "document is not modified";
    }
}

void MicrologicIDE::openFile(void)
{
    if (ui->textEdit->document()->isModified()) //编辑区内容是否改变
    {
        saveDocumentText();
    }
    getDocumentText();
    setDocumentTitile(currentfileName);
}

void MicrologicIDE::saveFile(void)
{
    if (ui->textEdit->document()->isModified() || !currentfileName.isEmpty())
    {
        QString tmpFileName = saveDocumentText();
        setDocumentTitile(tmpFileName);
        currentfileName = tmpFileName;
    }
}

void MicrologicIDE::saveAsFile()
{
    if (ui->textEdit->document()->isModified())
    {
        QString tmpFileName = currentfileName;
        currentfileName.clear();
        saveDocumentText();
        currentfileName = tmpFileName;
    }
}


//退出
void MicrologicIDE::exitNotepad()
{
    if (ui->textEdit->document()->isModified())
    {
        switch (requestIsSave())
        {
            case QMessageBox::Yes :
                saveDocumentText();
                this->close();
                break;
            case QMessageBox::No :
                this->close();
                break;
            case QMessageBox::Cancel :
                qDebug() << "取消";
                break;
            default:
                break;
        }
    }
}

void MicrologicIDE::doForAboutAction()
{
//    QMessageBox::about(this, tr("关于"), tr("简单的记事本工具 - write by Ares"));
    AboutDialog about;
    about.exec();
}

void MicrologicIDE::setDateTime()
{
    ui->textEdit->append(QDateTime::currentDateTime().toString());
    //ui->textEdit->append("<p style=\"text-decoration: underline; text-decoration-color: red;\">111</p>");
    //qDebug()<<ui->textEdit->document()->toPlainText();
}

void MicrologicIDE::seeHelp(void)
{
    QUrl url("https://github.com/HZZcode/MicrologicIDE");
    QDesktopServices::openUrl(url);
}


QString MicrologicIDE::saveDocumentText(void)
{
    QString fileName;
    QFile file;
    QTextStream *out;

    if (currentfileName.isEmpty())   //如果当前没有打开的文件，就将内容保存新建的文件中，否则保存到当前打开的文件中
    {
        fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), QDir::currentPath(), tr("文本(*.txt)"));
    }
    else
    {
        fileName = currentfileName;
    }
//    if (fileName.isEmpty())
//        return fileName;

    file.setFileName(fileName);
    out = new QTextStream(&file);
    if (file.open(QIODevice::WriteOnly))   //只写模式打开
    {
        *out << ui->textEdit->document()->toPlainText(); //将textEdit编辑区域内容写入文件中
        file.close();
    }

    return fileName;
}

void MicrologicIDE::getDocumentText()
{
    QString fileName;
    QFile file;
    QTextStream *in;

    fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), QDir::currentPath(), tr("Micrologic文件(*.mcl)"));
    currentfileName = fileName;
    file.setFileName(fileName);
    in = new QTextStream(&file);
    if (file.open(QIODevice::ReadOnly))   //只写模式打开
    {
        ui->textEdit->setText(in->readAll());
        file.close();
    }
//    else
//    {
//        QMessageBox::warning(this, tr("提示"), file.errorString());
//    }
}

int MicrologicIDE::requestIsSave()
{
    int answer =  QMessageBox::question(this, tr("notepad"), tr("是否将更改保存？"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    return answer;
}


void MicrologicIDE::setDocumentTitile(QString title)
{
    ui->textEdit->setDocumentTitle(title);
    setWindowTitle(ui->textEdit->documentTitle());
}

void MicrologicIDE::runFile()
{
    QByteArray b=("start ./Micrologic.exe "+currentfileName).toLatin1();
    system(b.data());
}

