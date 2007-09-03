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

#include <gtk/gtktreednd.h>

#include "task.h"
#include "task-list-io-text.h"
#include "task-list-io-xml.h"

enum {
	COL_TASK,
	N_COLUMNS
};

static void implement_drag_dest (GtkTreeDragDestIface* iface);
G_DEFINE_TYPE_WITH_CODE (CTaskList, c_task_list, GTK_TYPE_TREE_STORE,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_DRAG_DEST, implement_drag_dest));

static void
c_task_list_init (CTaskList* self)
{
	GType types[N_COLUMNS] = {
		G_TYPE_OBJECT
	};

	gtk_tree_store_set_column_types (GTK_TREE_STORE (self),
					 N_COLUMNS,
					 types);
}

static void
task_list_finalize (GObject* object)
{
	CTaskList* self = C_TASK_LIST (object);
	gchar    * path = g_build_filename (g_get_home_dir (),
					    ".local",
					    "share",
					    "classify",
					    NULL);
	c_task_list_save (self, path);
	g_free (path);

	G_OBJECT_CLASS (c_task_list_parent_class)->finalize (object);
}

static void
c_task_list_class_init (CTaskListClass* self_class)
{
	GObjectClass* object_class = G_OBJECT_CLASS (self_class);

	object_class->finalize = task_list_finalize;
}

static void
task_list_append_task (CTaskList  * self,
		       GtkTreeIter* iter,
		       GtkTreeIter* parent,
		       GtkTreeIter* before,
		       CTask      * task)
{
	GtkTreeIter  iter2;

	if (!parent && before) {
		gtk_tree_store_insert_after (GTK_TREE_STORE (self),
					     &iter2,
					     NULL,
					     before);
	} else {
		gtk_tree_store_append       (GTK_TREE_STORE (self),
					     &iter2,
					     parent);
	}

	gtk_tree_store_set (GTK_TREE_STORE (self), &iter2,
			    COL_TASK, task,
			    -1);

	if (iter) {
		*iter = iter2;
	}
}

void
c_task_list_append (CTaskList   * store,
		    GtkTreeIter * iter,
		    GtkTreeIter * before,
		    gchar const * text)
{
	CTask      * task;

	g_return_if_fail (C_IS_TASK_LIST (store));

	task = c_task_new     (text);
	task_list_append_task (store,
			       iter,
			       NULL,
			       before,
			       task);
	g_object_unref        (task);
}

void
c_task_list_append_task (CTaskList   * self,
			 GtkTreeIter * iter,
			 GtkTreeIter * parent,
			 CTask       * task)
{
	g_return_if_fail (C_IS_TASK_LIST (self));
	g_return_if_fail (C_IS_TASK (task));

	task_list_append_task (self, iter, parent, NULL, task);
}

CTask*
c_task_list_get_task (CTaskList   * self,
		      GtkTreeIter * iter)
{
	CTask* result = NULL;

	g_return_val_if_fail (C_IS_TASK_LIST (self), NULL);

	gtk_tree_model_get (GTK_TREE_MODEL (self), iter,
			    COL_TASK, &result,
			    -1);

	/* during DND there's not always an object */
	if (result) {
		g_object_unref (result); // remove the reference created by GtkTreeModel::get()
	}

	return result;
}

gchar const*
c_task_list_get_text (CTaskList  * self,
		      GtkTreeIter* iter)
{
	CTask* task;

	g_return_val_if_fail (C_IS_TASK_LIST (self), NULL);

	task = c_task_list_get_task (self, iter);

	if (G_LIKELY (task)) {
		return c_task_get_text (task);
	} else {
		return NULL;
	}
}

CTaskList*
c_task_list_new (void)
{
	return g_object_new (C_TYPE_TASK_LIST, NULL);
}

CTaskList*
c_task_list_new_default (void)
{
	CTaskList* self;
	gchar* path = g_build_filename (g_get_home_dir (),
				 ".local",
				 "share",
				 "classify",
				 NULL);
	gchar* xml_path = g_strdup_printf ("%s.xml", path);

	self = c_task_list_new ();

	// FIXME: detect the file type and act accordingly

	if (g_file_test (xml_path, G_FILE_TEST_IS_REGULAR)) {
		c_task_list_io_load (C_TYPE_TASK_LIST_IO_XML,
				     self,
				     xml_path);
	} else if (g_file_test (path, G_FILE_TEST_IS_REGULAR)) {
		c_task_list_io_load (C_TYPE_TASK_LIST_IO_TEXT,
				     self,
				     path);

		c_task_list_save (self, path);
	}

	if (g_file_test (path, G_FILE_TEST_IS_REGULAR)) {
		g_remove (path);
	}

	g_free (xml_path);
	g_free (path);
	return self;
}

void
c_task_list_save (CTaskList  * self,
		  gchar const* path)
{
	CTaskListIOClass* io_class = g_type_class_ref (C_TYPE_TASK_LIST_IO_XML);
	gchar* xml_path;

	g_return_if_fail (C_IS_TASK_LIST (self));

	xml_path = g_strdup_printf ("%s.xml", path);

	io_class->save (self, xml_path);

	g_free (xml_path);
	g_type_class_unref (io_class);
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

/* GtkTreeDragDest */
static GtkTreeDragDestIface* c_task_list_parent_drag_dest = NULL;

static gboolean
task_list_row_drop_possible (GtkTreeDragDest * drag_dest,
			     GtkTreePath     * path,
			     GtkSelectionData* data)
{
	return gtk_tree_path_get_depth (path) == 1 &&
	       c_task_list_parent_drag_dest->row_drop_possible (drag_dest,
								path,
								data);
}

static void
implement_drag_dest (GtkTreeDragDestIface* iface)
{
	c_task_list_parent_drag_dest = g_type_interface_peek_parent (iface);

	//iface->row_drop_possible = task_list_row_drop_possible;
}

