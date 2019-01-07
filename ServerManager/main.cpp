#include "dialog.h"
#include <QApplication>
#include "sql.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    /*
    if(Sql::OpenDatabase() == 0)
        return 1;
    */
    w.show();

    return a.exec();
}
