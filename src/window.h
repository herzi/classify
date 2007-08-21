/* This file is part of classify
 *
 * AUTHORS
 *     Sven Herzberg  <herzi@gnome-de.org>
 *
 * Copyright (C) 2007  Sven Herzberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtkwindow.h>

G_BEGIN_DECLS

#define CWindow     GtkWindow
#define C_WINDOW(i) GTK_WINDOW(i)

GtkWidget* c_window_get_button (CWindow* self);
GtkWidget* c_window_get_swin   (CWindow* self);
GtkWidget* c_window_get_tree   (CWindow* self);
GtkWidget* c_window_get_vbox   (CWindow* self);
GtkWidget* c_window_new        (void);

G_END_DECLS

#endif /* !WINDOW_H */