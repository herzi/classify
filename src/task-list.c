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

#define GETTEXT_PACKAGE "classify"
#include <glib/gi18n-lib.h>

struct _CTaskListPrivate {
        gchar* path;
        guint  save_timeout;
};

#define PRIV(i) (((CTaskList*)i)->_private)

enum {
	COL_TASK,
	N_COLUMNS
};

static void implement_tree_model (GtkTreeModelIface* iface);
G_DEFINE_TYPE_WITH_CODE (CTaskList, c_task_list, GTK_TYPE_TREE_STORE,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, implement_tree_model));

void
c_task_list_init (CTaskList* self)
{
	GType types[N_COLUMNS] = {
		G_TYPE_OBJECT
	};

	gtk_tree_store_set_column_types (GTK_TREE_STORE (self),
					 N_COLUMNS,
					 types);

	self->_private = G_TYPE_INSTANCE_GET_PRIVATE (self, C_TYPE_TASK_LIST, CTaskListPrivate);
}

static inline void
task_list_save (CTaskList* self)
{
        if (PRIV (self)->path)
          {
            c_task_list_save (self, PRIV (self)->path);
          }
}

static void
task_list_finalize (GObject* object)
{
        CTaskList* self = C_TASK_LIST (object);

        if (PRIV (self)->save_timeout) {
                task_list_save (self);
                g_source_remove (PRIV (self)->save_timeout);
                PRIV (self)->save_timeout = 0;
        }

        g_free (PRIV (self)->path);

        G_OBJECT_CLASS (c_task_list_parent_class)->finalize (object);
}

static void
c_task_list_class_init (CTaskListClass* self_class)
{
	GObjectClass* object_class = G_OBJECT_CLASS (self_class);

	object_class->finalize = task_list_finalize;

	g_type_class_add_private (self_class, sizeof (CTaskListPrivate));
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
        CTaskList  * self;
        GError     * error = NULL;
        gchar const* prefix;
        gchar      * path;

        if (g_getenv ("CLASSIFY_PATH"))
          {
            prefix = g_getenv ("CLASSIFY_PATH");
          }
        else
          {
            prefix = g_get_user_data_dir ();
          }

        path = g_build_filename (prefix,
                                 "classify",
                                 NULL);

        self = c_task_list_new_from_file (path, &error);
        /* FIXME: forward the error */
        if (error)
          {
            g_warning (_("error opening file %s: %s"),
                       path, error->message);
            g_error_free (error);
          }

        g_free (path);
	return self;
}

CTaskList*
c_task_list_new_from_file (gchar const* path,
                           GError     **error)
{
        CTaskList* self;
        guint      i;
        GType      loaders[] = {
                C_TYPE_TASK_LIST_IO_XML,
                C_TYPE_TASK_LIST_IO_TEXT
        };

        g_return_val_if_fail (path && *path, NULL);
        g_return_val_if_fail (!error || !*error, NULL);

        self = c_task_list_new ();
        PRIV (self)->path = g_strdup (path);

        for (i = 0; i < G_N_ELEMENTS (loaders); i++) {
		if (c_task_list_io_test (loaders[i], path)) {
			c_task_list_io_load (loaders[i], self, path);

			if (i != 0) {
				c_task_list_io_remove (loaders[i], path);
				c_task_list_save (self, path);
			}
			break;
		}
	}

        if (i == G_N_ELEMENTS (loaders))
          {
            /* no file found; create some default content */
            GtkTreeIter  iter;
            c_task_list_append (self, &iter, NULL, _("Double-Click or Press F2 to change this into a task"));
            c_task_list_append (self, &iter, &iter, _("Press Ctrl-N to add a new task after the current one"));
            c_task_list_append (self, &iter, &iter, _("Press Ctrl-V to paste something from the clipboard"));
            c_task_list_append (self, &iter, &iter, _("Start typing in this list and you can quickly search for items"));
          }

	/* Special-case the text loader, there have been public revisions that
	 * leave a text file there */
	if (c_task_list_io_test (C_TYPE_TASK_LIST_IO_TEXT, path)) {
		c_task_list_io_remove (C_TYPE_TASK_LIST_IO_TEXT, path);
	}

        if (PRIV (self)->save_timeout)
          {
            /* don't request save after loading a file with content */
            g_source_remove (self->_private->save_timeout);
            self->_private->save_timeout = 0;
          }

        return self;
}

void
c_task_list_save (CTaskList  * self,
		  gchar const* path)
{
	CTaskListIOClass* io_class = g_type_class_ref (C_TYPE_TASK_LIST_IO_XML);
	gchar* xml_path;

	g_return_if_fail (C_IS_TASK_LIST (self));

        /* FIXME: enforce suffix somehow */
        if (!g_str_has_suffix (path, ".xml"))
          {
            xml_path = g_strdup_printf ("%s.xml", path);
          }
        else
          {
            xml_path = g_strdup (path);
          }

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

/* GtkTreeModelIface */
static GtkTreeModelIface* c_task_list_parent_model = NULL;

static gboolean
task_list_save_timeout (gpointer data)
{
	CTaskList* self = C_TASK_LIST (data);

	task_list_save (self);

	self->_private->save_timeout = 0;
	return FALSE;
}

static void
task_list_queue_save (CTaskList* self)
{
	if (G_LIKELY (self->_private->save_timeout)) {
		g_source_remove (self->_private->save_timeout);
	}

#if !GLIB_CHECK_VERSION(2,14,0)
#define g_timeout_add_seconds(sec,callback,data) g_timeout_add(sec*1000,callback,data)
#endif
	self->_private->save_timeout = g_timeout_add_seconds (30, // seconds
							      task_list_save_timeout,
							      self);
}

static void
task_list_row_changed (GtkTreeModel* model,
		       GtkTreePath * path,
		       GtkTreeIter * iter)
{
	if (c_task_list_parent_model->row_changed) {
		c_task_list_parent_model->row_changed (model, path, iter);
	}

	task_list_queue_save (C_TASK_LIST (model));
}

static void
task_list_row_inserted (GtkTreeModel* model,
		        GtkTreePath * path,
		        GtkTreeIter * iter)
{
	if (c_task_list_parent_model->row_inserted) {
		c_task_list_parent_model->row_inserted (model, path, iter);
	}

	task_list_queue_save (C_TASK_LIST (model));
}

static void
task_list_row_has_child_toggled (GtkTreeModel* model,
				 GtkTreePath * path,
				 GtkTreeIter * iter)
{
	if (c_task_list_parent_model->row_has_child_toggled) {
		c_task_list_parent_model->row_has_child_toggled (model, path, iter);
	}

	task_list_queue_save (C_TASK_LIST (model));
}

static void
task_list_row_deleted (GtkTreeModel* model,
		       GtkTreePath * path)
{
	if (c_task_list_parent_model->row_deleted) {
		c_task_list_parent_model->row_deleted (model, path);
	}

	task_list_queue_save (C_TASK_LIST (model));
}

static void
task_list_rows_reordered (GtkTreeModel* model,
		          GtkTreePath * path,
			  GtkTreeIter * iter,
			  gint        * new_order)
{
	if (c_task_list_parent_model->rows_reordered) {
		c_task_list_parent_model->rows_reordered (model, path, iter, new_order);
	}

	task_list_queue_save (C_TASK_LIST (model));
}

static void
implement_tree_model (GtkTreeModelIface* iface)
{
	c_task_list_parent_model = g_type_interface_peek_parent (iface);

	iface->row_changed           = task_list_row_changed;
	iface->row_inserted          = task_list_row_inserted;
	iface->row_has_child_toggled = task_list_row_has_child_toggled;
	iface->row_deleted           = task_list_row_deleted;
	iface->rows_reordered        = task_list_rows_reordered;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
