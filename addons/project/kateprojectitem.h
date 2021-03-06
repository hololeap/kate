/*  This file is part of the Kate project.
 *
 *  SPDX-FileCopyrightText: 2010 Christoph Cullmann <cullmann@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KATE_PROJECT_ITEM_H
#define KATE_PROJECT_ITEM_H

#include <KTextEditor/ModificationInterface>
#include <QStandardItem>

namespace KTextEditor
{
class Document;

}

/**
 * Class representing a item inside a project.
 * Items can be: projects, directories, files
 */
class KateProjectItem : public QStandardItem
{
public:
    /**
     * Possible Types
     */
    enum Type { Project, Directory, File };

    /**
     * construct new item with given text
     * @param type type for this item
     * @param text text for this item
     */
    KateProjectItem(Type type, const QString &text);

    /**
     * deconstruct project
     */
    ~KateProjectItem() override;

    /**
     * Overwritten data method for on-demand icon creation and co.
     * @param role role to get data for
     * @return data for role
     */
    QVariant data(int role = Qt::UserRole + 1) const override;

public:
    void slotModifiedChanged(KTextEditor::Document *);
    void slotModifiedOnDisk(KTextEditor::Document *document, bool isModified, KTextEditor::ModificationInterface::ModifiedOnDiskReason reason);

private:
    QIcon *icon() const;

private:
    /**
     * type
     */
    const Type m_type;

    /**
     * cached icon
     */
    mutable QIcon *m_icon;

    /**
     * for document icons
     */
    QString m_emblem;
};

#endif
