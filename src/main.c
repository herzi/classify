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
#include "task-list.h"
#include "window.h"

#include <glib/gi18n.h>

int
main (int   argc,
      char**argv)
{
	CTaskList* store;
	GtkWidget* window;
	gchar    * path;

	gtk_init (&argc, &argv);
	window = c_window_new ();

	path = g_build_filename (g_get_home_dir (),
				 ".local",
				 "share",
				 "classify",
				 NULL);
	store = c_task_list_new_from_file (path);

	gtk_tree_view_set_model (GTK_TREE_VIEW (c_window_get_tree (C_WINDOW (window))),
				 GTK_TREE_MODEL (store));

	gtk_widget_show (window);
	gtk_main ();

	c_task_list_save (store, path);
	g_free (path);

	g_object_unref (store);
	return 0;
}

