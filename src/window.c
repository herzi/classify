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

#include "window.h"

#include <gtk/gtk.h>
#include "preferences.h"

#include <glib/gi18n.h>

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

GtkWidget*
c_window_get_vbox (CWindow* self)
{
	return gtk_bin_get_child (GTK_BIN (self));
}

static void
open_prefs (GtkAction* action,
	    CWindow  * self)
{
	GtkWidget* dialog = c_preferences_new (GTK_WINDOW (self));
	gtk_dialog_run     (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

static gboolean
tree_button_press_event (GtkWidget     * tree,
			 GdkEventButton* event,
			 CWindow       * window)
{
	gboolean result = FALSE;

	if (event->button == 1 && event->type == GDK_2BUTTON_PRESS) {
		GtkTreeSelection* selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree));
		if (gtk_tree_selection_count_selected_rows (selection) == 1) {
			GtkTreeViewColumn* column;
			GtkTreeModel* model = NULL;
			GList       * selected = gtk_tree_selection_get_selected_rows (selection, &model);
			GList       * renderers;

			column    = gtk_tree_view_get_column (GTK_TREE_VIEW (tree), 0);
			renderers = gtk_tree_view_column_get_cell_renderers (column);

			g_object_set (renderers->data, "editable", TRUE, NULL);
			gtk_tree_view_set_cursor (GTK_TREE_VIEW (tree),
						  selected->data,
						  column,
						  TRUE);
			g_object_set (renderers->data, "editable", FALSE, NULL);
			g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
			g_list_free    (selected);

			g_list_free    (renderers);

			result = TRUE;
		}
	}

	return result;
}

GtkWidget*
c_window_new (void)
{
	GtkActionEntry  entries[] = {
		{"File", NULL, N_("_File")},

		{"EditPreferences", GTK_STOCK_PREFERENCES, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (open_prefs)}
	};
	GtkActionGroup* group;
	GtkUIManager* ui_manager = gtk_ui_manager_new ();
	GtkWidget   * button;
	GtkWidget   * result = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	GtkWidget   * tree;
	GtkWidget   * vbox   = gtk_vbox_new   (FALSE, 0);
	GError      * error = NULL;

	gtk_window_set_default_size (GTK_WINDOW (result),
				     400, 300);
	gtk_window_set_title        (GTK_WINDOW (result),
				     _("Classify"));
	g_signal_connect (result, "destroy",
			  G_CALLBACK (gtk_main_quit), NULL);

	group = gtk_action_group_new ("main-group");
	gtk_action_group_add_actions (group, entries, G_N_ELEMENTS (entries), result);

	gtk_ui_manager_insert_action_group (ui_manager,
					    group,
					    0);
	gtk_ui_manager_add_ui_from_string  (ui_manager,
					    "<ui>"
						"<menubar name='menubar'>"
							"<menu action='File'>"
								"<menuitem action='EditPreferences' />"
							"</menu>"
						"</menubar>"
					    "</ui>",
					    -1,
					    &error);
	g_object_unref (group);

	if (error) {
		g_warning ("Error setting up the user interface: %s",
			   error->message);
		g_error_free (error);
		error = NULL;
	}

	gtk_box_pack_start (GTK_BOX (vbox),
			    gtk_ui_manager_get_widget (ui_manager, "/menubar"),
			    FALSE,
			    FALSE,
			    0);
	gtk_widget_show    (vbox);
	gtk_container_add  (GTK_CONTAINER (result),
			    vbox);

	button = gtk_button_new_from_stock (GTK_STOCK_ADD);
	gtk_widget_show (button);
	gtk_box_pack_start (GTK_BOX (vbox),
			    button,
			    FALSE,
			    FALSE,
			    0);
	g_object_set_data_full (G_OBJECT (result),
				"CWindow::Button",
				g_object_ref_sink (button),
				g_object_unref);

	tree = gtk_tree_view_new ();
	gtk_tree_view_set_reorderable (GTK_TREE_VIEW (tree),
				       TRUE);
	g_signal_connect (tree, "button-press-event",
			  G_CALLBACK (tree_button_press_event), result);
	g_object_set_data_full (G_OBJECT (result),
				"CWindow::TreeView",
				g_object_ref_sink (tree),
				g_object_unref);

	g_object_set_data_full (G_OBJECT (result),
				"CWindow::UIManager",
				ui_manager,
				g_object_unref);

	return result;
}

