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

#ifndef DEFAULT_WINDOW_H
#define DEFAULT_WINDOW_H

#include "window.h"

G_BEGIN_DECLS

typedef struct _CDefaultWindow        CDefaultWindow;
typedef struct _CDefaultWindowPrivate CDefaultWindowPrivate;
typedef struct _CDefaultWindowClass   CDefaultWindowClass;

#define C_TYPE_DEFAULT_WINDOW         (c_default_window_get_type ())
#define C_DEFAULT_WINDOW(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), C_TYPE_DEFAULT_WINDOW, CDefaultWindow))
#define C_DEFAULT_WINDOW_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), C_TYPE_DEFAULT_WINDOW, CDefaultWindowClass))
#define C_IS_DEFAULT_WINDOW(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), C_TYPE_DEFAULT_WINDOW))
#define C_IS_DEFAULT_WINDOW_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), C_TYPE_DEFAULT_WINDOW))
#define C_DEFAULT_WINDOW_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), C_TYPE_DEFAULT_WINDOW, CDefaultWindowClass))

GType      c_default_window_get_type      (void);
GType      c_default_window_register_type (GTypeModule* module);

GtkWidget* c_default_window_new           (void);

struct _CDefaultWindow {
  CWindow                base_instance;
  CDefaultWindowPrivate* _private;
};

struct _CDefaultWindowClass {
  CWindowClass           base_class;
};

G_END_DECLS

#endif /* !DEFAULT_WINDOW_H */
