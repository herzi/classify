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

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "task-list.h"
#include "window.h"

#include <glib/gi18n.h>

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
button_clicked_cb (GtkButton  * button,
		   GtkTreeView* tree)
{
	GtkListStore* store = GTK_LIST_STORE (gtk_tree_view_get_model (tree));
	GtkTreePath * path;
	GtkTreeIter   iter;

	c_task_list_append (store, &iter, _("New Task"));

	path = gtk_tree_model_get_path (gtk_tree_view_get_model (tree),
				        &iter);

	tree_edit_path (tree, path);

	gtk_tree_path_free (path);
}

static void
edited_cb (GtkCellRendererText* renderer,
	   gchar* path,
	   gchar* new_text,
	   GtkListStore* store)
{
	GtkTreePath* _path = gtk_tree_path_new_from_string (path);
	GtkTreeIter  iter;
	gtk_tree_model_get_iter (GTK_TREE_MODEL (store), &iter, _path);
	c_task_list_set_text (store, &iter, new_text);
	gtk_tree_path_free (_path);
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

int
main (int   argc,
      char**argv)
{
	GtkCellRenderer* renderer;
	GtkListStore* store;
	GtkTreeIter   iter;
	GtkWidget   * button;
	GtkWidget   * swin;
	GtkWidget   * tree;
	GtkWidget   * vbox;
	GtkWidget   * window;
	gchar       * path;

	gtk_init (&argc, &argv);
	window = c_window_new ();

	path = g_build_filename (g_get_home_dir (),
				 ".local",
				 "share",
				 "classify",
				 NULL);
	store = c_task_list_new_from_file (path);

	tree = c_window_get_tree (C_WINDOW (window));
	gtk_tree_view_set_reorderable (GTK_TREE_VIEW (tree),
				       TRUE);
	g_signal_connect (tree, "key-press-event",
			  G_CALLBACK (tree_key_press_event), NULL);

	vbox = c_window_get_vbox (C_WINDOW (window));

	button = gtk_button_new_from_stock (GTK_STOCK_ADD);
	g_signal_connect (button, "clicked",
			  G_CALLBACK (button_clicked_cb), tree);
	gtk_widget_show (button);
	gtk_box_pack_start (GTK_BOX (vbox),
			    button,
			    FALSE,
			    FALSE,
			    0);

	renderer = gtk_cell_renderer_text_new ();
	g_signal_connect (renderer, "edited",
			  G_CALLBACK (edited_cb), store);
	gtk_tree_view_insert_column_with_data_func (GTK_TREE_VIEW (tree),
						    -1,
						    _("Task"),
						    renderer,
						    task_list_data_func,
						    NULL, NULL);

	gtk_tree_view_set_model (GTK_TREE_VIEW (tree),
				 GTK_TREE_MODEL (store));

	gtk_widget_show (tree);
	swin = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (swin),
					     GTK_SHADOW_IN);
	gtk_container_add (GTK_CONTAINER (swin), tree);
	gtk_widget_show (swin);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), swin);
	gtk_widget_show (vbox);

	gtk_widget_show (window);
	gtk_main ();

	c_task_list_save (store, path);
	g_free (path);

	g_object_unref (store);
	return 0;
}

