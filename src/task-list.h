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

G_BEGIN_DECLS

#define CTaskList         GtkListStore
#define C_TASK_LIST(i)    GTK_LIST_STORE(i)
#define C_IS_TASK_LIST(i) GTK_IS_LIST_STORE(i)

CTaskList* c_task_list_new      (void);
void       c_task_list_append   (CTaskList   * self,
				 gchar const * text);
gchar*     c_task_list_get_text (CTaskList   * self,
				 GtkTreeIter * iter);
void       c_task_list_set_text (CTaskList   * self,
				 GtkTreeIter * iter,
				 gchar const * text);

G_END_DECLS

#endif /* !TASK_LIST_H */
