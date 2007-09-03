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

#include "task-list-io-text.h"

#include <errno.h>
#include <glib/gstdio.h>

G_DEFINE_TYPE (CTaskListIOText, c_task_list_io_text, C_TYPE_TASK_LIST_IO);

static void
c_task_list_io_text_init (CTaskListIOText* self)
{}

static void
task_list_io_text_load (CTaskList  * self,
			gchar const* path)
{
	GMappedFile* file = g_mapped_file_new (path,
					       FALSE,
					       NULL);

	if (file) {
		gchar** lines = g_strsplit (g_mapped_file_get_contents (file), "\n", 0);
		gchar** liter;
		for (liter = lines; liter && *liter; liter++) {
			if (!**liter) {
				// empty string
				continue;
			}
			gchar* line = g_strcompress (*liter);
			c_task_list_append (self, NULL, NULL, line);
			g_free (line);
		}
		g_strfreev (lines);
		g_mapped_file_free (file);

		if (0 != g_remove (path)) {
			g_warning ("couldn't delete the old file: %s",
				   g_strerror (errno));
		}
	}
}

static void
c_task_list_io_text_class_init (CTaskListIOTextClass* self_class)
{
	CTaskListIOClass* io_class = C_TASK_LIST_IO_CLASS (self_class);

	io_class->load = task_list_io_text_load;
}

