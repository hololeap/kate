/*  SPDX-License-Identifier: MIT

    SPDX-FileCopyrightText: 2021 Waqar Ahmed <waqar.17a@gmail.com>

    SPDX-License-Identifier: MIT
*/
#include "buildoutputhighlighter.h"

#include <KTextEditor/Editor>
#include <KSyntaxHighlighting/Theme>

using TextStyle = KSyntaxHighlighting::Theme::TextStyle;
using EditorColor = KSyntaxHighlighting::Theme::EditorColorRole;

BuildOutputHighlighter::BuildOutputHighlighter(QTextDocument *doc)
    : QSyntaxHighlighter(doc)
{
    updateColors(KTextEditor::Editor::instance());
}

void BuildOutputHighlighter::updateColors(KTextEditor::Editor *editor)
{
    if (!editor)
        return;

    auto theme = editor->theme();

    fmt1.setForeground(QColor::fromRgba(theme.textColor(TextStyle::Keyword)));
    fmt1.setFontWeight(300);
    fmt2.setForeground(QColor::fromRgba(theme.textColor(TextStyle::DataType)));
    fmt2.setFontWeight(300);
    warn.setForeground(QColor::fromRgba(theme.editorColor(EditorColor::MarkWarning)));
    error.setForeground(QColor::fromRgba(theme.editorColor(EditorColor::MarkError)));
}

void BuildOutputHighlighter::highlightBlock(const QString &text)
{
    if (text.contains(QLatin1String("Automatic MOC"))) {
        int idx = text.indexOf(QLatin1Char(']'));
        int start = idx > -1 ? idx + 1 : 0;
        setFormat(start, text.length(), fmt1);
        return;
    }

    if (text.contains(QLatin1String("Scanning dependencies of"))) {
        setFormat(0, text.length(), fmt2);
        return;
    }

    int warnPos = text.indexOf(QLatin1String("warning"));
    if (warnPos > -1) {
        setFormat(warnPos, text.length(), warn);
        return;
    }

    int errPos = text.indexOf(QLatin1String("error"));
    if (warnPos > -1) {
        setFormat(errPos, text.length(), warn);
        return;
    }
}
