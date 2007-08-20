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

enum {
	COL_TEXT,
	N_COLUMNS
};

void
c_task_list_append (CTaskList  * store,
		    gchar const* task)
{
	GtkTreeIter iter;

	gtk_list_store_append (store, &iter);
	c_task_list_set_text  (store, &iter, task);
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

CTaskList*
c_task_list_new_from_file (gchar const* path)
{
	GMappedFile* file;
	CTaskList  * self = c_task_list_new ();

	file = g_mapped_file_new (path,
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
			c_task_list_append (self, line);
			g_free (line);
		}
		g_strfreev (lines);
		g_mapped_file_free (file);
	}

	return self;
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
