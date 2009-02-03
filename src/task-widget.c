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

static gboolean
all_changed (GtkTreeModel* model,
	     GtkTreePath * path,
	     GtkTreeIter * iter,
	     gpointer      data)
{
	gtk_tree_model_row_changed (model, path, iter);
	return FALSE;
}

static void
task_widget_size_allocate (GtkWidget    * widget,
                           GtkAllocation* allocation)
{
	GtkTreeViewColumn* column;
	GtkTreeView      * view;
	GList            * renderers;
	gint               wrap_width;
	gint               focus;
	gint               hspace;
	gint               expander;
	gint               text_pad;
	gint               target_width;

        GTK_WIDGET_CLASS (c_task_widget_parent_class)->size_allocate (widget, allocation);

	view      = GTK_TREE_VIEW (widget);
	column    = gtk_tree_view_get_column (view, 0);
	renderers = gtk_tree_view_column_get_cell_renderers (column);

	g_object_get (renderers->data,
		      "xpad",       &text_pad,
		      "wrap-width", &wrap_width,
		      NULL);

	gtk_widget_style_get (widget,
			      "expander-size",        &expander,
			      "focus-padding",        &focus,
			      "horizontal-separator", &hspace,
			      NULL);

	target_width = allocation->width - 2 * (hspace + focus + text_pad);

	if ((gtk_tree_model_get_flags (gtk_tree_view_get_model (view)) & GTK_TREE_MODEL_LIST_ONLY) == 0) {
		if (column == gtk_tree_view_get_expander_column (view)) {
			target_width -= expander + 4 /* EXPANDER_EXTRA_PADDING */;
		}
	}

	target_width = MAX (target_width, 0);

	if (target_width != wrap_width) {
		GtkTreeModel* model;

		gtk_tree_view_column_set_sizing      (column, GTK_TREE_VIEW_COLUMN_FIXED);
		gtk_tree_view_column_set_fixed_width (column, allocation->width);

		g_object_set (renderers->data,
			      "wrap-width", target_width,
			      NULL);

		model = gtk_tree_view_get_model (view);

		gtk_tree_model_foreach (model,
					all_changed,
					NULL);
	}

	g_list_free (renderers);
}

static gboolean
task_widget_button_press_event (GtkWidget     * widget,
                                GdkEventButton* event)
{
        if (event->button == 1 && event->type == GDK_2BUTTON_PRESS) {
                GtkTreeSelection* selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
                if (gtk_tree_selection_count_selected_rows (selection) == 1) {
                        GList* selected = gtk_tree_selection_get_selected_rows (selection, NULL);

                        c_task_widget_edit_path (C_TASK_WIDGET (widget), selected->data);

                        g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
                        g_list_free    (selected);

                        return TRUE;
                }
        }

        if (GTK_WIDGET_CLASS (c_task_widget_parent_class)->button_press_event)
          {
            return GTK_WIDGET_CLASS (c_task_widget_parent_class)->button_press_event (widget, event);
          }

        return FALSE;
}

static void
c_task_widget_class_init (CTaskWidgetClass* self_class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (self_class);

  /* size management */
  widget_class->size_allocate      = task_widget_size_allocate;

  /* event handling */
  widget_class->button_press_event = task_widget_button_press_event;
}

static int
renderer_is_text (gconstpointer a,
                  gconstpointer b)
{
  if (GTK_IS_CELL_RENDERER_TEXT (a))
    return 0;

  return 1;
}

void
c_task_widget_edit_path (CTaskWidget* self,
                         GtkTreePath* path)
{
  GtkCellRenderer* renderer;
  GList          * columns;
  GList          * renderers;

  g_return_if_fail (C_IS_TASK_WIDGET (self));
  g_return_if_fail (path && gtk_tree_path_get_depth (path) > 0);

  columns   = gtk_tree_view_get_columns (GTK_TREE_VIEW (self));
  renderers = gtk_tree_view_column_get_cell_renderers (g_list_last (columns)->data);
  renderer = g_list_find_custom (renderers,
                                 NULL,
                                 renderer_is_text)->data;
  g_object_set (renderer, "editable", TRUE, NULL);
  gtk_tree_view_set_cursor_on_cell (GTK_TREE_VIEW (self),
                                    path,
                                    g_list_last (columns)->data,
                                    renderer,
                                    TRUE);
  g_object_set (renderer, "editable", FALSE, NULL);
  g_list_free (renderers);
  g_list_free (columns);
}

GtkWidget*
c_task_widget_new (void)
{
  return g_object_new (C_TYPE_TASK_WIDGET,
                       NULL);
}

