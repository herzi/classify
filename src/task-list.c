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

#include "task.h"
#include "task-list-io-text.h"
#include "task-list-io-xml.h"

enum {
	COL_TASK,
	N_COLUMNS
};

void
c_task_list_append (CTaskList   * store,
		    GtkTreeIter * iter,
		    GtkTreeIter * before,
		    gchar const * text)
{
	GtkTreeIter  iter2;
	CTask      * task;

	if (before) {
		gtk_list_store_insert_after (store,
					     &iter2,
					     before);
	} else {
		gtk_list_store_append (store, &iter2);
	}

	task = c_task_new  (text);
	gtk_list_store_set (GTK_LIST_STORE (store), &iter2,
			    COL_TASK, task,
			    -1);
	g_object_unref     (task);

	if (iter) {
		*iter = iter2;
	}
}

gchar const*
c_task_list_get_text (CTaskList  * self,
		      GtkTreeIter* iter)
{
	CTask* task = NULL;

	g_return_val_if_fail (C_IS_TASK_LIST (self), NULL);

	gtk_tree_model_get (GTK_TREE_MODEL (self), iter,
			    COL_TASK, &task,
			    -1);

	return c_task_get_text (task);
}

CTaskList*
c_task_list_new (void)
{
	return gtk_list_store_new (N_COLUMNS,
				   G_TYPE_OBJECT);
}

CTaskList*
c_task_list_new_from_file (gchar const* path)
{
	CTaskList* self = c_task_list_new ();
	gchar    * xml_path = g_strdup_printf ("%s.xml", path);

	// FIXME: detect the file type and act accordingly

	if (g_file_test (xml_path, G_FILE_TEST_IS_REGULAR)) {
		task_list_io_xml_load  (self, xml_path);
	} else if (g_file_test (path, G_FILE_TEST_IS_REGULAR)) {
		task_list_io_text_load (self, path);
	}

	g_free (xml_path);

	return self;
}

void
c_task_list_save (CTaskList  * self,
		  gchar const* path)
{
	gchar* xml_path;

	g_return_if_fail (C_IS_TASK_LIST (self));

	xml_path = g_strdup_printf ("%s.xml", path);
	task_list_io_xml_save  (self,
				xml_path);
	g_free (xml_path);
}

void
c_task_list_set_text (CTaskList   * store,
		      GtkTreeIter * iter,
		      gchar const * text)
{
	GtkTreePath* path;
	CTask      * task = NULL;

	gtk_tree_model_get (GTK_TREE_MODEL (store), iter,
			    COL_TASK, &task,
			    -1);

	c_task_set_text (task, text);

	path = gtk_tree_model_get_path (GTK_TREE_MODEL (store),
					iter);
	gtk_tree_model_row_changed     (GTK_TREE_MODEL (store),
					path,
					iter);
	gtk_tree_path_free             (path);
}

