/*  SPDX-License-Identifier: MIT

    SPDX-FileCopyrightText: 2021 Waqar Ahmed <waqar.17a@gmail.com>

    SPDX-License-Identifier: MIT
*/
#ifndef BUILTOUTPUTHIGHLIGHTER_H
#define BUILTOUTPUTHIGHLIGHTER_H

#include <QSyntaxHighlighter>

namespace KTextEditor
{
class Editor;
}

class BuildOutputHighlighter : public QSyntaxHighlighter
{
public:
    BuildOutputHighlighter(QTextDocument* doc = nullptr);

    void updateColors(KTextEditor::Editor* editor);

protected:
    void highlightBlock(const QString &text) override;

private:
    QTextCharFormat fmt1;
    QTextCharFormat fmt2;
    QTextCharFormat warn;
    QTextCharFormat error;
};

#endif // BUILTOUTPUTHIGHLIGHTER_H
