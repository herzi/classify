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

#include "default-window.h"

#include "main-window.h"
#include "preferences.h"

struct _CDefaultWindowPrivate {
  GtkUIManager* ui_manager;

  GtkWidget   * vbox;
  GtkWidget   * scrolled_window;
};

#define PRIV(i) (((CDefaultWindow*)(i))->_private)

enum {
  PROP_0,
  PROP_UI_MANAGER
};

static void c_default_window_init       (CDefaultWindow     * self);
static void c_default_window_class_init (CDefaultWindowClass* self_class);
static void implement_main_window       (CMainWindowIface   * iface);

static GType    c_default_window_type = 0;
static gpointer c_default_window_parent_class = NULL;

GType
c_default_window_get_type (void)
{
  return c_default_window_type;
}

GType
c_default_window_register_type (GTypeModule* module)
{
  if (G_UNLIKELY (!c_default_window_type))
    {
      GTypeInfo const info = {
        sizeof (CDefaultWindowClass),
        NULL, NULL,
        (GClassInitFunc) c_default_window_class_init,
        NULL, NULL,
        sizeof (CDefaultWindow), 0,
        (GInstanceInitFunc) c_default_window_init,
        NULL
      };
      GInterfaceInfo const iinfo = {
        (GInterfaceInitFunc) implement_main_window,
        NULL, NULL
      };

      c_default_window_type = g_type_module_register_type (module,
                                                           c_ui_module_register_type (module),
                                                           G_STRINGIFY (CDefaultWindow),
                                                           &info,
                                                           0);
      g_message ("begin: if you see a warning after this...");
      g_type_module_add_interface (module,
                                   c_default_window_type,
                                   C_TYPE_MAIN_WINDOW,
                                   &iinfo);
      g_message ("end:   ...and before this, you need the patches from http://bugzilla.gnome.org/show_bug.cgi?id=570826");
    }

  return c_default_window_type;
}

static void
open_prefs (GtkAction* action,
            CWindow  * self)
{
  GtkWidget* dialog = c_preferences_new (GTK_WINDOW (self));
  gtk_dialog_run     (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static void
c_default_window_init (CDefaultWindow* self)
{
  /* FIXME: store/restore window size and position properly */
  GtkActionGroup* group;
  GtkActionEntry  entries[] = {
		{"EditPreferences", GTK_STOCK_PREFERENCES, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (open_prefs)}
  };
  GError        * error = NULL;

  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, C_TYPE_DEFAULT_WINDOW, CDefaultWindowPrivate);

        c_main_window_initialize (C_MAIN_WINDOW (self));

  PRIV (self)->vbox = gtk_vbox_new (FALSE, 0);

  group = gtk_action_group_new ("backend-actions");
  gtk_action_group_add_actions (group, entries, G_N_ELEMENTS (entries), self);
  gtk_ui_manager_insert_action_group (PRIV (self)->ui_manager, group, -1);
  g_object_unref (group);

        gtk_ui_manager_add_ui_from_string  (PRIV (self)->ui_manager,
                                            "<ui>"
                                                "<menubar name='menus'>"
                                                        "<menu action='File'>"
                                                                "<menuitem action='TaskNew'/>"
								"<separator/>"
								"<menuitem action='FileClose' />"
							"</menu>"
							"<menu action='Edit'>"
								"<menuitem action='EditCopy'/>"
								"<menuitem action='EditPaste'/>"
								"<menuitem action='EditDelete'/>"
								"<separator/>"
								"<menuitem action='EditRename'/>"
								"<separator/>"
								"<menuitem action='EditPreferences' />"
							"</menu>"
							"<menu action='View'>"
								"<menuitem action='ViewExpandAll'/>"
								"<menuitem action='ViewCollapseAll'/>"
							"</menu>"
						"</menubar>"
					    "</ui>",
					    -1,
					    &error);

  if (error)
    {
      g_warning ("error constructing window: %s", error->message);
      g_error_free (error);
      return;
    }

	PRIV (self)->scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	//gtk_scrolled_window_set_policy      (GTK_SCROLLED_WINDOW (PRIV (self)->scrolled_window),
	//				     GTK_POLICY_NEVER,
	//				     GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (PRIV (self)->scrolled_window),
					     GTK_SHADOW_IN);

  gtk_box_pack_end_defaults (GTK_BOX (PRIV (self)->vbox), PRIV (self)->scrolled_window);
  gtk_container_add (GTK_CONTAINER (self), PRIV (self)->vbox);
}

static void
default_window_pack_menu_shell (CMainWindow * window,
                                GtkMenuShell* menus)
{
  gtk_box_pack_start (GTK_BOX (PRIV (window)->vbox), GTK_WIDGET (menus),
                      FALSE, FALSE, 0);
}

static void
default_window_pack_toolbar (CMainWindow* window,
                             GtkToolbar * toolbar)
{
  gtk_box_pack_start (GTK_BOX (PRIV (window)->vbox), GTK_WIDGET (toolbar),
                      FALSE, FALSE, 0);
}

static void
default_window_pack_content (CMainWindow* window,
                             GtkWidget  * content)
{
  gtk_container_add (GTK_CONTAINER (PRIV (window)->scrolled_window), content);

  gtk_widget_show (PRIV (window)->scrolled_window);

  gtk_widget_show (PRIV (window)->vbox);
}

static void
window_finalize (GObject* object)
{
  g_object_unref (PRIV (object)->ui_manager);

  G_OBJECT_CLASS (c_default_window_parent_class)->finalize (object);
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
c_default_window_class_init (CDefaultWindowClass* self_class)
{
  GObjectClass  * object_class = G_OBJECT_CLASS (self_class);

  c_default_window_parent_class = g_type_class_peek_parent (self_class);

  object_class->finalize     = window_finalize;
  object_class->get_property = window_get_property;
  object_class->set_property = window_set_property;

  c_main_window_implement (object_class, PROP_UI_MANAGER);

  g_type_class_add_private (self_class, sizeof (CDefaultWindowPrivate));
}

GtkWidget*
c_default_window_new (void)
{
  return g_object_new (C_TYPE_DEFAULT_WINDOW,
                       NULL);
}

static GtkWidget*
default_window_get_content (CMainWindow* main_window)
{
  return gtk_bin_get_child (GTK_BIN (PRIV (main_window)->scrolled_window));
}

static void
implement_main_window (CMainWindowIface* iface)
{
  iface->get_content  = default_window_get_content;
  iface->pack_content = default_window_pack_content;

  iface->pack_tools   = default_window_pack_toolbar;
  iface->pack_menus   = default_window_pack_menu_shell;
}

