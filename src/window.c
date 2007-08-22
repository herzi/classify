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

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "preferences.h"
#include "task-list.h"

#include <glib/gi18n.h>

G_DEFINE_TYPE (CWindow, c_window, GTK_TYPE_WINDOW);

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
open_prefs (GtkAction* action,
	    CWindow  * self)
{
	GtkWidget* dialog = c_preferences_new (GTK_WINDOW (self));
	gtk_dialog_run     (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

static void
tree_edit_path (GtkTreeView* tree,
		GtkTreePath* path)
{
	GList       * renderers;
	renderers = gtk_tree_view_column_get_cell_renderers (gtk_tree_view_get_column (tree, 0));
	g_object_set (renderers->data, "editable", TRUE, NULL);
	gtk_tree_view_set_cursor (tree,
				  path,
				  gtk_tree_view_get_column (tree, 0),
				  TRUE);
	g_object_set (renderers->data, "editable", FALSE, NULL);
	g_list_free (renderers);
}

static void
task_bottom_activated (GtkAction* action,
		       CWindow  * self)
{
	GtkTreeSelection* selection;
	GtkTreeIter       iter;
	GList           * selected;

	g_return_if_fail (C_IS_WINDOW (self));

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (c_window_get_tree (self)));

	if (1 != gtk_tree_selection_count_selected_rows (selection)) {
		return;
	}

	selected = gtk_tree_selection_get_selected_rows (selection, NULL);
	g_return_if_fail (selected);
	g_return_if_fail (gtk_tree_model_get_iter (gtk_tree_view_get_model (GTK_TREE_VIEW (c_window_get_tree (self))), &iter, selected->data));
	gtk_list_store_move_before (GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (c_window_get_tree (self)))),
				    &iter,
				    NULL);
	g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
	g_list_free    (selected);
}

static void
task_new_activated (GtkAction* action,
		    CWindow  * self)
{
	GtkTreeSelection* selection;
	GtkTreeView * tree  = GTK_TREE_VIEW  (c_window_get_tree (self));
	GtkListStore* store = GTK_LIST_STORE (gtk_tree_view_get_model (tree));
	GtkTreePath * path;
	GtkTreeIter   iter;

	selection = gtk_tree_view_get_selection (tree);

	if (gtk_tree_selection_count_selected_rows (selection) == 1) {
		GtkTreeIter  selected_iter;
		GList      * selected = gtk_tree_selection_get_selected_rows (selection, NULL);

		if (selected && gtk_tree_model_get_iter (GTK_TREE_MODEL (store), &selected_iter, selected->data)) {
			c_task_list_append (store,
					    &iter,
					    &selected_iter,
					    _("New Task"));
		} else {
			// FIXME: this is not supposed to happen
			c_task_list_append (store, &iter, NULL, _("New Task"));
		}

		g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
		g_list_free    (selected);
	} else {
		c_task_list_append (store, &iter, NULL, _("New Task"));
	}

	path = gtk_tree_model_get_path (gtk_tree_view_get_model (tree),
				        &iter);

	tree_edit_path (tree, path);

	gtk_tree_path_free (path);
}

static void
task_top_activated (GtkAction* action,
		    CWindow  * self)
{
	GtkTreeSelection* selection;
	GtkTreeIter       iter;
	GList           * selected;

	g_return_if_fail (C_IS_WINDOW (self));

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (c_window_get_tree (self)));

	if (1 != gtk_tree_selection_count_selected_rows (selection)) {
		return;
	}

	selected = gtk_tree_selection_get_selected_rows (selection, NULL);
	g_return_if_fail (selected);
	g_return_if_fail (gtk_tree_model_get_iter (gtk_tree_view_get_model (GTK_TREE_VIEW (c_window_get_tree (self))), &iter, selected->data));
	gtk_list_store_move_after (GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (c_window_get_tree (self)))),
				   &iter,
				   NULL);
	g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
	g_list_free    (selected);
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

static gboolean
tree_key_press_event (GtkTreeView* tree,
		      GdkEventKey* event)
{
	switch (event->keyval) {
	case GDK_F2:
		if (gtk_tree_selection_count_selected_rows (gtk_tree_view_get_selection (tree)) == 1) {
			GList* list;

			list = gtk_tree_selection_get_selected_rows (gtk_tree_view_get_selection (tree), NULL);
			tree_edit_path (tree, list->data);
			g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
			g_list_free    (list);

			return TRUE;
		}
		break;
	case GDK_Delete:
		if (gtk_tree_selection_count_selected_rows (gtk_tree_view_get_selection (tree)) > 0) {
			GtkTreeModel* model = NULL;
			GList       * selected = gtk_tree_selection_get_selected_rows (gtk_tree_view_get_selection (tree), &model);
			GList       * iter;

			for (iter = selected; iter; iter = iter->next) {
				GtkTreePath* path = iter->data;
				iter->data = gtk_tree_row_reference_new (model, path);
				gtk_tree_path_free (path);
			}
			for (iter = selected; iter; iter = iter->next) {
				GtkTreeIter titer;
				gtk_tree_model_get_iter (model, &titer,
							 gtk_tree_row_reference_get_path (iter->data));
				gtk_list_store_remove   (GTK_LIST_STORE (model),
						         &titer);
				gtk_tree_row_reference_free (iter->data);
			}

			g_list_free (selected);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

static void
edited_cb (GtkCellRendererText* renderer,
	   gchar* path,
	   gchar* new_text,
	   GtkTreeView        * tree)
{
	GtkTreePath* _path = gtk_tree_path_new_from_string (path);
	GtkTreeIter  iter;
	gtk_tree_model_get_iter (gtk_tree_view_get_model (tree), &iter, _path);
	c_task_list_set_text (C_TASK_LIST (gtk_tree_view_get_model (tree)), &iter, new_text);
	gtk_tree_path_free (_path);
}

static void
task_list_data_func (GtkTreeViewColumn* column,
		     GtkCellRenderer  * renderer,
		     GtkTreeModel     * model,
		     GtkTreeIter      * iter,
		     gpointer           data)
{
	gchar* text = c_task_list_get_text (C_TASK_LIST (model), iter);
	g_object_set (renderer, "text", text, NULL);
	g_free (text);
}

GtkWidget*
c_window_new (void)
{
	return g_object_new (C_TYPE_WINDOW, NULL);
}

static void
tree_size_allocate_after (GtkWidget    * tree_widget,
			  GtkAllocation* allocation)
{
	GtkTreeViewColumn* column;
	GtkTreeView      * view;
	GList            * renderers;
	gint               wrap_width;
	gint               hspace;

	view      = GTK_TREE_VIEW (tree_widget);
	column    = gtk_tree_view_get_column (view, 0);
	renderers = gtk_tree_view_column_get_cell_renderers (column);

	g_object_get (renderers->data, "wrap-width", &wrap_width, NULL);

	gtk_widget_style_get (tree_widget,
			      "horizontal-separator", &hspace,
			      NULL);

	if (allocation->width - 2*hspace != wrap_width) {
		GtkTreeModel* model;
		GtkTreeIter   iter;

		gtk_tree_view_column_set_sizing      (column, GTK_TREE_VIEW_COLUMN_FIXED);
		gtk_tree_view_column_set_fixed_width (column, allocation->width);

		g_object_set (renderers->data,
			      "wrap-width", allocation->width - 2 * hspace,
			      NULL);

		model = gtk_tree_view_get_model (view);

		gtk_tree_model_foreach (model,
					(GtkTreeModelForeachFunc)gtk_tree_model_row_changed,
					NULL);
	}

	g_list_free (renderers);
}

static void
c_window_init (CWindow* self)
{
	GtkActionEntry  entries[] = {
		{"File", NULL, N_("_File")},

		{"FileClose", GTK_STOCK_CLOSE, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (file_close_activated)},

		{"EditPreferences", GTK_STOCK_PREFERENCES, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (open_prefs)},

		{"TaskBottom", GTK_STOCK_GOTO_BOTTOM, N_("To _Bottom"),
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (task_bottom_activated)},
		{"TaskNew", GTK_STOCK_ADD, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (task_new_activated)},
		{"TaskTop", GTK_STOCK_GOTO_TOP, N_("To _Top"),
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (task_top_activated)}
	};
	GtkCellRenderer* renderer;
	GtkActionGroup* group;
	GtkUIManager* ui_manager = gtk_ui_manager_new ();
	GtkWidget   * result = GTK_WIDGET (self); // FIXME: remove this line
	GtkWidget   * swin;
	GtkWidget   * tree;
	GtkWidget   * vbox   = gtk_vbox_new   (FALSE, 0);
	GError      * error = NULL;

	gtk_window_add_accel_group  (GTK_WINDOW (self),
				     gtk_ui_manager_get_accel_group (ui_manager));

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
								"<separator/>"
								"<menuitem action='FileClose' />"
							"</menu>"
						"</menubar>"
						"<toolbar name='toolbar'>"
							"<toolitem action='TaskNew'/>"
							"<separator/>"
							"<toolitem action='TaskTop'/>"
							"<toolitem action='TaskBottom'/>"
						"</toolbar>"
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
	gtk_box_pack_start (GTK_BOX (vbox),
			    gtk_ui_manager_get_widget (ui_manager, "/toolbar"),
			    FALSE,
			    FALSE,
			    0);

	swin = gtk_scrolled_window_new (NULL, NULL);
	//gtk_scrolled_window_set_policy      (GTK_SCROLLED_WINDOW (swin),
	//				     GTK_POLICY_NEVER,
	//				     GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (swin),
					     GTK_SHADOW_IN);
	gtk_widget_show (swin);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), swin);

	tree = gtk_tree_view_new ();
	g_signal_connect (tree, "key-press-event",
			  G_CALLBACK (tree_key_press_event), NULL);
	gtk_tree_view_set_reorderable (GTK_TREE_VIEW (tree),
				       TRUE);
	g_signal_connect (tree, "button-press-event",
			  G_CALLBACK (tree_button_press_event), result);
	g_signal_connect (tree, "size-allocate",
			        G_CALLBACK (tree_size_allocate_after), NULL);
	g_object_set_data_full (G_OBJECT (result),
				"CWindow::TreeView",
				g_object_ref_sink (tree),
				g_object_unref);
	gtk_widget_show (tree);
	gtk_container_add (GTK_CONTAINER (swin), tree);

	renderer = gtk_cell_renderer_text_new ();
	g_object_set     (renderer,
			  "wrap-mode", PANGO_WRAP_WORD_CHAR,
			  NULL);
	g_signal_connect (renderer, "edited",
			  G_CALLBACK (edited_cb), tree);
	gtk_tree_view_insert_column_with_data_func (GTK_TREE_VIEW (tree),
						    -1,
						    _("Task"),
						    renderer,
						    task_list_data_func,
						    NULL, NULL);

	g_object_set_data_full (G_OBJECT (result),
				"CWindow::UIManager",
				ui_manager,
				g_object_unref);

	gtk_widget_show    (vbox);
	gtk_container_add  (GTK_CONTAINER (result),
			    vbox);
}

static void
c_window_class_init (CWindowClass* self_class)
{}

