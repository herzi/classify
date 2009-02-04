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

#include <gdk/gdkx.h>
#include "window.h"

G_MODULE_EXPORT GtkWidget* c_user_interface_module_create_window (void);
G_MODULE_EXPORT gint       c_ui_module_get_priority              (void);
G_MODULE_EXPORT void       c_ui_module_register_types            (GTypeModule* module);
G_MODULE_EXPORT gboolean   c_ui_module_test                      (void);

GtkWidget*
c_user_interface_module_create_window (void)
{
  return c_window_new ();
}

gint
c_ui_module_get_priority (void)
{
  return 1; /* default priority + 1 */
}

void
c_ui_module_register_types (GTypeModule* module)
{
  c_ui_module_register_type (module);
}

gboolean
c_ui_module_test (void)
{
  GdkAtom  atom_net_supporting_wm_check = gdk_atom_intern_static_string ("_NET_SUPPORTING_WM_CHECK");
  GdkAtom  atom_net_wm_name             = gdk_atom_intern_static_string ("_NET_WM_NAME");
  GdkAtom  actual_type = gdk_atom_intern_static_string ("WINDOW");
  guchar * data = NULL;
  gint     actual_format = 0;
  gint     actual_length = 0;
  gboolean result = FALSE;

  /* http://standards.freedesktop.org/wm-spec/wm-spec-latest.html#id2550836 */

  if (!gdk_property_get (gdk_screen_get_root_window (gdk_screen_get_default ()),
                         atom_net_supporting_wm_check,
                         GDK_NONE,
                         0, 4,
                         FALSE,
                         &actual_type,
                         &actual_format,
                         &actual_length,
                         &data))
    {
      g_warning ("Couldn't get _NET_SUPPORTING_WM_CHECK on the root window.");
      g_warning ("\tplease use a window manager compliant to the FreeDesktop.org specification:");
      g_warning ("\thttp://standards.freedesktop.org/wm-spec/wm-spec-latest.html#id2550836");
      return FALSE;
    }

  if (actual_type != gdk_atom_intern_static_string ("WINDOW"))
    {
      g_warning ("Result of _NET_SUPPORTING_WM_CHECK on the root window was %s, not %s.",
                 gdk_atom_name (actual_type),
                 "WINDOW");
      return FALSE;
    }

  if (actual_format != 32)
    {
      g_warning ("Result of _NET_SUPPORTING_WM_CHECK on the root window had format %d, not %d.",
                 actual_format,
                 32);
      return FALSE;
    }

  if (actual_length != 4)
    {
      g_warning ("Result of _NET_SUPPORTING_WM_CHECK on the root window was length %d, not %d.",
                 actual_length,
                 4);
      return FALSE;
    }

  GdkWindow* wm_window = gdk_window_foreign_new (*(GdkNativeWindow*)data);

  g_free (data);
  data = NULL;

  if (!gdk_property_get (wm_window,
                         atom_net_supporting_wm_check,
                         actual_type,
                         0, 4,
                         FALSE,
                         &actual_type,
                         &actual_format,
                         &actual_length,
                         &data))
    {
      g_object_unref (wm_window);

      g_warning ("Couldn't get _NET_SUPPORTING_WM_CHECK on the window manager window");
      return FALSE;
    }

  if (actual_type != gdk_atom_intern_static_string ("WINDOW"))
    {
      g_warning ("Result of _NET_SUPPORTING_WM_CHECK on the window manager window was %s, not %s.",
                 gdk_atom_name (actual_type),
                 "WINDOW");
      return FALSE;
    }

  if (actual_format != 32)
    {
      g_warning ("Result of _NET_SUPPORTING_WM_CHECK on the window manager window had format %d, not %d.",
                 actual_format,
                 32);
      return FALSE;
    }

  if (actual_length != 4)
    {
      g_warning ("Result of _NET_SUPPORTING_WM_CHECK on the window manager window was length %d, not %d.",
                 actual_length,
                 4);
      return FALSE;
    }

#ifdef GDK_WINDOWING_X11
  if (GDK_WINDOW_XID (wm_window) != *(GdkNativeWindow*)data)
    {
      g_warning ("Result of _NET_SUPPORTING_WM_CHECK on the window manager window didn't return the window itself");

      return FALSE;
    }
#else
#error "unsupported windowing system of GDK"
#endif

  g_free (data);
  actual_type = GDK_NONE;
  if (!gdk_property_get (wm_window,
                         atom_net_wm_name,
                         actual_type,
                         0, 256,
                         FALSE,
                         &actual_type,
                         &actual_format,
                         &actual_length,
                         &data))
    {
      g_warning ("Couldn't get _NET_WM_NAME on the window manager window");
      return FALSE;
    }

#if 0
  {
    gchar* name = g_strndup ((gchar*)data, actual_length);
    g_print ("window manager is %s\n", name);
    g_free (name);
  }
#endif

  result = actual_length >= 8 && g_str_has_prefix ((gchar*)data, "matchbox");

  g_free (data);

  return result;
}

