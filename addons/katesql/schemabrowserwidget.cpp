/*
   Copyright (C) 2010  Marco Mentasti  <marcomentasti@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "schemabrowserwidget.h"
#include "schemawidget.h"

#include <QTreeView>
#include <QVBoxLayout>

SchemaBrowserWidget::SchemaBrowserWidget(QWidget *parent, SQLManager *manager)
    : QWidget(parent)
    , m_schemaWidget(new SchemaWidget(this, manager))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_schemaWidget);
    setLayout(layout);
}

SchemaBrowserWidget::~SchemaBrowserWidget()
{
}

SchemaWidget *SchemaBrowserWidget::schemaWidget() const
{
    return m_schemaWidget;
}
