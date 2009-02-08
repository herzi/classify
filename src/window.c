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
static void     implement_main_window (CMainWindowIface* iface);

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
      GInterfaceInfo const iinfo = {
        (GInterfaceInitFunc) implement_main_window,
        NULL, NULL
      };
      c_window_type = g_type_module_register_type (module,
                                                   PARENT_TYPE,
                                                   type_name,
                                                   &info,
                                                   0);
      g_type_module_add_interface (module,
                                   c_window_type,
                                   C_TYPE_MAIN_WINDOW,
                                   &iinfo);
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

GtkUIManager*
c_window_get_ui_manager (CWindow* self)
{
  g_return_val_if_fail (C_IS_WINDOW (self), NULL);

  return PRIV (self)->ui_manager;
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

        c_main_window_initialize (C_MAIN_WINDOW (self));
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
window_finalize (GObject* object)
{
  g_object_unref (PRIV (object)->ui_manager);

  G_OBJECT_CLASS (c_window_parent_class)->finalize (object);
}

static void
window_get_property (GObject   * object,
                     guint       prop_id,
                     GValue    * value,
                     GParamSpec* pspec)
{
  switch (prop_id)
    {
      case PROP_UI_MANAGER:
        g_value_set_object (value, PRIV (object)->ui_manager);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
window_set_property (GObject     * object,
                     guint         prop_id,
                     GValue const* value,
                     GParamSpec  * pspec)
{
  switch (prop_id)
    {
      case PROP_UI_MANAGER:
        g_return_if_fail (!PRIV (object)->ui_manager);

        if (g_value_get_object (value))
          {
#if GTK_CHECK_VERSION(2,14,0)
            PRIV (object)->ui_manager = g_value_dup_object (value);
#else
            PRIV (object)->ui_manager = g_object_ref (g_value_get_object (value));
#endif
          }

        g_object_notify (object, "ui-manager");
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
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

  object_class->finalize     = window_finalize;
  object_class->get_property = window_get_property;
  object_class->set_property = window_set_property;

  gtk_object_class->destroy  = window_destroy;

  c_main_window_implement (object_class, PROP_UI_MANAGER);

  c_window_parent_class = g_type_class_peek_parent (self_class);

  g_type_class_add_private (self_class, sizeof (CWindowPrivate));
}

static GtkMenuShell*
window_get_menus (CMainWindow* main_window)
{
  return GTK_MENU_SHELL (gtk_ui_manager_get_widget (PRIV (main_window)->ui_manager, "/ui/menus"));
}

static GtkToolbar*
window_get_toolbar (CMainWindow* main_window)
{
  return GTK_TOOLBAR (gtk_ui_manager_get_widget (PRIV (main_window)->ui_manager,
                                                 "/ui/toolbar"));
}

static void
window_pack_content (CMainWindow* main_window,
                     GtkWidget  * content)
{
  g_return_if_fail (C_WINDOW_GET_CLASS (main_window)->pack_content);

  C_WINDOW_GET_CLASS (main_window)->pack_content (C_WINDOW (main_window), content);
}

static void
window_pack_menus (CMainWindow * main_window,
                   GtkMenuShell* menus)
{
  g_return_if_fail (C_WINDOW_GET_CLASS (main_window)->pack_menu_shell);

  C_WINDOW_GET_CLASS (main_window)->pack_menu_shell (C_WINDOW (main_window), menus);
}

static void
window_pack_tools (CMainWindow* main_window,
                   GtkToolbar * tools)
{
  g_return_if_fail (C_WINDOW_GET_CLASS (main_window)->pack_toolbar);

  C_WINDOW_GET_CLASS (main_window)->pack_toolbar (C_WINDOW (main_window), tools);
}

static void
implement_main_window (CMainWindowIface* iface)
{
  iface->get_menus    = window_get_menus;
  iface->get_toolbar  = window_get_toolbar;

  iface->pack_content = window_pack_content;
  iface->pack_menus   = window_pack_menus;
  iface->pack_tools   = window_pack_tools;
}

