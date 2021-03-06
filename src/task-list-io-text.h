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

#ifndef TASK_LIST_IO_TEXT_H
#define TASK_LIST_IO_TEXT_H

#include "task-list-io.h"

G_BEGIN_DECLS

typedef struct _CTaskListIOText        CTaskListIOText;
typedef struct _CTaskListIOTextPrivate CTaskListIOTextPrivate;
typedef struct _CTaskListIOTextClass   CTaskListIOTextClass;

#define C_TYPE_TASK_LIST_IO_TEXT         (c_task_list_io_text_get_type ())

GType c_task_list_io_text_get_type (void);

struct _CTaskListIOText {
	CTaskListIO             base_instance;
	CTaskListIOTextPrivate* _private;
};

struct _CTaskListIOTextClass {
	CTaskListIOClass        base_class;
};

G_END_DECLS

#endif /* !TASK_LIST_IO_TEXT_H */
