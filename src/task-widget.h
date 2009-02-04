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

#ifndef TASK_WIDGET_H
#define TASK_WIDGET_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _CTaskWidget        CTaskWidget;
typedef struct _CTaskWidgetPrivate CTaskWidgetPrivate;
typedef struct _CTaskWidgetClass   CTaskWidgetClass;

#define C_TYPE_TASK_WIDGET         (c_task_widget_get_type ())
#define C_TASK_WIDGET(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), C_TYPE_TASK_WIDGET, CTaskWidget))
#define C_TASK_WIDGET_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), C_TYPE_TASK_WIDGET, CTaskWidgetClass))
#define C_IS_TASK_WIDGET(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), C_TYPE_TASK_WIDGET))
#define C_IS_TASK_WIDGET_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), C_TYPE_TASK_WIDGET))
#define C_TASK_WIDGET_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), C_TYPE_TASK_WIDGET, CTaskWidgetClass))

GType      c_task_widget_get_type         (void);
GtkWidget* c_task_widget_new              (void);
void       c_task_widget_copy_clipboard   (CTaskWidget* self);
void       c_task_widget_create_task      (CTaskWidget* self);
void       c_task_widget_delete_selected  (CTaskWidget* self);
void       c_task_widget_edit_path        (CTaskWidget* self,
                                           GtkTreePath* path);
void       c_task_widget_move_bottom      (CTaskWidget* self);
void       c_task_widget_move_top         (CTaskWidget* self);
void       c_task_widget_paste_clipboard  (CTaskWidget* self);
void       c_task_widget_rename_selection (CTaskWidget* self);

struct _CTaskWidget {
  GtkTreeView         base_instance;
  CTaskWidgetPrivate* _private;
};

struct _CTaskWidgetClass {
  GtkTreeViewClass    base_class;
};

G_END_DECLS

#endif /* !TASK_WIDGET_H */
