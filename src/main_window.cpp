/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2017 Deepin, Inc.
 *               2011 ~ 2017 Wang Yong
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Wang Yong <wangyong@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "main_window.h"
#include <DTitlebar>
#include <QLabel>
#include <QDebug>
#include "dthememanager.h"

MainWindow::MainWindow(DMainWindow *parent) : DMainWindow(parent)
{
    DThemeManager::instance()->setTheme("dark");
    
    installEventFilter(this);   // add event filter

    layoutWidget = new QWidget();
    layout = new QVBoxLayout(layoutWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    this->setCentralWidget(layoutWidget);
    
    editor = new Editor();
    layout->addWidget(editor);
    
    QLabel *label = new QLabel();
    layout->addWidget(label, 0, Qt::AlignBottom);
    
    tabbar = new Tabbar();
    this->titlebar()->setCustomWidget(tabbar, Qt::AlignVCenter, false);
}

MainWindow::~MainWindow()
{
    // We don't need clean pointers because application has exit here.
}

void MainWindow::keyPressEvent(QKeyEvent *)
{
    
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    tabbar->setFixedWidth(rect().width() - 130);
}
