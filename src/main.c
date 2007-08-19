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

int
main (int   argc,
      char**argv)
{
	GtkListStore* store;
	GtkWidget   * tree;
	GtkWidget   * window;

	gtk_init (&argc, &argv);
	window = gtk_window_new     (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (window),
				     400, 300);
	gtk_window_set_title        (GTK_WINDOW (window),
				     _("Classify"));
	g_signal_connect (window, "destroy",
			  G_CALLBACK (gtk_main_quit), NULL);

	tree = gtk_tree_view_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (tree),
						     -1,
						     _("Task"),
						     gtk_cell_renderer_text_new (),
						     "text", COL_TEXT,
						     NULL);

	store = gtk_list_store_new (N_COLUMNS, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (tree),
				 GTK_TREE_MODEL (store));
	g_object_unref (store);

	gtk_widget_show   (tree);
	gtk_container_add (GTK_CONTAINER (window),
			   tree);

	gtk_widget_show (window);
	gtk_main ();
	return 0;
}

