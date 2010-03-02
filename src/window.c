/* This file is part of classify
 *
 * AUTHORS
 *     Sven Herzberg  <herzi@lanedo.com>
 *
 * Copyright (C) 2007,2009  Sven Herzberg
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

#include "window.h"

#include <gtk/gtk.h>

#include "main-window.h"

#include <glib/gi18n.h>

struct _CWindowPrivate {
  GtkUIManager* ui_manager;
};

#define PRIV(i) (((CWindow*)(i))->_private)

enum {
  PROP_0,
  PROP_UI_MANAGER
};

static void     c_window_init         (CWindow         * self);
static void     c_window_class_init   (CWindowClass    * self_class);

#ifdef HAVE_HILDON
#define PARENT_TYPE HILDON_TYPE_WINDOW
#define type_name "CWindowHildon"
#else
#define PARENT_TYPE GTK_TYPE_WINDOW
#define type_name "CWindowDefault"
#endif

static GType    c_window_type = 0;
static gpointer c_window_parent_class = NULL;

GType
c_ui_module_register_type (GTypeModule* module)
{
  if (G_UNLIKELY (!c_window_type))
    {
      GTypeInfo const info = {
        sizeof (CWindowClass),
        NULL, NULL,
        (GClassInitFunc) c_window_class_init,
        NULL, NULL,
        sizeof (CWindow), 0,
        (GInstanceInitFunc) c_window_init,
        NULL
      };
      c_window_type = g_type_module_register_type (module,
                                                   PARENT_TYPE,
                                                   type_name,
                                                   &info,
                                                   0);
    }

  return c_window_type;
}
#undef type_name
#undef PARENT_TYPE

GType
c_window_get_type (void)
{
  return c_window_type;
}

GtkWidget*
c_window_new (void)
{
  return g_object_new (C_TYPE_WINDOW, NULL);
}

static void
c_window_init (CWindow* self)
{
        PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, C_TYPE_WINDOW, CWindowPrivate);
}

static void
window_constructed (GObject* object)
{
  CWindow* self = C_WINDOW (object);

  if (G_OBJECT_CLASS (c_window_parent_class)->constructed)
    {
      G_OBJECT_CLASS (c_window_parent_class)->constructed (object);
    }

  c_main_window_constructed (C_MAIN_WINDOW (self));
}

static void
window_destroy (GtkObject* object)
{
  gtk_main_quit ();

  GTK_OBJECT_CLASS (c_window_parent_class)->destroy (object);
}

static void
c_window_class_init (CWindowClass* self_class)
{
  GObjectClass  * object_class = G_OBJECT_CLASS (self_class);
  GtkObjectClass* gtk_object_class = GTK_OBJECT_CLASS (self_class);

  object_class->constructed  = window_constructed;

  gtk_object_class->destroy  = window_destroy;

  c_window_parent_class = g_type_class_peek_parent (self_class);

  g_type_class_add_private (self_class, sizeof (CWindowPrivate));
}

