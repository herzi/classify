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

#include <string.h>
#include <gdk/gdkkeysyms.h>

#include "task-list.h"

/* FIXME: define this properly for the library */
#define GETTEXT_PACKAGE NULL
#include <glib/gi18n-lib.h>

struct _CTaskWidgetPrivate {
  CTaskList* list;
};

#define PRIV(i) (((CTaskWidget*)(i))->_private)

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

static gboolean
entry_key_press_event (GtkWidget  * widget,
		       GdkEventKey* event,
		       gpointer     user_data)
{
	if ((event->state & GDK_SHIFT_MASK) != 0 &&
	    (event->keyval == GDK_Return || event->keyval == GDK_KP_Enter))
	{
		gint cursor, select;
		g_object_get (widget,
			      "cursor-position", &cursor,
			      "selection-bound", &select,
			      NULL);

		if (cursor != select) {
			gtk_editable_delete_text (GTK_EDITABLE (widget),
						  MIN (cursor, select),
						  MAX (cursor, select));
		}

		g_signal_emit_by_name (widget, "insert-at-cursor",
				       "\n");

		return TRUE;
	}

	return FALSE;
}

static void
editing_started_cb (GtkCellRenderer* renderer,
		    GtkCellEditable* editable,
		    gchar const    * path,
		    gpointer         user_data)
{
  if (GTK_IS_ENTRY (editable))
    {
#ifdef HAVE_HILDON
      /* FIXME: eeek! this should not be hildon-agnostic */
      hildon_gtk_im_context_show (GTK_ENTRY (editable)->im_context);
#endif
      g_signal_connect (editable, "key-press-event",
                        G_CALLBACK (entry_key_press_event), NULL);
    }
}

static void
edited_cb (GtkCellRendererText* renderer,
	   gchar* path,
	   gchar* new_text,
	   GtkTreeView        * tree)
{
	GtkTreePath* _path = gtk_tree_path_new_from_string (path);
	GtkTreeIter  iter;
	gtk_tree_model_get_iter (gtk_tree_view_get_model (tree), &iter, _path);
	c_task_list_set_text (C_TASK_LIST (gtk_tree_view_get_model (tree)), &iter, new_text);
	gtk_tree_path_free (_path);
}

static void
task_list_data_func (GtkTreeViewColumn* column,
		     GtkCellRenderer  * renderer,
		     GtkTreeModel     * model,
		     GtkTreeIter      * iter,
		     gpointer           data)
{
	g_object_set (renderer,
		      "text", c_task_list_get_text (C_TASK_LIST (model), iter),
		      NULL);
}

static void
c_task_widget_init (CTaskWidget* self)
{
	GtkCellRenderer* renderer;

  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, C_TYPE_TASK_WIDGET, CTaskWidgetPrivate);

  gtk_tree_view_set_reorderable (GTK_TREE_VIEW (self),
                                 TRUE);
  gtk_tree_view_set_rules_hint  (GTK_TREE_VIEW (self),
                                 TRUE);
  gtk_tree_view_set_search_equal_func (GTK_TREE_VIEW (self),
                                       tree_search_equal_func,
                                       NULL, NULL);
  gtk_tree_view_set_search_column     (GTK_TREE_VIEW (self),
                                       0);

	renderer = gtk_cell_renderer_text_new ();
	g_object_set     (renderer,
			  "wrap-mode", PANGO_WRAP_WORD_CHAR,
			  NULL);
	g_signal_connect (renderer, "editing-started",
			  G_CALLBACK (editing_started_cb), NULL);
	g_signal_connect (renderer, "edited",
			  G_CALLBACK (edited_cb), self);
	gtk_tree_view_insert_column_with_data_func (GTK_TREE_VIEW (self),
						    -1,
						    _("Task"),
						    renderer,
						    task_list_data_func,
						    NULL, NULL);
}

static void
row_has_child_toggled_cb (GtkTreeModel* model,
                          GtkTreePath * path,
                          GtkTreeIter * iter,
                          GtkTreeView * view)
{
  if (gtk_tree_model_iter_has_child (model, iter))
    {
      gtk_tree_view_expand_row (view, path, TRUE);
    }
}

static void
task_widget_notify (GObject   * object,
                    GParamSpec* pspec)
{
  if (G_UNLIKELY (!strcmp (pspec->name, "model")))
    {
      if (PRIV (object)->list)
        {
          g_signal_handlers_disconnect_by_func (PRIV (object)->list, row_has_child_toggled_cb, object);
          g_object_unref (PRIV (object)->list);
        }

      PRIV (object)->list = C_TASK_LIST (gtk_tree_view_get_model (GTK_TREE_VIEW (object)));

      if (PRIV (object)->list)
        {
          g_object_ref (PRIV (object)->list);
          g_signal_connect_after   (PRIV (object)->list, "row-has-child-toggled",
                                    G_CALLBACK (row_has_child_toggled_cb), object);
          gtk_tree_view_expand_all (GTK_TREE_VIEW (object));
        }
    }

  if (G_OBJECT_CLASS (c_task_widget_parent_class)->notify)
    {
      G_OBJECT_CLASS (c_task_widget_parent_class)->notify (object, pspec);
    }
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
  GObjectClass  * object_class = G_OBJECT_CLASS (self_class);
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (self_class);

  object_class->notify             = task_widget_notify;

  /* size management */
  widget_class->size_allocate      = task_widget_size_allocate;

  /* event handling */
  widget_class->button_press_event = task_widget_button_press_event;

  g_type_class_add_private (self_class, sizeof (CTaskWidgetPrivate));
}

void
c_task_widget_copy_clipboard (CTaskWidget* self)
{
	GtkTreeSelection* selection;
	GtkTreeModel    * model;
	GtkTreeView     * view;
	GtkTreeIter       iter;
	GList           * selected;

        /* FIXME: I think it reduces the code size to have task_widget_get_selected_path() */
	view      = GTK_TREE_VIEW (self);
	selection = gtk_tree_view_get_selection (view);

	if (1 != gtk_tree_selection_count_selected_rows (selection)) {
		return;
	}

	selected = gtk_tree_selection_get_selected_rows (selection, &model);
	gtk_tree_model_get_iter (model, &iter, selected->data);

	gtk_clipboard_set_text (gtk_clipboard_get_for_display (gtk_widget_get_display (GTK_WIDGET (self)),
							       GDK_SELECTION_CLIPBOARD),
				c_task_list_get_text (C_TASK_LIST (gtk_tree_view_get_model (view)),
						      &iter),
				-1);

	g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
	g_list_free    (selected);
}

void
c_task_widget_create_task (CTaskWidget* self)
{
	GtkTreeSelection* selection;
	GtkTreeView * tree  = GTK_TREE_VIEW  (self);
	CTaskList   * store = PRIV (self)->list;
	GtkTreePath * path;
	GtkTreeIter   iter;

	selection = gtk_tree_view_get_selection (tree);

	if (gtk_tree_selection_count_selected_rows (selection) == 1) {
		GtkTreeIter  selected_iter;
		GList      * selected = gtk_tree_selection_get_selected_rows (selection, NULL);

		if (selected && gtk_tree_model_get_iter (GTK_TREE_MODEL (store), &selected_iter, selected->data)) {
			c_task_list_append (store,
					    &iter,
					    &selected_iter,
					    _("New Task"));
		} else {
			// FIXME: this is not supposed to happen
			c_task_list_append (store, &iter, NULL, _("New Task"));
		}

		g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
		g_list_free    (selected);
	} else {
		c_task_list_append (store, &iter, NULL, _("New Task"));
	}

	path = gtk_tree_model_get_path (gtk_tree_view_get_model (tree),
                                        &iter);

        c_task_widget_edit_path (C_TASK_WIDGET (tree), path);

        gtk_tree_path_free (path);
}

void
c_task_widget_delete_selected (CTaskWidget* self)
{
	GtkTreeModel* model = NULL;
	GtkTreeView * tree = GTK_TREE_VIEW (self);
	GList       * selected;
	GList       * iter;

	if (gtk_tree_selection_count_selected_rows (gtk_tree_view_get_selection (tree)) == 0) {
		return;
	}

	selected = gtk_tree_selection_get_selected_rows (gtk_tree_view_get_selection (tree), &model);

	for (iter = selected; iter; iter = iter->next) {
		GtkTreePath* path = iter->data;
		iter->data = gtk_tree_row_reference_new (model, path);
		gtk_tree_path_free (path);
	}
	for (iter = selected; iter; iter = iter->next) {
		GtkTreeIter titer;
		gtk_tree_model_get_iter (model, &titer,
					 gtk_tree_row_reference_get_path (iter->data));
		gtk_tree_store_remove   (GTK_TREE_STORE (model),
					 &titer);
		gtk_tree_row_reference_free (iter->data);
	}

	g_list_free (selected);
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

void
c_task_widget_move_bottom (CTaskWidget* self)
{
	GtkTreeSelection* selection;
	GtkTreeIter       iter;
	GList           * selected;

	g_return_if_fail (C_IS_TASK_WIDGET (self));

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (self));

	if (1 != gtk_tree_selection_count_selected_rows (selection)) {
		return;
	}

	selected = gtk_tree_selection_get_selected_rows (selection, NULL);
	g_return_if_fail (selected);
	g_return_if_fail (gtk_tree_model_get_iter (gtk_tree_view_get_model (GTK_TREE_VIEW (self)), &iter, selected->data));
	gtk_tree_store_move_before (GTK_TREE_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (self))),
				    &iter,
				    NULL);
	g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
	g_list_free    (selected);
}

GtkWidget*
c_task_widget_new (void)
{
  return g_object_new (C_TYPE_TASK_WIDGET,
                       NULL);
}

static void
clipboard_text_received_cb (GtkClipboard* clipboard,
			    gchar const * text,
			    gpointer      data)
{
	GtkTreeSelection* selection;
	GtkTreeModel    * model = NULL;
	GtkTreeView     * view;
	GtkTreePath     * path;
	GtkTreeIter       iter;
	GtkTreeIter       sibling;
        gboolean          has_sibling;
        GList           * columns;

        view        = GTK_TREE_VIEW (data);
	selection   = gtk_tree_view_get_selection (view);
	has_sibling = 1 == gtk_tree_selection_count_selected_rows (selection);

	if (has_sibling) {
		GList* selected = gtk_tree_selection_get_selected_rows (selection, &model);
		gtk_tree_model_get_iter (model, &sibling, selected->data);
		g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
		g_list_free    (selected);
	} else {
		model = gtk_tree_view_get_model (view);
	}

	c_task_list_append (C_TASK_LIST (model),
			    &iter,
			    has_sibling ? &sibling : NULL,
			    text);

        path = gtk_tree_model_get_path (model, &iter);
        columns = gtk_tree_view_get_columns (view);
        gtk_tree_view_set_cursor (view,
                                  path,
                                  g_list_last (columns)->data,
                                  FALSE);
        g_list_free (columns);
        gtk_tree_path_free (path);
}

void
c_task_widget_paste_clipboard (CTaskWidget* self)
{
	GtkClipboard* clipboard = gtk_clipboard_get_for_display (gtk_widget_get_display (GTK_WIDGET (self)),
								 GDK_SELECTION_CLIPBOARD);

	gtk_clipboard_request_text (clipboard,
				    clipboard_text_received_cb,
				    self);
}

void
c_task_widget_rename_selection (CTaskWidget* self)
{
        GtkTreeView* view = GTK_TREE_VIEW (self);
	GList* list;

	if (gtk_tree_selection_count_selected_rows (gtk_tree_view_get_selection (view)) != 1) {
		return;
	}

        list = gtk_tree_selection_get_selected_rows (gtk_tree_view_get_selection (view), NULL);
        c_task_widget_edit_path (C_TASK_WIDGET (view), list->data);
        g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
	g_list_free    (list);
}

