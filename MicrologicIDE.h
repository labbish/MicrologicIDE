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
    void markLine(std::vector<int>);
    void unmark();
    std::vector<bool> grammarCheck(std::vector<std::string>);

    inline bool isNumber(const std::string str) {
        for (char c : str) {
            if (std::string("0123456789").find(c)==std::string::npos) return false;
        }
        return true;
    }

private slots:

private:
    Ui::MicrologicIDE *ui;
    QString currentfileName;
};

#endif // MicrologicIDE_H
