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

#ifndef TASK_LIST_H
#define TASK_LIST_H

#include <gtk/gtkliststore.h>
#include <gtk/gtktreestore.h>
#include "task.h"

G_BEGIN_DECLS

typedef struct _CTaskList        CTaskList;
typedef struct _CTaskListPrivate CTaskListPrivate;
typedef struct _CTaskListClass   CTaskListClass;

#define C_TYPE_TASK_LIST         (c_task_list_get_type ())
#define C_TASK_LIST(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), C_TYPE_TASK_LIST, CTaskList))
#define C_TASK_LIST_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), C_TYPE_TASK_LIST, CTaskListClass))
#define C_IS_TASK_LIST(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), C_TYPE_TASK_LIST))
#define C_IS_TASK_LIST_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), C_TYPE_TASK_LIST))
#define C_TASK_LIST_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), C_TYPE_TASK_LIST, CTaskListClass))

GType        c_task_list_get_type      (void);

CTaskList*   c_task_list_new           (void);
CTaskList*   c_task_list_new_default   (void);
CTaskList*   c_task_list_new_from_file (gchar const * path,
                                        GError      **error);
void         c_task_list_append        (CTaskList   * self,
					GtkTreeIter * iter,
					GtkTreeIter * before,
					gchar const * text);
void         c_task_list_append_task   (CTaskList   * self,
					GtkTreeIter * iter,
					GtkTreeIter * parent,
					CTask       * task);
CTask*       c_task_list_get_task      (CTaskList   * self,
					GtkTreeIter * iter);
gchar const* c_task_list_get_text      (CTaskList   * self,
					GtkTreeIter * iter);
void         c_task_list_save          (CTaskList   * self,
					gchar const * path);
void         c_task_list_set_text      (CTaskList   * self,
					GtkTreeIter * iter,
					gchar const * text);

struct _CTaskList {
	GtkTreeStore      base_instance;
	CTaskListPrivate* _private;
};

struct _CTaskListClass {
	GtkTreeStoreClass base_class;
};

G_END_DECLS

#endif /* !TASK_LIST_H */
