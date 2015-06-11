#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isFirstTime = true;
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    is_prepare = false;
	pc = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::maybeSave()
{
    if (ui->plainTextEdit->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("MipsEdition"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return on_actionSave_file_triggered();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    ui->plainTextEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}

bool MainWindow::on_actionNew_file_triggered()
{
    if (maybeSave()) {
        ui->plainTextEdit->clear();
        setCurrentFile("");
    }

    return true;
}

bool MainWindow::on_actionSave_file_triggered()
{
    if (curFile.isEmpty()) {
        return on_actionSave_as_triggered();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::on_actionSave_as_triggered()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList files;
    if (dialog.exec())
        files = dialog.selectedFiles();
    else
        return false;

    return saveFile(files.at(0));
}


bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MipsEditor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
}
    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << ui->plainTextEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::on_actionOpen_file_triggered()
{
    QString selectedFliter;
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,"Open file",0,"asm file(*.asm);;coe file(*.coe);;binary file(*.bin)",&selectedFliter,0);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

void MainWindow::loadFile(const QString &fileName)
//! [42] //! [43]
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MipsEditor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    ui->plainTextEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainWindow::on_actionFind_triggered()
{
    QString searchString = ui->lineEdit->text();
    QTextDocument *document = ui->plainTextEdit->document();

    bool found = false;

    if (isFirstTime == false)
        document->undo();

    if (searchString.isEmpty()) {
        QMessageBox::information(this, tr("Empty Search Field"),
                "The search field is empty. Please enter a word and click Find.");
    } else {

        QTextCursor highlightCursor(document);
        QTextCursor cursor(document);

        cursor.beginEditBlock();
        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setForeground(Qt::red);

        while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
            highlightCursor = document->find(searchString, highlightCursor, QTextDocument::FindWholeWords);

            if (!highlightCursor.isNull()) {
                found = true;
                highlightCursor.movePosition(QTextCursor::WordRight,
                                       QTextCursor::KeepAnchor);
                highlightCursor.mergeCharFormat(colorFormat);
            }
        }

        cursor.endEditBlock();

        isFirstTime = false;

        if (found == false) {
            QMessageBox::information(this, tr("Word Not Found"),
                "Sorry, the word cannot be found.");
        }
    }
}

void MainWindow::on_actionAsm_triggered()
{
    if(ui->plainTextEdit->document()->isEmpty())
        return;
    on_actionSave_file_triggered();

    MipsSti MipsMachine;
    std::string path = curFile.toStdString();
    std::ifstream input(path);

    MipsMachine.run(input);
    MipsMachine.binary("binaryfile.bin");
    QMessageBox::about(NULL,"ASM succeed","Generate a binary file named \"binaryfile.bin\". ");
}

void MainWindow::on_actionDisasm_triggered()
{
    QString selectedFliter;
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,"Open file",0,"binary file(*.bin);;coe file(*.coe)",&selectedFliter,0);
        if (!fileName.isEmpty())
        {
            string file = fileName.toStdString();
            auto sz = file.size();
            string type = file.substr(sz-3,3);
            ifstream in(file);
            ofstream out("asmfile.asm");
            if(type == "bin")
            {
                Mips::bin2asm(in,out);
                QMessageBox::about(NULL, "DISASM succeed", "Generate a assembly file named \"asmfile.asm\" from a binary file. ");
            }
            else if(type == "coe")
            {
                Mips::coe2asm(in,out);
                QMessageBox::about(NULL, "DISASM succeed", "Generate a assembly file named \"asmfile.asm\" from a coe file. ");
            }

        }
        else
        {
            QMessageBox::warning(this, tr("MipsEditor"),
                                 tr("File name si empty %1:\n%2.")
                                 );
            return;
        }
    }

}

void MainWindow::on_actionRun_triggered()
{
    if(ui->plainTextEdit->document()->isEmpty())
        return;
	ui->textBrowser->clear();
    on_actionSave_file_triggered();
    MipsSti MipsMachine;
    std::map<string, bitset<32>> regs;
    std::string path = curFile.toStdString();
    std::ifstream input(path);
    regs = MipsMachine.run(input);
    int cnt=0;
    string str;
    QString qstr;
    for (auto i : regs)
    {
        std::string temp;
        std::stringstream ss;
        ss << i.first << ":" << std::hex<< i.second.to_ulong();
        ss >> temp;

        if(cnt%4==0)
            str = temp;
        else
            str = str + "\t" + temp;

        if(cnt%4==3)
        {
            qstr = QString::fromStdString(str);
            ui->textBrowser->append(qstr);
        }
        cnt++;
    }
    map<unsigned,string> hex_code = MipsMachine.get_allo_text();
    unsigned i=0;
    for(auto line:hex_code)
    {
        unsigned address = line.first;
        string addr_str = Mips::dec2hex8(address);
        QString addr_qstr = QString::fromStdString(addr_str);
        string code = line.second;
        QString code1 = QString::fromStdString(code.substr(0,2));
        QString code2 = QString::fromStdString(code.substr(2,2));
        QString code3 = QString::fromStdString(code.substr(4,2));
        QString code4 = QString::fromStdString(code.substr(6,2));
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(addr_qstr));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(code1));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(code2));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(code3));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(code4));
        i++;
    }
}

void MainWindow::on_actionCoe_triggered()
{
    if(ui->plainTextEdit->document()->isEmpty())
        return;
	on_actionSave_file_triggered();

	MipsSti MipsMachine;
	std::string path = curFile.toStdString();
	std::ifstream input(path);

	MipsMachine.run(input);
	MipsMachine.binary("binaryfile.bin");

    MipsMachine.coe("binaryfile.bin","coefile.coe");
    QMessageBox::about(NULL,"COE succeed","Generate coe file named \"coefile.coe\" from \"asmfile.asm\". ");
}

void MainWindow::on_actionStep_triggered()
{
    if(ui->plainTextEdit->document()->isEmpty())
        return;
    ui->textBrowser->clear();
    on_actionSave_file_triggered();
	MipsSti MipsMachine;
	std::map<string, bitset<32>> regs;

	std::string path = curFile.toStdString();
	std::ifstream input(path);
	MipsMachine.prepare(input);

	if (is_prepare == false)
	{
		is_prepare = true;
		pc = MipsMachine.text.allo_text.begin()->first;
	}

	if (MipsMachine.text.allo_text[pc].size() == 0)
	{
		is_prepare = false;
		MipsMachine.text.pc = MipsMachine.text.allo_text.begin()->first;
		QMessageBox::information(this, tr("Program ends"),
			"It has reach the last line of the program.");
		return;
	}

	string str;
	for (auto iter : MipsMachine.text.allo_text[pc])
		str = str + " " + iter;

	QString qstr;
	qstr = QString::fromStdString(str);
	ui->textBrowser->append(qstr);

	regs = MipsMachine.step(pc);

    int cnt=0;

    for (auto i : regs)
    {
        std::string temp;
        std::stringstream ss;
        ss << i.first << ":" << std::hex<< i.second.to_ulong();
        ss >> temp;

        if(cnt%4==0)
            str = temp;
        else
            str = str + "\t" + temp;

        if(cnt%4==3)
        {
            qstr = QString::fromStdString(str);
            ui->textBrowser->append(qstr);
        }
        cnt++;
    }
    map<unsigned,string> hex_code = MipsMachine.get_allo_text();
    unsigned i=0;
    for(auto line:hex_code)
    {
        unsigned address = line.first;
        string addr_str = Mips::dec2hex8(address);
        QString addr_qstr = QString::fromStdString(addr_str);
        string code = line.second;
        QString code1 = QString::fromStdString(code.substr(0,2));
        QString code2 = QString::fromStdString(code.substr(2,2));
        QString code3 = QString::fromStdString(code.substr(4,2));
        QString code4 = QString::fromStdString(code.substr(6,2));
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(addr_qstr));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(code1));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(code2));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(code3));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(code4));
        i++;
    }
}
