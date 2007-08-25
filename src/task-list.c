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

#include "task-list.h"

#include <glib/gstdio.h>
#include "task-list-io-text.h"

enum {
	COL_TEXT,
	N_COLUMNS
};

void
c_task_list_append (CTaskList   * store,
		    GtkTreeIter * iter,
		    GtkTreeIter * before,
		    gchar const * task)
{
	GtkTreeIter iter2;

	if (before) {
		gtk_list_store_insert_after (store,
					     &iter2,
					     before);
	} else {
		gtk_list_store_append (store, &iter2);
	}
	c_task_list_set_text  (store, &iter2, task);

	if (iter) {
		*iter = iter2;
	}
}

gchar*
c_task_list_get_text (CTaskList  * self,
		      GtkTreeIter* iter)
{
	gchar* result = NULL;

	g_return_val_if_fail (C_IS_TASK_LIST (self), NULL);

	gtk_tree_model_get (GTK_TREE_MODEL (self), iter,
			    COL_TEXT, &result,
			    -1);

	return result;
}

CTaskList*
c_task_list_new (void)
{
	return gtk_list_store_new (N_COLUMNS,
				   G_TYPE_STRING);
}

static void
task_list_io_text_load (CTaskList  * self,
			gchar const* path)
{
	GMappedFile* file = g_mapped_file_new (path,
					       FALSE,
					       NULL);

	if (file) {
		gchar** lines = g_strsplit (g_mapped_file_get_contents (file), "\n", 0);
		gchar** liter;
		for (liter = lines; liter && *liter; liter++) {
			if (!**liter) {
				// empty string
				continue;
			}
			gchar* line = g_strcompress (*liter);
			c_task_list_append (self, NULL, NULL, line);
			g_free (line);
		}
		g_strfreev (lines);
		g_mapped_file_free (file);
	}
}

CTaskList*
c_task_list_new_from_file (gchar const* path)
{
	CTaskList  * self = c_task_list_new ();

	// FIXME: detect the file type and act accordingly

	if (g_file_test (path, G_FILE_TEST_IS_DIR)) {
		// FIXME: parse XML file
	} else {
		task_list_io_text_load (self, path);
	}

	return self;
}

static gboolean
write_node_to_file (GtkTreeModel* model,
		    GtkTreePath * path,
		    GtkTreeIter * iter,
		    gpointer      data)
{
	FILE* file = data;
	gchar* task;
	gchar* line;

	task = c_task_list_get_text (C_TASK_LIST (model), iter);
	line = g_strescape (task, NULL);
	fprintf (file, "%s\n", line);
	g_free (line);
	g_free (task);

	return FALSE;
}

void
c_task_list_save (CTaskList  * self,
		  gchar const* path)
{
	FILE* file;

	g_return_if_fail (C_IS_TASK_LIST (self));

	file = fopen (path, "w");
	gtk_tree_model_foreach (GTK_TREE_MODEL (self),
				write_node_to_file,
				file);
	fclose (file);
}

void
c_task_list_set_text (CTaskList   * store,
		      GtkTreeIter * iter,
		      gchar const * text)
{
	gtk_list_store_set (store, iter,
			    COL_TEXT, text,
			    -1);
}
