#ifndef MicrologicIDE_H
#define MicrologicIDE_H

#include <QMainWindow>

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

    void selectFont(void);
    void selectColor(void);

    void seeHelp(void);

    void setDateTime();

    QString saveDocumentText(void);
    void getDocumentText(void);
    int requestIsSave(void);

    void setDocumentTitile(QString title);

    void doForSelectChanged(void);

    void runFile(void);

private:
    Ui::MicrologicIDE *ui;
    QString currentfileName;
};

#endif // MicrologicIDE_H
