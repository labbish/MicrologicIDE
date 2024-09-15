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
    AboutDialog about;
    about.exec();
}

void MicrologicIDE::setDateTime()
{
    ui->textEdit->append(QDateTime::currentDateTime().toString());
    //markLine(0);
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
        fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), QDir::currentPath(), tr("Micrologic文件(*.mcl)"));
    }
    else
    {
        fileName = currentfileName;
    }

    file.setFileName(fileName);
    out = new QTextStream(&file);
    if (file.open(QIODevice::WriteOnly))   //只写模式打开
    {
        *out << ui->textEdit->document()->toPlainText(); //将textEdit编辑区域内容写入文件中
        file.close();
    }

    unmark();
    std::vector<int> errorList;
    for(int i=0;i<content().size();i++) if(!grammarCheck(content())[i]) errorList.push_back(i);
    markLine(errorList);

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

std::vector<std::string> MicrologicIDE::content(){
    std::string text=ui->textEdit->document()->toPlainText().toStdString();
    std::vector<std::string> lines;
    while(1){
        int t=text.find("\n");
        if(t!=std::string::npos){
            lines.push_back(text.substr(0,t));
            text=text.substr(t+1,text.size());
        }
        else{
            lines.push_back(text);
            break;
        }
    }
    return lines;
}

void MicrologicIDE::markLine(std::vector<int> errorList)
{
    std::string text;
    std::vector<std::string> lines=content();
    for(int k:errorList) if(lines.size()>=k+1) lines[k]="<span style=\"text-decoration: underline; text-decoration-color: red;\">"+lines[k]+"</span>";
    for(std::string& line:lines) text+=(line+"<br>");
    text=text.substr(0,text.length()-4);
    ui->textEdit->setText(text.c_str());
}

void MicrologicIDE::unmark()
{
    std::string text=ui->textEdit->document()->toPlainText().toStdString();
    ui->textEdit->setText(text.c_str());
}

std::vector<bool> MicrologicIDE::grammarCheck(std::vector<std::string> lines)
{
    std::vector<bool> ans(lines.size());
    std::vector<bool> Ls; //line order->is wide
    std::vector<bool> ins; //input line order->is wide
    std::vector<bool> outs; //output line order->is wide
    std::map<std::string,int> mods; //mod name->block count
    for(std::string s:{"N","A","R","T","C","P"}) mods.insert({s,0});
    for(int i=0;i<lines.size();i++){
        std::string line=lines[i];
        std::stringstream ss(line);
        std::string s;
        std::vector<std::string> args;
        while (std::getline(ss, s, ' ')) {
            args.push_back(s);
        }
        if(args.size()==0) ans[i]=true;
        else if(args[0]=="end"||args[0]=="clear"){
            ans[i]=args.size()==1;
        }
        else if((args[0]=="line"||args[0]=="wline")&&args.size()==1){
            ans[i]=true;
            Ls.push_back(args[0]=="wline");
        }
        else if((args[0]=="line"||args[0]=="wline")&&args.size()==2){
            ans[i]=isNumber(args[1]);
            if(isNumber(args[1])) for(int j=0;j<atoi(args[1].c_str());j++) Ls.push_back(args[0]=="wline");
        }
        else if((args[0]=="line"||args[0]=="wline")&&args.size()>=3){
            ans[i]=false;
        }
        else if((args[0]=="N"||args[0]=="T")&&args.size()==3){
            if(isNumber(args[1])&&isNumber(args[2])){
                if(atoi(args[1].c_str())<Ls.size()&&atoi(args[2].c_str())<Ls.size()&&Ls[atoi(args[1].c_str())]==0&&Ls[atoi(args[2].c_str())]==0){
                    ans[i]=true;
                    mods[args[0]]++;
                }
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if((args[0]=="A"||args[0]=="R")&&args.size()==4){
            if(isNumber(args[1])&&isNumber(args[2])&&isNumber(args[3])){
                if(atoi(args[1].c_str())<Ls.size()&&atoi(args[2].c_str())<Ls.size()&&atoi(args[3].c_str())<Ls.size()&&Ls[atoi(args[1].c_str())]==0&&Ls[atoi(args[2].c_str())]==0&&Ls[atoi(args[3].c_str())]==0){
                    ans[i]=true;
                    mods[args[0]]++;
                }
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if(args[0]=="C"&&args.size()==6){
            if(isNumber(args[1])&&isNumber(args[2])&&isNumber(args[3])&&isNumber(args[4])&&isNumber(args[5])){
                if(atoi(args[1].c_str())<Ls.size()&&atoi(args[2].c_str())<Ls.size()&&atoi(args[3].c_str())<Ls.size()&&atoi(args[4].c_str())<Ls.size()&&atoi(args[5].c_str())<Ls.size()&&Ls[atoi(args[1].c_str())]==0&&Ls[atoi(args[2].c_str())]==0&&Ls[atoi(args[3].c_str())]==0&&Ls[atoi(args[4].c_str())]==0&&Ls[atoi(args[5].c_str())]==1){
                    ans[i]=true;
                    mods[args[0]]++;
                }
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if(args[0]=="P"&&args.size()==6){
            if(isNumber(args[1])&&isNumber(args[2])&&isNumber(args[3])&&isNumber(args[4])&&isNumber(args[5])){
                if(atoi(args[1].c_str())<Ls.size()&&atoi(args[2].c_str())<Ls.size()&&atoi(args[3].c_str())<Ls.size()&&atoi(args[4].c_str())<Ls.size()&&atoi(args[5].c_str())<Ls.size()&&Ls[atoi(args[1].c_str())]==1&&Ls[atoi(args[2].c_str())]==0&&Ls[atoi(args[3].c_str())]==0&&Ls[atoi(args[4].c_str())]==0&&Ls[atoi(args[5].c_str())]==0){
                    ans[i]=true;
                    mods[args[0]]++;
                }
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if(args[0]=="set"&&args.size()==3){
            if(isNumber(args[1])&&isNumber(args[2])){
                if(atoi(args[1].c_str())<Ls.size()&&Ls[atoi(args[1].c_str())]==0&&(args[2]=="0"||args[2]=="1")) ans[i]=true;
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if(args[0]=="set"&&args.size()==6){
            if(isNumber(args[1])&&isNumber(args[2])&&isNumber(args[3])&&isNumber(args[4])&&isNumber(args[5])){
                if(atoi(args[1].c_str())<Ls.size()&&Ls[atoi(args[1].c_str())]==1&&(args[2]=="0"||args[2]=="1")&&(args[3]=="0"||args[3]=="1")&&(args[4]=="0"||args[4]=="1")&&(args[5]=="0"||args[5]=="1")) ans[i]=true;
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if(args[0]=="check"&&args.size()==1){
            ans[i]=true;
        }
        else if(args[0]=="check"&&args.size()==2){
            if(isNumber(args[1])){
                if(atoi(args[1].c_str())<Ls.size()) ans[i]=true;
                else ans[i]=false;
            }
        }
        else if((args[0]=="input:"||args[0]=="output:")&&args.size()==2){
            if(isNumber(args[1])){
                if(atoi(args[1].c_str())<Ls.size()){
                    ans[i]=true;
                    if(args[0]=="input:") ins.push_back(Ls[atoi(args[1].c_str())]);
                    if(args[0]=="output:") outs.push_back(Ls[atoi(args[1].c_str())]);
                }
                else ans[i]=false;
            }
        }
        else if(args[0]=="input"&&args.size()==3){
            if(isNumber(args[1])&&isNumber(args[2])){
                if(atoi(args[1].c_str())<ins.size()&&(args[2]=="0"||args[2]=="1")&&ins[atoi(args[1].c_str())]==0) ans[i]=true;
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if(args[0]=="input"&&args.size()==6){
            if(isNumber(args[1])&&isNumber(args[2])&&isNumber(args[3])&&isNumber(args[4])&&isNumber(args[5])){
                if(atoi(args[1].c_str())<ins.size()&&(args[2]=="0"||args[2]=="1")&&(args[3]=="0"||args[3]=="1")&&(args[4]=="0"||args[4]=="1")&&(args[5]=="0"||args[5]=="1")&&ins[atoi(args[1].c_str())]==1) ans[i]=true;
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if(args[0]=="output"&&args.size()==1){
            ans[i]=true;
        }
        else if(args[0]=="output"&&args.size()==2){
            if(isNumber(args[1])){
                if(atoi(args[1].c_str())<outs.size()) ans[i]=true;
                else ans[i]=false;
            }
        }
        else if((args[0]=="tick"||args[0]=="tick!")&&args.size()==1){
            ans[i]=true;
        }
        else if((args[0]=="tick"||args[0]=="tick!"||args[0]=="speed")&&args.size()==2){
            ans[i]=isNumber(args[1]);
        }
        else if(args[0]=="path"&&args.size()==2){
            ans[i]=(args[1][args[1].size()-1]=='\\');
        }
        else if(args[0]=="open"&&args.size()==2){
            ans[i]=true;
        }
        else if(args[0]=="mod"&&args.size()==3){
            mods.insert({args[1],0});
            ans[i]=true;
        }
        else if(args[0]=="block"&&args.size()>=3){
            if(mods.count(args[1])!=0){
                ans[i]=true;
                mods[args[1]]++;
            }
            else ans[i]=true;//false;
        }
        else if(args[0]=="inspect"&&args.size()==3){
            if(mods.count(args[1])!=0||args[1]=="N"||args[1]=="A"||args[1]=="R"||args[1]=="T"||args[1]=="C"||args[1]=="P"){
                if(isNumber(args[2])&&atoi(args[2].c_str())<mods[args[1]]) ans[i]=true;
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if(args[0]=="echo"){
            ans[i]=true;
        }
        else if(args[0]=="@echo"&&args.size()==2){
            if(isNumber(args[1])){
                if(args[1]=="0"||args[1]=="1") ans[i]=true;
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else ans[i]=false;
    }
    return ans;
}
