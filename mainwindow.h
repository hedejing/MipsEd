#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QFileDialog>
#include <QTextStream>

#include "headers.h"
#include "file.h"
#include "memory.h"
#include "register.h"
#include "instruction.h"
#include "MipsSti.h"

namespace Ui {
class MainWindow;
}

using namespace std;
using namespace Mips;

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    bool on_actionNew_file_triggered();

    bool on_actionSave_file_triggered();

    bool on_actionSave_as_triggered();

    void on_actionOpen_file_triggered();

    void on_actionFind_triggered();

    void on_actionAsm_triggered();

    void on_actionDisasm_triggered();

    void on_actionRun_triggered();

    void on_actionCoe_triggered();

    void on_actionStep_triggered();

private:
    Ui::MainWindow *ui;

    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);
    bool isFirstTime;
    bool is_prepare;
	unsigned pc;
    QString curFile;
};

#endif // MAINWINDOW_H
