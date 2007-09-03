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

#ifndef TASK_LIST_IO_H
#define TASK_LIST_IO_H

#include "task-list.h"

G_BEGIN_DECLS

typedef struct _CTaskListIO        CTaskListIO;
typedef struct _CTaskListIOPrivate CTaskListIOPrivate;
typedef struct _CTaskListIOClass   CTaskListIOClass;

#define C_TYPE_TASK_LIST_IO         (c_task_list_io_get_type ())
#define C_TASK_LIST_IO_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), C_TYPE_TASK_LIST_IO, CTaskListIOClass))

GType c_task_list_io_get_type (void);

void     c_task_list_io_remove (GType        loader,
				gchar const* path);
void     c_task_list_io_load   (GType        loader,
				CTaskList  * task_list,
				gchar const* path);
gboolean c_task_list_io_test   (GType        loader,
				gchar const* path);

struct _CTaskListIO {
	GObject             object;
	CTaskListIOPrivate* _private;
};

struct _CTaskListIOClass {
	GObjectClass        object_class;

	/* vtable */
	void     (*remove) (gchar const* path);
	void     (*load)   (CTaskList  * task_list,
			    gchar const* path);
	void     (*save)   (CTaskList  * task_list,
			    gchar const* path);
	gboolean (*test)   (gchar const* path);
};

G_END_DECLS

#endif /* !TASK_LIST_IO_H */
