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

#include "task-list-io.h"

G_DEFINE_TYPE (CTaskListIO, c_task_list_io, G_TYPE_OBJECT);

static void
c_task_list_io_init (CTaskListIO* self)
{}

static void
c_task_list_io_class_init (CTaskListIOClass* self)
{}

void
c_task_list_io_load (GType        loader,
		     CTaskList  * task_list,
		     gchar const* path)
{
	CTaskListIOClass* io_class;

	g_return_if_fail (g_type_is_a (loader, C_TYPE_TASK_LIST_IO));

	io_class = g_type_class_ref (loader);
	if (!io_class || !io_class->load) {
		g_warning ("%s cannot load files",
			   g_type_name (loader));
	} else {
		io_class->load (task_list, path);
	}
	g_type_class_unref (io_class);
}

void
c_task_list_io_remove (GType        loader,
		       gchar const* path)
{
	CTaskListIOClass* io_class;

	g_return_if_fail (g_type_is_a (loader, C_TYPE_TASK_LIST_IO));

	io_class = g_type_class_ref (loader);
	if (!io_class || !io_class->remove) {
		g_warning ("%s cannot test",
			   g_type_name (loader));
	} else {
		io_class->remove (path);
	}
	g_type_class_unref (io_class);
}

gboolean
c_task_list_io_test (GType        loader,
		     gchar const* path)
{
	CTaskListIOClass* io_class;
	gboolean          result;

	g_return_val_if_fail (g_type_is_a (loader, C_TYPE_TASK_LIST_IO), FALSE);

	io_class = g_type_class_ref (loader);
	if (!io_class || !io_class->test) {
		g_warning ("%s cannot test",
			   g_type_name (loader));
		result = FALSE;
	} else {
		result = io_class->test (path);
	}
	g_type_class_unref (io_class);

	return result;
}

