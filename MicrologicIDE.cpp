#include "aboutdialog.h"
#include "MicrologicIDE.h"
#include "ui_MicrologicIDE.h"
#include <fstream>
#include <iostream>
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
#include <QTextLayout>

std::string exepath="";

MicrologicIDE::MicrologicIDE(QWidget *parent,QString f) : QMainWindow(parent), ui(new Ui::MicrologicIDE){
    currentContent="";
    path="";

    ui->setupUi(this);
    ui->textEdit->setDocumentTitle("untitled -- MicrologicIDE");
    setWindowTitle(ui->textEdit->documentTitle());

    lightMode();

    debugger = new QFileSystemWatcher();
    QByteArray b=(QString::fromStdString(exepath+"\\debug.log")+currentfileName).toLatin1();
    debugger->addPath(b.data());

    connect(ui->newFileAction, &QAction::triggered, this, &MicrologicIDE::newFile);
    connect(ui->openFileAction, &QAction::triggered, this, &MicrologicIDE::openFile);
    connect(ui->saveFileAction, &QAction::triggered, this, &MicrologicIDE::saveFile);
    connect(ui->exitNotepadAction, &QAction::triggered, this, &MicrologicIDE::exitNotepad);
    connect(ui->saveAsFileAction, &QAction::triggered, this, &MicrologicIDE::saveAsFile);
    connect(ui->aboutAction, &QAction::triggered, this, &MicrologicIDE::doForAboutAction);

    connect(ui->getHelpAction, &QAction::triggered, this, &MicrologicIDE::seeHelp);
    connect(ui->dateAction, &QAction::triggered, this, &MicrologicIDE::setDateTime);
    connect(ui->refreshAction, &QAction::triggered, this, &MicrologicIDE::makeMarks);

    connect(ui->copyAction, &QAction::triggered, ui->textEdit, &QTextEdit::copy);
    connect(ui->cutAction, &QAction::triggered, ui->textEdit, &QTextEdit::cut);
    connect(ui->pasteAction, &QAction::triggered, ui->textEdit, &QTextEdit::paste);
    connect(ui->undoAction, &QAction::triggered, ui->textEdit, &QTextEdit::undo);
    connect(ui->selectAllAction, &QAction::triggered, ui->textEdit, &QTextEdit::selectAll);

    connect(ui->darkModeAction, &QAction::triggered, this, &MicrologicIDE::darkMode);
    connect(ui->lightModeAction, &QAction::triggered, this, &MicrologicIDE::lightMode);
    connect(ui->safeModeAction, &QAction::triggered, this, &MicrologicIDE::safeMode);

    connect(ui->runAction, &QAction::triggered, this, &MicrologicIDE::runFile);
    connect(debugger, SIGNAL(fileChanged(QString)), this, SLOT(updateDebug()));

    connect(ui->textEdit, &QTextEdit::textChanged, this, &MicrologicIDE::makeNewMarks);

    if(f!="") getDocumentText(f);
}

MicrologicIDE::~MicrologicIDE(){
    delete ui;
}

void MicrologicIDE::newFile(void){
    switch (requestIsSave())
    {
    case QMessageBox::Yes :
        saveDocumentText();

        if (!currentfileName.isEmpty())
        {
            currentfileName.clear();
        }

        setDocumentTitile("untitled -- MicrologicIDE");
        ui->textEdit->document()->clear();
        break;
    case QMessageBox::No :
        setDocumentTitile("untitled -- MicrologicIDE");
        ui->textEdit->document()->clear();
        break;
    case QMessageBox::Cancel :
        break;
    default:
        break;
    }
}

void MicrologicIDE::openFile(void){
    //saveDocumentText();
    getDocumentText();
    setDocumentTitile(currentfileName);
    makeMarks();
}

void MicrologicIDE::saveFile(void){
    QString tmpFileName = saveDocumentText();
    setDocumentTitile(tmpFileName);
    currentfileName = tmpFileName;
}

void MicrologicIDE::saveAsFile(){
    QString tmpFileName = currentfileName;
    currentfileName.clear();
    saveDocumentText();
    currentfileName = tmpFileName;
}

void MicrologicIDE::exitNotepad(){
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

void MicrologicIDE::doForAboutAction(){
    AboutDialog about;
    about.exec();
}

void MicrologicIDE::setDateTime()
{
    ui->textEdit->append(QDateTime::currentDateTime().toString());
}

void MicrologicIDE::seeHelp(void){
    QUrl url("https://github.com/HZZcode/MicrologicIDE");
    QDesktopServices::openUrl(url);
}


QString MicrologicIDE::saveDocumentText(void){
    QString fileName;
    QFile file;
    QTextStream *out;

    if (currentfileName.isEmpty())
    {
        fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), QDir::currentPath(), tr("Micrologic文件(*.mcl)"));
    }
    else
    {
        fileName = currentfileName;
    }

    file.setFileName(fileName);
    out = new QTextStream(&file);
    if (file.open(QIODevice::WriteOnly))
    {
        *out << ui->textEdit->document()->toPlainText();
        file.close();
    }

    return fileName;
}

void MicrologicIDE::getDocumentText(QString f){
    QString fileName;
    QFile file;
    QTextStream *in;

    if(f!="") fileName=f;
    else fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), QDir::currentPath(), tr("Micrologic文件(*.mcl)"));
    currentfileName = fileName;
    file.setFileName(fileName);
    in = new QTextStream(&file);
    if (file.open(QIODevice::ReadOnly))
    {
        ui->textEdit->setText(in->readAll());
        file.close();
    }
    try{
        int p1=currentfileName.toStdString().rfind("/"),p2=currentfileName.toStdString().rfind("\\");
        if(p1!=std::string::npos&&p1>p2) path=currentfileName.toStdString().substr(0,p1+1);
        else path=currentfileName.toStdString().substr(0,p2+1);
        path.replace(path.size()-1,1,"\\");
    }catch(...){}
    qDebug()<<fileName<<path;
    makeMarks();
}

int MicrologicIDE::requestIsSave(){
    int answer = QMessageBox::question(this, tr("MicrologicIDE"), tr("是否将更改保存？"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    return answer;
}


void MicrologicIDE::setDocumentTitile(QString title){
    ui->textEdit->setDocumentTitle(title);
    setWindowTitle(ui->textEdit->documentTitle());
}

void MicrologicIDE::runFile(){
    qDebug()<<currentfileName;
    QByteArray b=(QString::fromStdString("start /d \""+exepath+"\" Micrologic.exe ")+currentfileName).toLatin1();
    system(b.data());
}

std::vector<std::string> MicrologicIDE::content(){
    std::string text=ui->textEdit->document()->toPlainText().toStdString();
    std::vector<std::string> lines{};
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

void MicrologicIDE::mark(std::vector<int> errorList={}){
    std::string text{};
    std::vector<std::string> lines=content();

    //error
    for(int k:errorList) if(lines.size()>=k+1) lines[k]=errorStart+lines[k]+errorEnd;
    for(std::string& line:lines) text+=(line+"<br>");
    text=text.substr(0,text.length()-4);

    //keyword
    for(const std::string& key:keys){
        if(safe&&std::count(unsafeKeys.begin(),unsafeKeys.end(),key)) continue;
        int t{};
        for(const std::string& x:blankChars)
            for(const std::string& y:blankChars)
                while((t=text.find(x+key+y))!=std::string::npos){
                    text.replace(t,x.size()+key.size()+y.size(),x+keyStart+key+keyEnd+y);
                }
        if(text.length()>=key.length()){
            if(text.substr(0,key.size())==key)
                text.replace(0,key.size(),keyStart+key+keyEnd);
            if(text.substr(text.size()-key.size(),text.size())==key)
                text.replace(text.size()-key.size(),key.size(),keyStart+key+keyEnd);
        }
    }

    //unsafe keyword
    if(safe){
        for(const std::string& unsafeKey:unsafeKeys){
            int t{};
            for(const std::string& x:blankChars)
                for(const std::string& y:blankChars)
                    while((t=text.find(x+unsafeKey+y))!=std::string::npos){
                        text.replace(t,x.size()+unsafeKey.size()+y.size(),x+unsafeKeyStart+unsafeKey+unsafeKeyEnd+y);
                    }
            if(text.length()>=unsafeKey.length()){
                if(text.substr(0,unsafeKey.size())==unsafeKey)
                    text.replace(0,unsafeKey.size(),unsafeKeyStart+unsafeKey+unsafeKeyEnd);
                if(text.substr(text.size()-unsafeKey.size(),text.size())==unsafeKey)
                    text.replace(text.size()-unsafeKey.size(),unsafeKey.size(),unsafeKeyStart+unsafeKey+unsafeKeyEnd);
            }
        }
    }

    //mod name
    for(const std::pair<std::string,std::pair<int,int>>& m:findMods(lines)){
        std::string mod=m.first;
        int t{};
        for(const std::string& x:blankChars)
            for(const std::string& y:blankChars)
                while((t=text.find(x+mod+y))!=std::string::npos){
                    text.replace(t,x.size()+mod.size()+y.size(),x+modStart+mod+modEnd+y);
                }
        if(text.substr(0,mod.size())==mod)
            text.replace(0,mod.size(),modStart+mod+modEnd);
        if(text.substr(text.size()-mod.size(),text.size())==mod)
            text.replace(text.size()-mod.size(),mod.size(),modStart+mod+modEnd);
    }

    //number
    std::vector<std::string> segments{};
    for(int i=0;i<text.size();){
        bool found=false;
        for(int j=0;i+j<=text.size();j++){
            if(isBlank(text.substr(i,j))){
                segments.push_back(text.substr(i,j));
                found=true;
                i+=j;
                break;
            }
            if(j>maxBlankLength(text.substr(i,j))) break;
        }
        if(!found){
            segments.push_back(text.substr(i,1));
            i++;
        }
    }
    std::vector<std::string> words{};
    for(int i=0;i<segments.size();i++){
        if(i==0) words.push_back(segments[i]);
        else{
            if(isNumber(segments[i])&&isNumber(segments[i-1])) words[words.size()-1]+=segments[i];
            else words.push_back(segments[i]);
        }
    }
    text.clear();
    for(int i=0;i<words.size();i++){
        bool mark=false;
        if(i==0&&i==words.size()-1){ if(isNumber(words[i])) mark=true;}
        else if(i==0){ if(isNumber(words[i])&&isBlank(words[i+1])) mark=true;}
        else if(i==words.size()-1){ if(isBlank(words[i-1])&&isNumber(words[i])) mark=true;}
        else{if(isBlank(words[i-1])&&isNumber(words[i])&&isBlank(words[i+1])) mark=true;}
        if(mark) words[i]=numStart+words[i]+numEnd;
        text+=words[i];
    }

    ui->textEdit->setText(text.c_str());
}

void MicrologicIDE::unmark(){
    std::string text=ui->textEdit->document()->toPlainText().toStdString();
    ui->textEdit->setText(text.c_str());
}

void MicrologicIDE::makeNewMarks(){
    if(currentContent!=ui->textEdit->document()->toPlainText().toStdString()){
        makeMarks();
    }
}

void MicrologicIDE::makeMarks(){
    currentContent=ui->textEdit->document()->toPlainText().toStdString();
    QTextCursor cursor=ui->textEdit->textCursor();
    int pos=cursor.position();
    unmark();
    std::vector<int> errorList;
    auto grammar=grammarCheck(content());
    for(int i=0;i<content().size();i++) if(!grammar[i]) errorList.push_back(i);
    mark(errorList);
    cursor=ui->textEdit->textCursor();
    cursor.setPosition(pos);
    ui->textEdit->setTextCursor(cursor);
}

void MicrologicIDE::lightMode(){
    this->setStyleSheet("background-color: white; color: black;");
    ui->textEdit->setStyleSheet("border-top-style: solid; border-bottom-style: solid; border-width: 1px; border-color: black;");
    ui->debugText->setStyleSheet("border-bottom-style: solid; border-width: 1px; border-color: black;");

    errorStart="<span style=\"text-decoration: underline; text-decoration-color: red; white-space: pre;\">";errorEnd="</span>";
    keyStart="<span style=\"color: orange; white-space: pre;\">";keyEnd="</span>";
    unsafeKeyStart="<span style=\"color: red; white-space: pre;\">";unsafeKeyEnd="</span>";
    numStart="<span style=\"color: aqua; white-space: pre;\">";numEnd="</span>";
    modStart="<span style=\"color: purple; white-space: pre;\">";modEnd="</span>";

    debugStart="<span style=\"white-space: pre;\">";debugEnd="</span>";
    IStart="<span style=\"color: red; white-space: pre;\">";IEnd="</span>";
    OStart="<span style=\"color: blue; white-space: pre;\">";OEnd="</span>";
    IOStart="<span style=\"color: purple; white-space: pre;\">";IOEnd="</span>";
    debugInfo="====[DEBUG INFO] "+IStart+"input "+IEnd+OStart+"output "+OEnd+IOStart+"input&output "+IOEnd+"====";

    blankChars={" ","\n",errorStart,errorEnd,"<br>"};
}

void MicrologicIDE::darkMode(){
    this->setStyleSheet("background-color: black; color: white;");
    ui->textEdit->setStyleSheet("border-top-style: solid; border-bottom-style: solid; border-width: 1px; border-color: white;");
    ui->debugText->setStyleSheet("border-bottom-style: solid; border-width: 1px; border-color: white;");

    errorStart="<span style=\"text-decoration: underline; text-decoration-color: red; white-space: pre;\">";errorEnd="</span>";
    keyStart="<span style=\"color: orange; white-space: pre;\">";keyEnd="</span>";
    unsafeKeyStart="<span style=\"color: red; white-space: pre;\">";unsafeKeyEnd="</span>";
    numStart="<span style=\"color: aqua; white-space: pre;\">";numEnd="</span>";
    modStart="<span style=\"color: purple; white-space: pre;\">";modEnd="</span>";

    debugStart="<span style=\"white-space: pre;\">";debugEnd="</span>";
    IStart="<span style=\"color: red; white-space: pre;\">";IEnd="</span>";
    OStart="<span style=\"color: blue; white-space: pre;\">";OEnd="</span>";
    IOStart="<span style=\"color: purple; white-space: pre;\">";IOEnd="</span>";
    debugInfo="====[DEBUG INFO] "+IStart+"input "+IEnd+OStart+"output "+OEnd+IOStart+"input&output "+IOEnd+"====";

    blankChars={" ","\n",errorStart,errorEnd,"<br>"};
}

void MicrologicIDE::safeMode(){
    safe=!safe;
    makeMarks();
    QAction *action = findChild<QAction*>("safeModeAction");
    if(!action) return;
    action->setText(safe?"安全模式-已开启":"安全模式-未开启");
}

int countInput(std::vector<std::string> lines){
    int ans=0;
    for(int i=0;i<lines.size();i++){
        std::string line=lines[i];
        std::stringstream ss(line);
        std::string s{};
        std::vector<std::string> args{};
        while (std::getline(ss, s, ' ')) {
            args.push_back(s);
        }
        if(args.size()>0) if(args[0]=="input:") ans++;
    }
    return ans;
}

int countOutput(std::vector<std::string> lines){
    int ans=0;
    for(int i=0;i<lines.size();i++){
        std::string line=lines[i];
        std::stringstream ss(line);
        std::string s{};
        std::vector<std::string> args{};
        while (std::getline(ss, s, ' ')) {
            args.push_back(s);
        }
        if(args.size()>0) if(args[0]=="output:") ans++;
    }
    return ans;
}

std::map<std::string,std::pair<int,int>> MicrologicIDE::findMods(std::vector<std::string> lines){
    std::map<std::string,std::pair<int,int>> mods{};
    for(int i=0;i<lines.size();i++){
        std::string line=lines[i];
        std::stringstream ss(line);
        std::string s{};
        std::vector<std::string> args{};
        while (std::getline(ss, s, ' ')) {
            args.push_back(s);
        }
        if(args.size()>0){
            if (args[0]=="path"&&count(line.begin(),line.end(),'\"')>=2) {
                int x=line.find("\""),y=line.rfind("\"");
                std::string f = line.substr(x+1,y-x-1);
                path=f;
            }
            else if (args[0]=="path"&&args.size()==2) {
                path=args[1];
            }
            else if(args[0]=="mod"&&args.size()==3){
                try{
                    std::vector<std::string> lines1{};
                    std::ifstream fin{};
                    std::string f=args[2];
                    fin.open(f,std::ios::out|std::ios::in);
                    bool b=fin.good();
                    std::string nextPath=f.substr(0,f.rfind("\\")+1);
                    if (!fin.good()){
                        fin.open(path+f,std::ios::out|std::ios::in);
                        nextPath=(path+f).substr(0,f.rfind("\\")+1);
                        b=b||fin.good();
                    }
                    int t;
                    while((t=nextPath.find("/"))!=std::string::npos) nextPath.replace(t,1,"\\");
                    if (nextPath!="") path=nextPath;
                    char fcmd[256]="";
                    while (fin.getline(fcmd, 256)) {
                        lines1.push_back(fcmd);
                    }
                    std::map<std::string,std::pair<int,int>> mods1=findMods(lines1);
                    if(b){
                        mods.insert({args[1],{countInput(lines1),countOutput(lines1)}});
                    }
                    mods.insert(mods1.begin(),mods1.end());
                }catch(...){}
            }
        }
    }
    return mods;
}

std::vector<bool> MicrologicIDE::grammarCheck(std::vector<std::string> lines){
    std::vector<bool> ans(lines.size());
    std::vector<bool> Ls{}; //line order->is wide
    std::vector<bool> ins{}; //input line order->is wide
    std::vector<bool> outs{}; //output line order->is wide
    std::map<std::string,int> modBlock{}; //mod name->block count
    std::map<std::string,std::pair<int,int>> mods=findMods(lines); //modname->{input count,output count}
    for(std::string s:{"N","A","R","T","C","P"}) modBlock.insert({s,0});
    for(int i=0;i<lines.size();i++){
        std::string line=lines[i];
        std::stringstream ss(line);
        std::string s{};
        std::vector<std::string> args{};
        while (std::getline(ss, s, ' ')) {
            args.push_back(s);
        }
        if(safe&&count(unsafeKeys.begin(),unsafeKeys.end(),args[0])){
            ans[i]=false;
        }
        else if(args.size()==0) ans[i]=true;
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
                    modBlock[args[0]]++;
                }
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if((args[0]=="A"||args[0]=="R")&&args.size()==4){
            if(isNumber(args[1])&&isNumber(args[2])&&isNumber(args[3])){
                if(atoi(args[1].c_str())<Ls.size()&&atoi(args[2].c_str())<Ls.size()&&atoi(args[3].c_str())<Ls.size()&&Ls[atoi(args[1].c_str())]==0&&Ls[atoi(args[2].c_str())]==0&&Ls[atoi(args[3].c_str())]==0){
                    ans[i]=true;
                    modBlock[args[0]]++;
                }
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if(args[0]=="C"&&args.size()==6){
            if(isNumber(args[1])&&isNumber(args[2])&&isNumber(args[3])&&isNumber(args[4])&&isNumber(args[5])){
                if(atoi(args[1].c_str())<Ls.size()&&atoi(args[2].c_str())<Ls.size()&&atoi(args[3].c_str())<Ls.size()&&atoi(args[4].c_str())<Ls.size()&&atoi(args[5].c_str())<Ls.size()&&Ls[atoi(args[1].c_str())]==0&&Ls[atoi(args[2].c_str())]==0&&Ls[atoi(args[3].c_str())]==0&&Ls[atoi(args[4].c_str())]==0&&Ls[atoi(args[5].c_str())]==1){
                    ans[i]=true;
                    modBlock[args[0]]++;
                }
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if(args[0]=="P"&&args.size()==6){
            if(isNumber(args[1])&&isNumber(args[2])&&isNumber(args[3])&&isNumber(args[4])&&isNumber(args[5])){
                if(atoi(args[1].c_str())<Ls.size()&&atoi(args[2].c_str())<Ls.size()&&atoi(args[3].c_str())<Ls.size()&&atoi(args[4].c_str())<Ls.size()&&atoi(args[5].c_str())<Ls.size()&&Ls[atoi(args[1].c_str())]==1&&Ls[atoi(args[2].c_str())]==0&&Ls[atoi(args[3].c_str())]==0&&Ls[atoi(args[4].c_str())]==0&&Ls[atoi(args[5].c_str())]==0){
                    ans[i]=true;
                    modBlock[args[0]]++;
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
            else ans[i]=false;
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
            else ans[i]=false;
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
            else ans[i]=false;
        }
        else if((args[0]=="tick"||args[0]=="tick!")&&args.size()==1){
            ans[i]=true;
        }
        else if((args[0]=="tick"||args[0]=="tick!"||args[0]=="speed")&&args.size()==2){
            ans[i]=isNumber(args[1]);
        }
        else if(args[0]=="path"&&count(line.begin(),line.end(),'\"')>=2){
            ans[i]=true;
        }
        else if(args[0]=="path"&&args.size()==2){
            ans[i]=true;
        }
        else if((args[0]=="open"||args[0]=="safe-open")&&args.size()==2){
            ans[i]=true;
        }
        else if((args[0]=="open"||args[0]=="safe-open")&&count(line.begin(),line.end(),'\"')>=2){
            ans[i]=true;
        }
        else if(args[0]=="mod"&&args.size()==3){
            modBlock.insert({args[1],0});
            ans[i]=true;
        }
        else if(args[0]=="check-mod"&&args.size()==1){
            ans[i]=true;
        }
        else if(args[0]=="block"&&args.size()>=3){
            if(mods.count(args[1])!=0){
                if(args.size()==2+mods[args[1]].first+mods[args[1]].second) ans[i]=true;
                else ans[i]=false;
                for(int j=2;j<args.size();j++){
                    if(!isNumber(args[j])) ans[i]=false;
                    if(atoi(args[j].c_str())>=Ls.size()) ans[i]=false;
                }
                modBlock[args[1]]++;
            }
            else ans[i]=false;
        }
        else if(args[0]=="block-type"&&args.size()==2){
            if(isNumber(args[1])){
                int blockCount=0;
                for(const std::pair<std::string,int>& p:modBlock){
                    if(p.first!="N"&&p.first!="A"&&p.first!="R"&&p.first!="T"&&p.first!="C"&&p.first!="P") blockCount+=p.second;
                }
                if(atoi(args[1].c_str())<blockCount) ans[i]=true;
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if((args[0]=="type"||args[0]=="tag")&&args.size()==2){
            if(isNumber(args[1])){
                if(atoi(args[1].c_str())<Ls.size()) ans[i]=true;
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if((args[0]=="check-input"||args[0]=="check-output")&&args.size()==1){
            ans[i]=true;
        }
        else if((args[0]=="check-input"||args[0]=="check-output")&&args.size()==2){
            if(isNumber(args[1])){
                if(args[0]=="check-input"&&atoi(args[1].c_str())<ins.size()) ans[i]=true;
                else if(args[0]=="check-output"&&atoi(args[1].c_str())<outs.size()) ans[i]=true;
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if(args[0]=="inspect"&&args.size()==3){
            if(mods.count(args[1])!=0||args[1]=="N"||args[1]=="A"||args[1]=="R"||args[1]=="T"||args[1]=="C"||args[1]=="P"){
                if(isNumber(args[2])&&atoi(args[2].c_str())<modBlock[args[1]]) ans[i]=true;
                else ans[i]=false;
            }
            else ans[i]=false;
        }
        else if(args[0]=="del"&&args.size()==3){
            if(args[1]=="line"){
                if(isNumber(args[2])&&atoi(args[2].c_str())<Ls.size()) ans[i]=true;
                else ans[i]=false;
            }
            else if(mods.count(args[1])!=0||args[1]=="N"||args[1]=="A"||args[1]=="R"||args[1]=="T"||args[1]=="C"||args[1]=="P"){
                if(isNumber(args[2])&&atoi(args[2].c_str())<modBlock[args[1]]) ans[i]=true;
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
        else if(args[0]=="lang"&&args.size()==2){
            ans[i]=std::count(langs.begin(),langs.end(),args[1])>0;
        }
        else if(args[0]=="neko"&&args.size()==1){
            ans[i]=true;
        }
        else ans[i]=false;
    }
    return ans;
}

void MicrologicIDE::on_textEdit_textChanged(){

}

void MicrologicIDE::updateDebug(){
    std::ifstream debug;
    debug.open(exepath+"\\debug.log",std::ios::in);
    if(!debug.good()){
        qDebug()<<"Error opening debug.log";
        return;
    }
    char text[256];
    debug.getline(text,256);

    std::stringstream ss(text);
    std::string s{};
    std::vector<std::string> infos{},nums{};
    while (std::getline(ss, s, ' ')) {
        infos.push_back(s);
    }
    std::string line1="",line2="";
    for(int i=0;i<infos.size();i++){
        nums.push_back(infos[i].substr(2,infos.size()));
        bool I=(infos[i][0]=='I'),O=(infos[i][1]=='O');
        std::string start="",end="";
        if(I&&O){
            start=IOStart;
            end=IOEnd;
        }
        else if(I){
            start=IStart;
            end=IEnd;
        }
        else if(O){
            start=OStart;
            end=OEnd;
        }

        if(nums[i].size()==1){
            line1+=(start+"[L"+std::to_string(i)+"]"+end+" ");
            if(std::to_string(i).length()==1) line2+=("  "+start+nums[i]+end+"  ");
            else line2+=("  "+start+nums[i]+end+"   ");
        }
        else{
            line1+=(start+"[W"+std::to_string(i)+"]"+end+" ");
            if(std::to_string(i).length()==1) line2+=(start+nums[i]+end+" ");
            else line2+=(" "+start+nums[i]+end+" ");
        }
    }
    line1=line1.substr(0,line1.length()-1);
    line2=line2.substr(0,line2.length()-1);
    ui->debugText->setText((debugInfo+"<br>"+debugStart+line1+"<br>"+line2+debugEnd).c_str());
}
