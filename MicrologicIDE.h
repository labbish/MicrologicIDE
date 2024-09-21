#ifndef MicrologicIDE_H
#define MicrologicIDE_H

#include <QMainWindow>

extern std::string exepath;

namespace Ui {
class MicrologicIDE;
}

class MicrologicIDE : public QMainWindow
{
    Q_OBJECT

public:
    std::string currentContent;
    std::string path;

    const std::string errorStart="<span style=\"text-decoration: underline; text-decoration-color: red; white-space: pre;\">",errorEnd="</span>";
    const std::string keyStart="<span style=\"color: orange; white-space: pre;\">",keyEnd="</span>";
    const std::string numStart="<span style=\"color: aqua; white-space: pre;\">",numEnd="</span>";
    const std::string modStart="<span style=\"color: purple; white-space: pre;\">",modEnd="</span>";

    const std::vector<std::string> blankChars={" ","\n",errorStart,errorEnd,"<br>"};
    const std::vector<std::string> keys={"end","line","wline","N","A","R","T","C","P","set","check","input:","input","output:","output","tick","tick!","speed","path","open","mod","block","inspect","echo","@echo","clear","help","clear"};
    const std::vector<std::string> nums={"0","1","2","3","4","5","6","7","8","9"};

    explicit MicrologicIDE(QWidget *parent = 0);
    ~MicrologicIDE();

    void newFile(void);
    void saveFile(void);
    void openFile(void);
    void saveAsFile(void);
    void exitNotepad(void);
    void doForAboutAction(void);

    void seeHelp(void);

    void setDateTime();

    QString saveDocumentText(void);
    void getDocumentText(void);
    int requestIsSave(void);

    void setDocumentTitile(QString title);

    void doForSelectChanged(void);

    void runFile(void);

    std::vector<std::string> content();
    void mark(std::vector<int>);
    void unmark();
    std::map<std::string,std::pair<int,int>> findMods(std::vector<std::string>);
    std::vector<bool> grammarCheck(std::vector<std::string>);
    void makeMarks();

    int charCount(int,int);

    inline bool isBlank(const std::string str){
        return count(blankChars.begin(),blankChars.end(),str)==1;
    }
    inline bool isNumber(const char c) {
        std::string s="";
        s.push_back(c);
        if (count(nums.begin(),nums.end(),s)==0) return false;
        return true;
    }
    inline bool isNumber(const std::string str) {
        if(str.length()>=4||str.length()==0) return false;
        for (char c : str) {
            std::string s="";
            s.push_back(c);
            if (count(nums.begin(),nums.end(),s)==0) return false;
        }
        return true;
    }
    inline unsigned int maxBlankLength(const std::string s){
        //return 10;
        int l=0;
        for(const std::string& b:blankChars){
            if(b.length()>l&&b.length()>=s.length()){
                if(b.substr(0,s.length())==s) l=b.length();
            }
        }
        return l;
    }

    std::vector<std::string> langs={"list","zh_cn","en_us"};

private slots:

    void on_textEdit_textChanged();

private:
    Ui::MicrologicIDE *ui;
    QString currentfileName;
};

#endif // MicrologicIDE_H
