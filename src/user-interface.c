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

#include "user-interface.h"

#include <stdlib.h> /* for exit() */

#include "marshal.h"

struct _CUserInterfacePrivate {
  gchar  * module_path;
  GModule* module;

  gint     priority;
  guint    initialized : 1;
};

#define PRIV(i) (((CUserInterface*)(i))->_private)

enum {
  PROP_0,
  PROP_MODULE_PATH
};

enum {
  CREATE_MAIN_WINDOW,
  LOAD,
  UNLOAD,
  N_SIGNALS
};

G_DEFINE_TYPE (CUserInterface, c_user_interface, G_TYPE_TYPE_MODULE);

static void
c_user_interface_init (CUserInterface* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, C_TYPE_USER_INTERFACE, CUserInterfacePrivate);
}

static gboolean
ui_lookup_create (CUserInterface* self,
                  GtkWidget   *(**creator) (void))
{
  gpointer func = NULL;

  g_return_val_if_fail (creator != NULL, FALSE);

  if (g_module_symbol (PRIV (self)->module, "c_user_interface_module_create_window", &func))
    {
      *creator = func;
      return TRUE;
    }

  return FALSE;
}

static void
ui_constructed (GObject* object)
{
  CUserInterface* self = C_USER_INTERFACE (object);
  gpointer        func = NULL;
  gpointer        func_ptr = &func;

  if (!g_type_module_use (G_TYPE_MODULE (self)))
    {
      g_warning ("error loading module from \"%s\"",
                 g_module_name (PRIV (self)->module));
      return;
    }

  if (ui_lookup_create (self, func_ptr) &&
      g_module_symbol (PRIV (self)->module, "c_ui_module_get_priority", &func))
    {
      gint (*get_prio) (void) = func;

      PRIV (self)->priority = get_prio ();
      PRIV (self)->initialized = TRUE;
    }

  g_type_module_unuse (G_TYPE_MODULE (self));
}

static void
ui_finalize (GObject* object)
{
  g_free (PRIV (object)->module_path);

  G_OBJECT_CLASS (c_user_interface_parent_class)->finalize (object);
}

static void
ui_set_property (GObject     * object,
                 guint         prop_id,
                 GValue const* value,
                 GParamSpec  * pspec)
{
  switch (prop_id)
    {
      case PROP_MODULE_PATH:
        g_return_if_fail (!PRIV (object)->module_path);
        PRIV (object)->module_path = g_value_dup_string (value);
        g_object_notify (object, "module-path");
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static gboolean
ui_load (GTypeModule* module)
{
  if (PRIV (module)->module)
    {
      g_warning ("don't try to load the module twice");
      return FALSE;
    }

  PRIV (module)->module = g_module_open (PRIV (module)->module_path, G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);

  if (!PRIV (module)->module)
    {
      g_warning ("error loading module from \"%s\"",
                 PRIV (module)->module_path);
    }

  return PRIV (module)->module != NULL;
}

static void
ui_unload (GTypeModule* module)
{
  if (!PRIV (module)->module)
    {
      g_warning ("don't try do unload modules twice");
      return;
    }

  if (!g_module_close (PRIV (module)->module))
    {
      g_warning ("error closing module");
    }

  PRIV (module)->module = NULL;
}

static void
c_user_interface_class_init (CUserInterfaceClass* self_class)
{
  GObjectClass    * object_class = G_OBJECT_CLASS (self_class);
  GTypeModuleClass* module_class = G_TYPE_MODULE_CLASS (self_class);

  object_class->constructed  = ui_constructed;
  object_class->finalize     = ui_finalize;
  object_class->set_property = ui_set_property;

  g_object_class_install_property (object_class, PROP_MODULE_PATH,
                                   g_param_spec_string ("module-path", "module path",
                                                        "the path to the file containing the module",
                                                        NULL, G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

  module_class->load   = ui_load;
  module_class->unload = ui_unload;

  g_type_class_add_private (self_class, sizeof (CUserInterfacePrivate));
}

CUserInterface*
c_user_interface_new (gchar const* path)
{
  return g_object_new (C_TYPE_USER_INTERFACE,
                       "module-path", path,
                       NULL);
}

GtkWidget*
c_user_interface_get_main_window (CUserInterface* self)
{
  /* FIXME: move into instance's private data */
  static gpointer main_window = NULL;

  g_return_val_if_fail (C_IS_USER_INTERFACE (self), NULL);

  if (G_TYPE_MODULE (self)->use_count < 1)
    {
      g_warning ("trying to get a main window from an unused user interface, call g_type_module_use() before");
      return NULL;
    }

  if (G_UNLIKELY (!main_window))
    {
      GtkWidget* (*creator) (void) = NULL;

      if (ui_lookup_create (self, &creator))
        {
          main_window = creator ();
        }

      if (!main_window)
        {
          g_warning ("no window was created: exit");
          exit (1);
        }

      g_object_add_weak_pointer (main_window, &main_window);
    }
  else
    {
      g_object_ref_sink (main_window);
    }

  return main_window;
}

gint
c_user_interface_get_priority (CUserInterface const* self)
{
  g_return_val_if_fail (C_IS_USER_INTERFACE (self), 0);

  return PRIV (self)->priority;
}

gboolean
c_user_interface_is_valid (CUserInterface const* self)
{
  g_return_val_if_fail (C_IS_USER_INTERFACE (self), FALSE);

  return PRIV (self)->initialized;
}

