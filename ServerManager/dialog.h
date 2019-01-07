#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "form.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Form *new_win;
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
    /* 鼠标拖动窗口 */
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:/*按钮*/
    void on_closeButton_clicked();
    void on_minButton_clicked();
    void OnConnectClientSlot(const int , const QString & ,const quint16 );
    void OnReadDataSlot(const int,const QString &, quint16, const QByteArray &);
    void OnsockDisConnectSlot(int ,QString ,quint16);


    void on_pushButton_clicked();

private:
    void Init();

private:
    Ui::Dialog *ui;

    /* 记录窗口当前的位置 */
    QPoint StartPos;
};

#endif // DIALOG_H
