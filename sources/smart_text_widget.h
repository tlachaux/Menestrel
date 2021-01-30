#ifndef SMARTTEXTWIDGET_H
#define SMARTTEXTWIDGET_H

#include "text_widget.h"

class SmartTextWidget : public TextWidget
{
public:
    SmartTextWidget     (QFont font, QString text="");

    void updateErrors   ();
    void checkOrthograph();
};

#endif // SMARTTEXTWIDGET_H
