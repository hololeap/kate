/*
    SPDX-FileCopyrightText: 2018 Sven Brauch <mail@svenbrauch.de>
    SPDX-FileCopyrightText: 2018 Michal Srb <michalsrb@gmail.com>
    SPDX-FileCopyrightText: 2020 Jan Paul Batrina <jpmbatrina01@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KATE_COLORPICKER_H
#define KATE_COLORPICKER_H

#include <KTextEditor/ConfigPage>
#include <KTextEditor/InlineNoteProvider>
#include <KTextEditor/MainWindow>
#include <KTextEditor/Plugin>

#include <QHash>
#include <QList>
#include <QRegularExpression>
#include <QVariant>
#include <QVector>

class ColorPickerInlineNoteProvider : public KTextEditor::InlineNoteProvider
{
    Q_OBJECT
public:
    ColorPickerInlineNoteProvider(KTextEditor::Document *doc);
    ~ColorPickerInlineNoteProvider();

    static void updateColorMatchingCriteria();
    // if startLine == -1, update all notes. endLine is inclusive and optional
    void updateNotes(int startLine=-1, int endLine=-1);

    QVector<int> inlineNotes(int line) const override;
    QSize inlineNoteSize(const KTextEditor::InlineNote &note) const override;
    void paintInlineNote(const KTextEditor::InlineNote &note, QPainter &painter) const override;
    void inlineNoteActivated(const KTextEditor::InlineNote &note, Qt::MouseButtons buttons, const QPoint &globalPos) override;

private:
    KTextEditor::Document *m_doc;
    int m_startChangedLines = -1;
    int m_previousNumLines = -1;

    // line, <colorNoteIndex, otherColorIndex>
    mutable QHash<int, QHash<int, int>> m_colorNoteIndices;

    // config variables shared between all note providers
    static QRegularExpression s_colorRegEx;
    static bool s_putPreviewAfterColor;
};

class KateColorPickerPlugin : public KTextEditor::Plugin
{
    Q_OBJECT
public:
    explicit KateColorPickerPlugin(QObject *parent = nullptr, const QList<QVariant> & = QList<QVariant>());
    ~KateColorPickerPlugin() override;

    QObject *createView(KTextEditor::MainWindow *mainWindow) override;
    void readConfig();

private:
    void addDocument(KTextEditor::Document *doc);

    int configPages() const override
    {
        return 1;
    }
    KTextEditor::ConfigPage *configPage(int number = 0, QWidget *parent = nullptr) override;

    KTextEditor::MainWindow *m_mainWindow;
    QHash<KTextEditor::Document*, ColorPickerInlineNoteProvider*> m_inlineColorNoteProviders;

};

#endif // KATE_COLORPICKER_H
