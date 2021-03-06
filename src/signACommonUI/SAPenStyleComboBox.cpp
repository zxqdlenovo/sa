#include "SAPenStyleComboBox.h"

SAPenStyleComboBox::SAPenStyleComboBox(QWidget *parent):PenStyleBox(parent)
{
    connect(this,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged)
            ,this,&SAPenStyleComboBox::onComboBoxIndexChanged);
}

void SAPenStyleComboBox::onComboBoxIndexChanged(int index)
{
    emit penStyleChanged(style());
}
