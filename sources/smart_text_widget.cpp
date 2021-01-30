#include "smart_text_widget.h"
#include "orthograph_corrector.h"

SmartTextWidget::SmartTextWidget(QFont font, QString text)
    : TextWidget(font, text)
{

}

/**
 * @brief SmartTextWidget::updateErrors
 */
void SmartTextWidget::updateErrors()
{
    for (auto cursor : _underlined_cursors)
    {
        if (OrthographCorrector::singleton().doesWordExist(cursor))
        {
            this->cleanUnderlinedCursor(cursor);
        }
    }
}

/**
 * @brief SmartTextWidget::checkOrthograph
 */
void SmartTextWidget::checkOrthograph()
{
    if (this->toPlainText().isEmpty()) return;

    QRegExp separator("[(,|'|;| |.|/-)]");

    for (QString word : this->toPlainText().split(separator))
    {
        if (word.size() > 1 && word.back().isLetterOrNumber())
        {
            if (!OrthographCorrector::singleton().doesWordExist(word.toLower()))
            {
                this->underlineWord(word, QColor(163, 52, 27));
            }
        }
    }
}

