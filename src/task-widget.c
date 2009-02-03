/* This file is part of classify
 *
 * AUTHORS
 *     Sven Herzberg  <herzi@lanedo.com>
 *
 * Copyright (C) 2009  Sven Herzberg
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

#include "task-widget.h"

#include "task-list.h"

G_DEFINE_TYPE (CTaskWidget, c_task_widget, GTK_TYPE_TREE_VIEW);

static gboolean
tree_search_equal_func (GtkTreeModel* model,
                        gint          column,
                        gchar const * key,
                        GtkTreeIter * iter,
                        gpointer      search_data)
{
  gchar* text = g_utf8_strdown (c_task_list_get_text (C_TASK_LIST (model), iter), -1);
  gchar* key_ = g_utf8_strdown (key, -1);
  gboolean result = g_str_has_prefix (text, key);
  g_free (key_);
  g_free (text);

  return !result;
}

static void
c_task_widget_init (CTaskWidget* self)
{
  gtk_tree_view_set_reorderable (GTK_TREE_VIEW (self),
                                 TRUE);
  gtk_tree_view_set_rules_hint  (GTK_TREE_VIEW (self),
                                 TRUE);
  gtk_tree_view_set_search_equal_func (GTK_TREE_VIEW (self),
                                       tree_search_equal_func,
                                       NULL, NULL);
  gtk_tree_view_set_search_column     (GTK_TREE_VIEW (self),
                                       0);
}

static void
c_task_widget_class_init (CTaskWidgetClass* self_class)
{}

GtkWidget*
c_task_widget_new (void)
{
  return g_object_new (C_TYPE_TASK_WIDGET,
                       NULL);
}

