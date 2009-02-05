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

#ifndef HAVE_HILDON
#include "preferences.h"
#endif
#include "task-list.h"
#include "task-widget.h"

#include <glib/gi18n.h>

struct _CWindowPrivate {
  GtkUIManager  * ui_manager;
  GtkActionGroup* actions;

  GtkWidget     * vbox;
  GtkWidget     * scrolled_window;
};

#define PRIV(i) (((CWindow*)(i))->_private)

static void     c_window_init        (CWindow     * self);
static void     c_window_class_init  (CWindowClass* self_class);

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
c_window_get_button (CWindow* self)
{
	return g_object_get_data (G_OBJECT (self), "CWindow::Button");
}

GtkWidget*
c_window_get_tree (CWindow* self)
{
	return g_object_get_data (G_OBJECT (self), "CWindow::TreeView");
}

static void
file_close_activated (GtkAction* action,
		      CWindow  * self)
{
	gtk_widget_destroy (GTK_WIDGET (self));
}

static void
edit_copy_activated (GtkAction* action,
		     CWindow  * self)
{
	if (GTK_IS_EDITABLE (gtk_window_get_focus (GTK_WINDOW (self)))) {
		gtk_editable_copy_clipboard (GTK_EDITABLE (gtk_window_get_focus (GTK_WINDOW (self))));
	} else if (C_IS_TASK_WIDGET (gtk_window_get_focus (GTK_WINDOW (self)))) {
                c_task_widget_copy_clipboard (C_TASK_WIDGET (gtk_window_get_focus (GTK_WINDOW (self))));
	}
}

static void
edit_delete_activated (GtkAction* action,
		       CWindow  * self)
{
        c_task_widget_delete_selected (C_TASK_WIDGET (c_window_get_tree (self)));
}

static void
edit_paste_activated (GtkAction* action,
		      CWindow  * self)
{
	if (GTK_IS_EDITABLE (gtk_window_get_focus(GTK_WINDOW (self)))) {
		gtk_editable_paste_clipboard (GTK_EDITABLE (gtk_window_get_focus(GTK_WINDOW (self))));
	} else if (C_IS_TASK_WIDGET (gtk_window_get_focus(GTK_WINDOW (self)))) {
                c_task_widget_paste_clipboard (C_TASK_WIDGET (gtk_window_get_focus(GTK_WINDOW (self))));
	}
}

#ifndef HAVE_HILDON
static void
open_prefs (GtkAction* action,
            CWindow  * self)
{
  GtkWidget* dialog = c_preferences_new (GTK_WINDOW (self));
  gtk_dialog_run     (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}
#endif

static void
edit_rename (GtkAction* action,
             CWindow  * self)
{
  c_task_widget_rename_selection (C_TASK_WIDGET (c_window_get_tree (self)));
}

static void
task_bottom_activated (GtkAction* action,
                       CWindow  * self)
{
  c_task_widget_move_bottom (C_TASK_WIDGET (c_window_get_tree (self)));
}

static void
task_new_activated (GtkAction* action,
                    CWindow  * self)
{
  c_task_widget_create_task (C_TASK_WIDGET (c_window_get_tree (self)));
}

static void
task_top_activated (GtkAction* action,
                    CWindow  * self)
{
  c_task_widget_move_top (C_TASK_WIDGET (c_window_get_tree (self)));
}

static void
view_expand_all_activated (GtkAction* action,
                           CWindow  * self)
{
  gtk_tree_view_expand_all (GTK_TREE_VIEW (c_window_get_tree (self)));
}

static void
view_collapse_all_activated (GtkAction* action,
                             CWindow  * self)
{
  gtk_tree_view_collapse_all (GTK_TREE_VIEW (c_window_get_tree (self)));
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
	GtkActionEntry  entries[] = {
		{"File", NULL, N_("_File")},

		{"FileClose", GTK_STOCK_CLOSE, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (file_close_activated)},

		{"Edit", NULL, N_("_Edit")},
		{"EditCopy", GTK_STOCK_COPY, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (edit_copy_activated)},
		{"EditDelete", GTK_STOCK_DELETE, NULL,
		 "Delete", NULL, // FIXME: add tooltip
		 G_CALLBACK (edit_delete_activated)},
		{"EditPaste", GTK_STOCK_PASTE, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (edit_paste_activated)},
		{"EditRename", NULL, N_("_Rename"),
		 "F2", NULL, // FIXME: add tooltip
		 G_CALLBACK (edit_rename)},

		{"TaskBottom", GTK_STOCK_GOTO_BOTTOM, N_("To _Bottom"),
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (task_bottom_activated)},
		{"TaskNew", GTK_STOCK_ADD, NULL,
		 "<Ctrl>n", NULL, // FIXME: add tooltip
		 G_CALLBACK (task_new_activated)},
		{"TaskTop", GTK_STOCK_GOTO_TOP, N_("To _Top"),
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (task_top_activated)},

		{"View", NULL, N_("_View")},
		{"ViewExpandAll", NULL, N_("_Expand All"),
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (view_expand_all_activated)},
		{"ViewCollapseAll", NULL, N_("_Collapse All"),
		 NULL, NULL, // FIXME: add tooltip
                 G_CALLBACK (view_collapse_all_activated)}
	};
	GtkActionGroup* group;
        CTaskList   * store;
        GtkWidget   * tree;

        PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, C_TYPE_WINDOW, CWindowPrivate);

        PRIV (self)->vbox = gtk_vbox_new (FALSE, 0);

        PRIV (self)->ui_manager = gtk_ui_manager_new ();
        gtk_window_add_accel_group  (GTK_WINDOW (self),
                                     gtk_ui_manager_get_accel_group (PRIV (self)->ui_manager));

        /* FIXME: remove this and properly use the private data */
        g_object_set_data_full (G_OBJECT (self),
                                "CWindow::UIManager",
                                PRIV (self)->ui_manager,
                                g_object_unref);

        /* FIXME: store/restore window size and position properly */
        gtk_window_set_default_size (GTK_WINDOW (self),
				     400, 300);
	gtk_window_set_title        (GTK_WINDOW (self),
                                     _("List of Tasks"));
	g_signal_connect (self, "destroy",
                          G_CALLBACK (gtk_main_quit), NULL);

        group = gtk_action_group_new ("main-group");
        gtk_action_group_add_actions (group, entries, G_N_ELEMENTS (entries), self);

        gtk_ui_manager_insert_action_group (PRIV (self)->ui_manager,
                                            group,
                                            0);

        /* FIXME: delay until finalize */
        g_object_unref (group);
        PRIV (self)->actions = group;

	PRIV (self)->scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	//gtk_scrolled_window_set_policy      (GTK_SCROLLED_WINDOW (PRIV (self)->scrolled_window),
	//				     GTK_POLICY_NEVER,
	//				     GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (PRIV (self)->scrolled_window),
					     GTK_SHADOW_IN);

	tree = c_task_widget_new ();

        /* FIXME: properly use the private data */
	g_object_set_data_full (G_OBJECT (self),
				"CWindow::TreeView",
				g_object_ref_sink (tree),
				g_object_unref);
	store = c_task_list_new_default ();
	gtk_tree_view_set_model  (GTK_TREE_VIEW (tree),
				  GTK_TREE_MODEL (store));
	g_object_unref (store);
	gtk_widget_show (tree);
	gtk_container_add (GTK_CONTAINER (PRIV (self)->scrolled_window), tree);
}

static void
window_constructed (GObject* object)
{
  CWindow* self = C_WINDOW (object);

  if (G_OBJECT_CLASS (c_window_parent_class)->constructed)
    {
      G_OBJECT_CLASS (c_window_parent_class)->constructed (object);
    }

  C_WINDOW_GET_CLASS (object)->pack_menu_shell (self);
  C_WINDOW_GET_CLASS (object)->pack_toolbar    (self);
  C_WINDOW_GET_CLASS (object)->pack_content    (self, PRIV (self)->scrolled_window);

  gtk_widget_show (PRIV (self)->scrolled_window);

  gtk_widget_show (PRIV (self)->vbox);
  gtk_container_add (GTK_CONTAINER (self), PRIV (self)->vbox);
}

static void
window_pack_menu_shell (CWindow* self)
{
#ifndef HAVE_HILDON
  GtkActionGroup* group;
  GtkActionEntry  entries[] = {
		{"EditPreferences", GTK_STOCK_PREFERENCES, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (open_prefs)}
  };
  GtkWidget     * shell;
  GError        * error = NULL;

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

  shell = gtk_ui_manager_get_widget (PRIV (self)->ui_manager, "/ui/menus");

  gtk_box_pack_start (GTK_BOX (PRIV (self)->vbox), GTK_WIDGET (shell),
                      FALSE, FALSE, 0);
#endif
}

static void
window_pack_toolbar (CWindow* self)
{
#ifndef HAVE_HILDON
  GtkWidget* toolbar;
#endif
  GError   * error = NULL;

        gtk_ui_manager_add_ui_from_string  (PRIV (self)->ui_manager,
                                            "<ui>"
						"<toolbar name='toolbar'>"
							"<toolitem action='TaskNew'/>"
							"<separator/>"
							"<toolitem action='TaskTop'/>"
							"<toolitem action='TaskBottom'/>"
						"</toolbar>"
					    "</ui>",
					    -1,
					    &error);

	if (error) {
		g_warning ("Error setting up the user interface: %s",
			   error->message);
		g_error_free (error);
		error = NULL;
	}

#ifndef HAVE_HILDON
  toolbar = gtk_ui_manager_get_widget (PRIV (self)->ui_manager, "/ui/toolbar");

  gtk_box_pack_start (GTK_BOX (PRIV (self)->vbox), toolbar,
                      FALSE, FALSE, 0);
#endif
}

static void
window_pack_content (CWindow  * self,
                     GtkWidget* content)
{
  gtk_box_pack_end_defaults (GTK_BOX (PRIV (self)->vbox), PRIV (self)->scrolled_window);
}

static void
c_window_class_init (CWindowClass* self_class)
{
  GObjectClass  * object_class = G_OBJECT_CLASS (self_class);

  object_class->constructed = window_constructed;

  self_class->pack_menu_shell  = window_pack_menu_shell;
  self_class->pack_toolbar     = window_pack_toolbar;
  self_class->pack_content     = window_pack_content;

  c_window_parent_class = g_type_class_peek_parent (self_class);

  g_type_class_add_private (self_class, sizeof (CWindowPrivate));
}

