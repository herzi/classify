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

#include "task.h"

struct _CTaskPrivate {
	gchar* text;
};

G_DEFINE_TYPE(CTask, c_task, G_TYPE_OBJECT);

static void
c_task_init (CTask* self)
{
	self->_private = G_TYPE_INSTANCE_GET_PRIVATE (self,
						      C_TYPE_TASK,
						      CTaskPrivate);
}

static void
task_finalize (GObject* object)
{
	CTask* self = C_TASK (object);

	c_task_set_text (self, NULL);

	G_OBJECT_CLASS (c_task_parent_class)->finalize (object);
}

static void
c_task_class_init (CTaskClass* self_class)
{
	GObjectClass* object_class = G_OBJECT_CLASS (self_class);

	object_class->finalize = task_finalize;

	g_type_class_add_private (self_class, sizeof (CTaskPrivate));
}

CTask*
c_task_new (gchar const* text)
{
	return g_object_new (C_TYPE_TASK, NULL);
}

gchar const*
c_task_get_text (CTask const* self)
{
	g_return_val_if_fail (C_IS_TASK (self), NULL);

	return self->_private->text;
}

void
c_task_set_text (CTask      * self,
		 gchar const* text)
{
	g_return_if_fail (C_IS_TASK (self));

	if (self->_private->text == text) {
		return;
	}

	g_free (self->_private->text);
	self->_private->text = g_strdup (text);

	// g_object_notify (G_OBJECT (self), "text");
}

