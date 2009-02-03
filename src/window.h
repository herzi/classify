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

#ifdef HAVE_HILDON
#include <hildon/hildon.h>
#else
#include <gtk/gtkwindow.h>
#endif

G_BEGIN_DECLS

typedef struct _CWindow        CWindow;
typedef struct _CWindowPrivate CWindowPrivate;
typedef struct _CWindowClass   CWindowClass;

#define C_TYPE_WINDOW         (c_window_get_type ())
#define C_WINDOW(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), C_TYPE_WINDOW, CWindow))
#define C_WINDOW_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), C_TYPE_WINDOW, CWindowClass))
#define C_IS_WINDOW(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), C_TYPE_WINDOW))
#define C_IS_WINDOW_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), C_TYPE_WINDOW))
#define C_WINDOW_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), C_TYPE_WINDOW, CWindowClass))

G_MODULE_EXPORT GType      c_ui_module_register_type (GTypeModule* module);

GType      c_window_get_type   (void);
GtkWidget* c_window_new        (void);
GtkWidget* c_window_get_button (CWindow* self);
GtkWidget* c_window_get_tree   (CWindow* self);

struct _CWindow {
#ifdef HAVE_HILDON
	HildonWindow       base_instance;
#else
	GtkWindow          base_instance;
#endif
	CWindowPrivate   * _private;
};

struct _CWindowClass {
#ifdef HAVE_HILDON
	HildonWindowClass  base_class;
#else
	GtkWindowClass     base_class;
#endif
};

G_END_DECLS

#endif /* !WINDOW_H */
