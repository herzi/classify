/* This file is part of classify
 *
 * AUTHORS
 *     Sven Herzberg  <herzi@lanedo.com>
 *
 * Copyright (C) 2009  Sven Herzberg
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

#ifndef HILDON_WINDOW_H
#define HILDON_WINDOW_H

#include "window.h"

G_BEGIN_DECLS

typedef struct _CHildonWindow        CHildonWindow;
typedef struct _CHildonWindowPrivate CHildonWindowPrivate;
typedef struct _CHildonWindowClass   CHildonWindowClass;

#define C_TYPE_HILDON_WINDOW         (c_hildon_window_get_type ())
#define C_HILDON_WINDOW(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), C_TYPE_HILDON_WINDOW, CHildonWindow))
#define C_HILDON_WINDOW_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), C_TYPE_HILDON_WINDOW, CHildonWindowClass))
#define C_IS_HILDON_WINDOW(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), C_TYPE_HILDON_WINDOW))
#define C_IS_HILDON_WINDOW_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), C_TYPE_HILDON_WINDOW))
#define C_HILDON_WINDOW_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), C_TYPE_HILDON_WINDOW, CHildonWindowClass))

GType      c_hildon_window_get_type      (void);
GType      c_hildon_window_register_type (GTypeModule* module);
GtkWidget* c_hildon_window_new           (void);

struct _CHildonWindow {
        CWindow               base_instance;
        CHildonWindowPrivate* _private;
};

struct _CHildonWindowClass {
        CWindowClass          base_class;
};

G_END_DECLS

#endif /* !HILDON_WINDOW_H */
