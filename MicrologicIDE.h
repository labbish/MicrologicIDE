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
    void markError();

    int charCount(int,int);

    inline bool isNumber(const std::string str) {
        for (char c : str) {
            if (std::string("0123456789").find(c)==std::string::npos) return false;
        }
        return true;
    }

    std::vector<std::string> langs={"list","zh_cn","en_us"};

private slots:

    void on_textEdit_textChanged();

private:
    Ui::MicrologicIDE *ui;
    QString currentfileName;
};

#endif // MicrologicIDE_H
