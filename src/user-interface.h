/* This file is part of classify
 *
 * AUTHORS
 *     Sven Herzberg  <sven@imendio.com>
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

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _CUserInterface        CUserInterface;
typedef struct _CUserInterfacePrivate CUserInterfacePrivate;
typedef struct _CUserInterfaceClass   CUserInterfaceClass;

#define C_TYPE_USER_INTERFACE         (c_user_interface_get_type ())
#define C_USER_INTERFACE(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), C_TYPE_USER_INTERFACE, CUserInterface))
#define C_USER_INTERFACE_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), C_TYPE_USER_INTERFACE, CUserInterfaceClass))
#define C_IS_USER_INTERFACE(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), C_TYPE_USER_INTERFACE))
#define C_IS_USER_INTERFACE_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), C_TYPE_USER_INTERFACE))
#define C_USER_INTERFACE_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), C_TYPE_USER_INTERFACE, CUserInterfaceClass))

GType           c_user_interface_get_type        (void);
CUserInterface* c_user_interface_new             (gchar const         * path);
gboolean        c_user_interface_is_valid        (CUserInterface const* self);
GtkWidget*      c_user_interface_get_main_window (CUserInterface      * self);
gint            c_user_interface_get_priority    (CUserInterface const* self);
gboolean        c_user_interface_test            (CUserInterface      * self);

struct _CUserInterface {
        GTypeModule            base_instance;
        CUserInterfacePrivate* _private;
};

struct _CUserInterfaceClass {
        GTypeModuleClass       base_class;
};

G_END_DECLS

#endif /* !USER_INTERFACE_H */
