#ifndef PARAMETER_SET_H
#define PARAMETER_SET_H

#include <QDialog>

namespace Ui {
class parameter_set;
}

class parameter_set : public QDialog
{
    Q_OBJECT

public:
    explicit parameter_set(QWidget *parent = nullptr);
    ~parameter_set();

private:
    Ui::parameter_set *ui;
};

#endif // PARAMETER_SET_H
