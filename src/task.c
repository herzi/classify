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

#include "uuid.h"

struct _CTaskPrivate {
	gchar* text;
	gchar* uuid;
};

enum {
	PROP_0,
	PROP_TEXT,
	PROP_UUID
};

G_DEFINE_TYPE(CTask, c_task, G_TYPE_OBJECT);

static void
c_task_init (CTask* self)
{
	self->_private = G_TYPE_INSTANCE_GET_PRIVATE (self,
						      C_TYPE_TASK,
						      CTaskPrivate);
}

static GObject*
task_constructor (GType                  type,
		  guint                  n_params,
		  GObjectConstructParam* params)
{
	GObject* result = G_OBJECT_CLASS(c_task_parent_class)->constructor (type,
									    n_params,
									    params);
	CTask  * self   = C_TASK (result);

	return result;
}

static void
task_finalize (GObject* object)
{
	CTask* self = C_TASK (object);

	c_task_set_text (self, NULL);
	g_free (self->_private->uuid);
	self->_private->uuid = NULL;

	G_OBJECT_CLASS (c_task_parent_class)->finalize (object);
}

static void
task_get_property (GObject   * object,
		   guint       prop_id,
		   GValue    * value,
		   GParamSpec* pspec)
{
	CTask* self = C_TASK (object);

	switch (prop_id) {
	case PROP_TEXT:
		g_value_set_string (value,
				    c_task_get_text (self));
		break;
	case PROP_UUID:
		g_value_set_string (value,
				    c_task_get_uuid (self));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object,
						   prop_id,
						   pspec);
		break;
	}
}

static void
task_set_property (GObject     * object,
		   guint         prop_id,
		   GValue const* value,
		   GParamSpec  * pspec)
{
	CTask* self = C_TASK (object);

	switch (prop_id) {
	case PROP_TEXT:
		c_task_set_text (self,
				 g_value_get_string (value));
		break;
	case PROP_UUID:
		g_free (self->_private->uuid);
		if (g_value_get_string (value)) {
			self->_private->uuid = g_value_dup_string (value);
		} else {
			self->_private->uuid = uuid_new ();
		}
		g_object_notify (object, "uuid");
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object,
						   prop_id,
						   pspec);
		break;
	}
}

static void
c_task_class_init (CTaskClass* self_class)
{
	GObjectClass* object_class = G_OBJECT_CLASS (self_class);

	object_class->constructor  = task_constructor;
	object_class->finalize     = task_finalize;
	object_class->get_property = task_get_property;
	object_class->set_property = task_set_property;

	g_object_class_install_property (object_class,
					 PROP_TEXT,
					 g_param_spec_string ("text",
							      "text",
							      "text",
							      NULL,
							      G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_UUID,
					 g_param_spec_string ("uuid",
							      "uuid",
							      "uuid",
							      NULL,
							      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_type_class_add_private (self_class, sizeof (CTaskPrivate));
}

CTask*
c_task_new (gchar const* text)
{
	return g_object_new (C_TYPE_TASK,
			     "text", text,
			     NULL);
}

CTask*
c_task_new_with_uuid (gchar const* uuid)
{
	return g_object_new (C_TYPE_TASK,
			     "uuid", uuid,
			     NULL);
}

gchar const*
c_task_get_text (CTask const* self)
{
	g_return_val_if_fail (C_IS_TASK (self), NULL);

	return self->_private->text;
}

gchar const*
c_task_get_uuid (CTask const* self)
{
	g_return_val_if_fail (C_IS_TASK (self), NULL);

	return self->_private->uuid;
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

