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

#include <gtk/gtk.h>

#include <glib/gi18n.h>

enum {
	COL_TEXT,
	N_COLUMNS
};

static void
button_clicked_cb (GtkButton  * button,
		   GtkTreeView* tree)
{
	GtkListStore* store = GTK_LIST_STORE (gtk_tree_view_get_model (tree));
	GtkTreePath * path;
	GtkTreeIter   iter;

	gtk_list_store_append (store, &iter);
	gtk_list_store_set    (store, &iter,
			       COL_TEXT, _("new task"),
			       -1);

	path = gtk_tree_model_get_path (gtk_tree_view_get_model (tree),
				        &iter);

	gtk_tree_view_set_cursor (tree,
				  path,
				  gtk_tree_view_get_column (tree, 0),
				  TRUE);

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
	gtk_list_store_set (store, &iter,
			    COL_TEXT, new_text,
			    -1);
	gtk_tree_path_free (_path);
}

static gboolean
write_node_to_file (GtkTreeModel* model,
		    GtkTreePath * path,
		    GtkTreeIter * iter,
		    gpointer      data)
{
	FILE* file = data;
	gchar* task = NULL;
	gchar* line;

	gtk_tree_model_get (model, iter,
			    COL_TEXT, &task,
			    -1);
	line = g_strescape (task, NULL);

	fprintf (file, "%s\n", line);

	g_free (line);
	g_free (task);

	return FALSE;
}

int
main (int   argc,
      char**argv)
{
	GtkCellRenderer* renderer;
	GtkListStore* store;
	GMappedFile * _file;
	GtkTreeIter   iter;
	GtkWidget   * button;
	GtkWidget   * tree;
	GtkWidget   * vbox;
	GtkWidget   * window;
	FILE        * file;

	gtk_init (&argc, &argv);
	window = gtk_window_new     (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (window),
				     400, 300);
	gtk_window_set_title        (GTK_WINDOW (window),
				     _("Classify"));
	g_signal_connect (window, "destroy",
			  G_CALLBACK (gtk_main_quit), NULL);

	store = gtk_list_store_new (N_COLUMNS, G_TYPE_STRING);
	_file = g_mapped_file_new ("/home/herzi/.local/share/classify",
				   FALSE,
				   NULL);

	if (_file) {
		gchar** lines = g_strsplit (g_mapped_file_get_contents (_file), "\n", 0);
		gchar** liter;
		for (liter = lines; liter && *liter; liter++) {
			gchar* line = g_strcompress (*liter);
			gtk_list_store_append (store, &iter);
			gtk_list_store_set    (store, &iter,
					       COL_TEXT, line,
					       -1);
			g_free (line);
		}
		g_strfreev (lines);
		g_mapped_file_free (_file);
	}

	tree = gtk_tree_view_new ();

	vbox = gtk_vbox_new (FALSE, 0);

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
	g_object_set (renderer, "editable", TRUE, NULL);
	g_signal_connect (renderer, "edited",
			  G_CALLBACK (edited_cb), store);
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (tree),
						     -1,
						     _("Task"),
						     renderer,
						     "text", COL_TEXT,
						     NULL);

	gtk_tree_view_set_model (GTK_TREE_VIEW (tree),
				 GTK_TREE_MODEL (store));

	gtk_widget_show   (tree);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), tree);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (window),
			   vbox);

	gtk_widget_show (window);
	gtk_main ();

	file = fopen ("/home/herzi/.local/share/classify", "w");
	gtk_tree_model_foreach (GTK_TREE_MODEL (store),
				write_node_to_file,
				file);
	fclose (file);

	g_object_unref (store);
	return 0;
}

