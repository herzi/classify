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

#include "task-list-io-text.h"

#include <errno.h>
#include <glib/gstdio.h>

void
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

		if (0 != g_remove (path)) {
			g_warning ("couldn't delete the old file: %s",
				   g_strerror (errno));
		}
	}
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
task_list_io_text_save (CTaskList  * self,
			gchar const* path)
{
	FILE* file;

	file = fopen (path, "w");
	gtk_tree_model_foreach (GTK_TREE_MODEL (self),
				write_node_to_file,
				file);
	fclose (file);
}

