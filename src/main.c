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
	GtkWidget   * tree;
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

	g_signal_connect (tree, "key-press-event",
			  G_CALLBACK (tree_key_press_event), NULL);

	g_signal_connect (c_window_get_button (C_WINDOW (window)), "clicked",
			  G_CALLBACK (button_clicked_cb), tree);

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

	gtk_widget_show (window);
	gtk_main ();

	c_task_list_save (store, path);
	g_free (path);

	g_object_unref (store);
	return 0;
}

