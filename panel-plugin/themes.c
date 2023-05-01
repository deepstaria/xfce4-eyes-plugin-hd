/* Copyright (C) 1999 Dave Camp <dave@davec.dhs.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>

#include <gtk/gtk.h>

#include "eyes.h"
#include "themes.h"

gchar *theme_directories[] = {
    THEMESDIR
};
#define NUM_THEME_DIRECTORIES 1

static void
parse_theme_file (EyesPlugin *eyes,
                  FILE       *theme_file)
{
    gchar line_buf [512]; /* prolly overkill */
    gchar *token;
    fgets (line_buf, 512, theme_file);

    /* initialize optional theme settings */
    if (eyes->eye_overlay_filename != NULL) {
        g_free (eyes->eye_overlay_filename);
        eyes->eye_overlay_filename = NULL;
    }
    if (eyes->eye_overlay_image != NULL) {
        g_object_unref (eyes->eye_overlay_image);
        eyes->eye_overlay_image = NULL;
    }

    if (eyes->eye_mask_filename != NULL) {
        g_free (eyes->eye_mask_filename);
        eyes->eye_mask_filename = NULL;
    }
    if (eyes->eye_mask_image != NULL) {
        g_object_unref (eyes->eye_mask_image);
        eyes->eye_mask_image = NULL;
    }

    eyes->eye_frame_height = 0;
    eyes->eye_frame_width = 0;
    eyes->eye_frames = 0;
    eyes->eye_variations = 0;
    eyes->pupil_height = 0;
    eyes->pupil_width = 0;
    eyes->pupil_frame_height = 0;
    eyes->pupil_frame_width = 0;
    eyes->pupil_frames = 0;
    eyes->pupil_variations = 0;
    eyes->blinks_on_click = 1;
    eyes->curr_blink_frame = 0;
    eyes->curr_pupil_frame = 0;
    eyes->scale_to_panel = 0;


    while (!feof (theme_file)) {
        token = strtok (line_buf, "=");
        if (strncmp (token, "wall-thickness",
                 strlen ("wall-thickness")) == 0)
        {
            token += strlen ("wall-thickness");
            while (!isdigit (*token))
            {
                token++;
            }
            sscanf (token, "%d", &eyes->wall_thickness);
        }
        else if (strncmp (token, "num-eyes", strlen ("num-eyes")) == 0)
        {
            token += strlen ("num-eyes");
            while (!isdigit (*token))
            {
                token++;
            }
            sscanf (token, "%d", &eyes->num_eyes);
        }
        else if (strncmp (token, "eye-frames", strlen ("eye-frames")) == 0)
        {
            token += strlen ("eye-frames");
            while (!isdigit (*token))
            {
                token++;
            }
            sscanf (token, "%d", &eyes->eye_frames);
        }
        else if (strncmp (token, "eye-variations", strlen ("eye-variations")) == 0)
        {
            token += strlen ("eye-variations");
            while (!isdigit (*token))
            {
                token++;
            }
            sscanf (token, "%d", &eyes->eye_variations);
        }
        else if (strncmp (token, "pupil-frames", strlen ("pupil-frames")) == 0)
        {
            token += strlen ("pupil-frames");
            while (!isdigit (*token))
            {
                token++;
            }
            sscanf (token, "%d", &eyes->pupil_frames);
        }
        else if (strncmp (token, "pupil-variations", strlen ("pupil-varitions")) == 0)
        {
            token += strlen ("pupil-variations");
            while (!isdigit (*token))
            {
                token++;
            }
            sscanf (token, "%d", &eyes->pupil_variations);
        }
        else if (strncmp (token, "pupil-width", strlen ("pupil-width")) == 0)
        {
            token += strlen ("pupil-width");
            while (!isdigit (*token))
            {
                token++;
            }
            sscanf (token, "%d", &eyes->pupil_width);
        }
        else if (strncmp (token, "pupil-height", strlen ("pupil-height")) == 0)
        {
            token += strlen ("pupil-height");
            while (!isdigit (*token))
            {
                token++;
            }
            sscanf (token, "%d", &eyes->pupil_height);
        }

        else if (strncmp (token, "blinks-on-click", strlen ("blinks-on-click")) == 0)
        {
            token += strlen ("blinks-on-click");
            while (!isdigit (*token))
            {
                token++;
            }
            sscanf (token, "%d", &eyes->blinks_on_click);
        }

        else if (strncmp (token, "eye-pixmap", strlen ("eye-pixmap")) == 0)
        {
            token = strtok (NULL, "\"");
            token = strtok (NULL, "\"");
            if (eyes->eye_filename != NULL)
                g_free (eyes->eye_filename);
            eyes->eye_filename = g_strdup_printf ("%s%s",
                                                  eyes->theme_dir,
                                                  token);
        }
        else if (strncmp (token, "eye-overlay-pixmap", strlen ("eye-overlay-pixmap")) == 0)
        {
            token = strtok (NULL, "\"");
            token = strtok (NULL, "\"");
            if (eyes->eye_overlay_filename != NULL)
                g_free (eyes->eye_overlay_filename);
            eyes->eye_overlay_filename = g_strdup_printf ("%s%s",
                                                  eyes->theme_dir,
                                                  token);
        }
        else if (strncmp (token, "eye-mask-pixmap", strlen ("eye-mask-pixmap")) == 0)
        {
            token = strtok (NULL, "\"");
            token = strtok (NULL, "\"");
            if (eyes->eye_mask_filename != NULL)
                g_free (eyes->eye_mask_filename);
            eyes->eye_mask_filename = g_strdup_printf ("%s%s",
                                                  eyes->theme_dir,
                                                  token);
        }
        else if (strncmp (token, "pupil-pixmap", strlen ("pupil-pixmap")) == 0)
        {
            token = strtok (NULL, "\"");
            token = strtok (NULL, "\"");
            if (eyes->pupil_filename != NULL)
                g_free (eyes->pupil_filename);
            eyes->pupil_filename = g_strdup_printf ("%s%s",
                                                    eyes->theme_dir,
                                                    token);
        }
        else if (strncmp (token, "scale-to-panel", strlen ("scale-to-panel")) == 0)
        {
            token += strlen ("scale-to-panel");
            while (!isdigit (*token))
            {
                token++;
            }
            sscanf (token, "%d", &eyes->scale_to_panel);
        }


        fgets (line_buf, 512, theme_file);
    }
}


static GdkPixbuf *
assert_size_and_alpha (GdkPixbuf *pixbuf, 
                       gint width, 
                       gint height) 
{
    GdkPixbuf *temp_buf = NULL;
    GdkPixbuf *ret_buf = NULL;
    temp_buf = pixbuf;
    ret_buf = gdk_pixbuf_scale_simple (temp_buf, 
                                       width,
                                       height,
                                       GDK_INTERP_HYPER);
    g_object_unref (temp_buf);
    temp_buf = ret_buf;
    ret_buf = gdk_pixbuf_add_alpha (temp_buf,
                                    FALSE, 0, 0, 0);
    g_object_unref (temp_buf);
    return ret_buf;
}



void
load_theme (EyesPlugin  *eyes,
            const gchar *theme_dir)
{
    FILE* theme_file;
    gchar *file_name;
    gint panel_size;
    gdouble scale_factor = 1.0;
    gint scaled_width = 0;
    gint scaled_height = 0;
    GdkPixbuf *temp_buf = NULL;

    eyes->theme_dir = g_strdup_printf ("%s/", theme_dir);

    file_name = g_strdup_printf ("%s%s",theme_dir,"/config");
    theme_file = fopen (file_name, "r");
    if (theme_file == NULL) {
        g_error ("Unable to open theme file.");
    }

    parse_theme_file (eyes, theme_file);
    fclose (theme_file);

    eyes->theme_name = g_strdup (theme_dir);
    panel_size = xfce_panel_plugin_get_size (eyes->plugin);

    if (eyes->eye_frames < 1)
      eyes->eye_frames = 1;

    if (eyes->eye_variations < 1)
      eyes->eye_variations = 1;

    if (eyes->pupil_frames < 1)
      eyes->pupil_frames = 1;

    if (eyes->pupil_variations < 1)
      eyes->pupil_variations = 1;

    if (eyes->eye_image)
        g_object_unref (eyes->eye_image);

    eyes->eye_image = gdk_pixbuf_new_from_file (eyes->eye_filename, NULL);
    if (eyes->scale_to_panel > 0)
    {
        scale_factor = (float)panel_size / gdk_pixbuf_get_height (eyes->eye_image) * eyes->eye_variations;
        scaled_width = ceil (gdk_pixbuf_get_width (eyes->eye_image) * scale_factor);
        scaled_width = scaled_width - (scaled_width % eyes->eye_frames);

        eyes->eye_image = assert_size_and_alpha (eyes->eye_image, 
                                                 scaled_width, 
                                                 panel_size * eyes->eye_variations); 
    }
    else
    {
        temp_buf = eyes->eye_image;
        eyes->eye_image = gdk_pixbuf_add_alpha (temp_buf,
                                                FALSE, 0, 0, 0);
        g_object_unref (temp_buf);
    }

    if (eyes->eye_overlay_image)
        g_object_unref (eyes->eye_overlay_image);

    if (eyes->eye_overlay_filename != NULL)
    {
        eyes->eye_overlay_image = gdk_pixbuf_new_from_file (eyes->eye_overlay_filename, NULL);
        eyes->eye_overlay_image = assert_size_and_alpha (eyes->eye_overlay_image, 
                                                         gdk_pixbuf_get_width (eyes->eye_image),
                                                         gdk_pixbuf_get_height (eyes->eye_image));
    }


    if (eyes->eye_mask_image)
        g_object_unref (eyes->eye_mask_image);

    if (eyes->eye_mask_filename != NULL)
    {
        eyes->eye_mask_image = gdk_pixbuf_new_from_file (eyes->eye_mask_filename, NULL);
        eyes->eye_mask_image = assert_size_and_alpha (eyes->eye_mask_image, 
                                                      gdk_pixbuf_get_width (eyes->eye_image),
                                                      gdk_pixbuf_get_height (eyes->eye_image));
    }

    if (eyes->pupil_image)
        g_object_unref (eyes->pupil_image);

    eyes->pupil_image = gdk_pixbuf_new_from_file (eyes->pupil_filename, NULL);
    if (eyes->scale_to_panel > 0)
    {
        scaled_width = ceil (gdk_pixbuf_get_width (eyes->pupil_image) * scale_factor);
        scaled_width = scaled_width - (scaled_width % eyes->pupil_frames);
        scaled_height = ceil (gdk_pixbuf_get_height (eyes->pupil_image) * scale_factor);
        scaled_height = scaled_height - (scaled_height % eyes->pupil_variations);
        eyes->pupil_image = assert_size_and_alpha (eyes->pupil_image, 
                                                   scaled_width,
                                                   scaled_height);
    }


    eyes->eye_frame_width = gdk_pixbuf_get_width (eyes->eye_image) / eyes->eye_frames;
    eyes->eye_frame_height = gdk_pixbuf_get_height (eyes->eye_image) / eyes->eye_variations;

    eyes->pupil_frame_width = gdk_pixbuf_get_width (eyes->pupil_image) / eyes->pupil_frames;
    eyes->pupil_frame_height = gdk_pixbuf_get_height (eyes->pupil_image) / eyes->pupil_variations;


    if (eyes->pupil_width == 0)
        eyes->pupil_width = eyes->pupil_frame_width;
    else 
        eyes->pupil_width *= scale_factor;

    if (eyes->pupil_height == 0)
        eyes->pupil_height = eyes->pupil_frame_height;
    else 
        eyes->pupil_height *= scale_factor;

    if (eyes->eye_frames <= 1)
        eyes->blinks_on_click = 0;

    g_free (file_name);
}
