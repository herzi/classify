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

#ifndef TASK_H
#define TASK_H

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _CTask        CTask;
typedef struct _CTaskPrivate CTaskPrivate;
typedef struct _CTaskClass   CTaskClass;

#define C_TYPE_TASK         (c_task_get_type ())
#define C_TASK(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), C_TYPE_TASK, CTask))
#define C_TASK_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), C_TYPE_TASK, CTaskClass))
#define C_IS_TASK(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), C_TYPE_TASK))
#define C_IS_TASK_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), C_TYPE_TASK))
#define C_TASK_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), C_TYPE_TASK, CTaskClass))

GType        c_task_get_type (void);
CTask*       c_task_new      (gchar const* text);
gchar const* c_task_get_text (CTask const* self);
gchar const* c_task_get_uuid (CTask const* self);
void         c_task_set_text (CTask      * self,
			      gchar const* text);

struct _CTask {
	GObject       base_instance;
	CTaskPrivate* _private;
};

struct _CTaskClass {
	GObjectClass  base_class;
};

G_END_DECLS

#endif /* !TASK_H */
