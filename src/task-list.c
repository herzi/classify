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

enum {
	COL_TEXT,
	N_COLUMNS
};

#warning "FIXME: request a CTaskList, not a GtkListStore"
void
c_task_list_append (GtkListStore* store,
		    gchar const * task)
{
	GtkTreeIter iter;

	gtk_list_store_append (store, &iter);
	gtk_list_store_set    (store, &iter,
			       COL_TEXT, task,
			       -1);
}

#warning "FIXME: return CTaskList"
GtkListStore*
c_task_list_new (void)
{
	return gtk_list_store_new (N_COLUMNS,
				   G_TYPE_STRING);
}

