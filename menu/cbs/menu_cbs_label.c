/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2011-2017 - Daniel De Matteis
 *
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include <compat/strl.h>
#include <file/file_path.h>
#include <string/stdstring.h>

#include "../menu_driver.h"
#include "../menu_cbs.h"
#include "../../file_path_special.h"
#ifdef HAVE_CHEATS
#include "../../cheat_manager.h"
#endif

#ifndef BIND_ACTION_LABEL
#define BIND_ACTION_LABEL(cbs, name) (cbs)->action_label = (name)
#endif

#define FILL_LABEL_MACRO(func, lbl) \
static int (func)(file_list_t *list, unsigned type, unsigned i, const char *label, const char *path, char *s, size_t len) \
{ \
   strlcpy(s, msg_hash_to_str(lbl), len); \
   return 0; \
}

static int action_bind_label_generic(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   return 0;
}

FILL_LABEL_MACRO(action_bind_label_rdb_entry_detail,         MENU_ENUM_LABEL_VALUE_RDB_ENTRY_DETAIL)
FILL_LABEL_MACRO(action_bind_label_internal_memory,          MSG_INTERNAL_STORAGE)
FILL_LABEL_MACRO(action_bind_label_removable_storage,        MSG_REMOVABLE_STORAGE)
FILL_LABEL_MACRO(action_bind_label_external_application_dir, MSG_EXTERNAL_APPLICATION_DIR)
FILL_LABEL_MACRO(action_bind_label_application_dir,          MSG_APPLICATION_DIR)

static int action_bind_label_playlist_collection_entry(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   if (!string_is_empty(path))
   {
      const char *playlist_file = path_basename_nocompression(path);

      if (!string_is_empty(playlist_file))
      {
         if (string_is_equal_noncase(path_get_extension(playlist_file),
                  "lpl"))
         {
            if (string_is_equal(playlist_file, FILE_PATH_CONTENT_HISTORY))
               strlcpy(s, msg_hash_to_str(MENU_ENUM_LABEL_VALUE_HISTORY_TAB), len);
            else if (string_is_equal(playlist_file, FILE_PATH_CONTENT_FAVORITES))
               strlcpy(s, msg_hash_to_str(MENU_ENUM_LABEL_VALUE_FAVORITES_TAB), len);
            else if (string_is_equal(playlist_file, FILE_PATH_CONTENT_IMAGE_HISTORY))
               strlcpy(s, msg_hash_to_str(MENU_ENUM_LABEL_VALUE_IMAGES_TAB), len);
            else if (string_is_equal(playlist_file, FILE_PATH_CONTENT_MUSIC_HISTORY))
               strlcpy(s, msg_hash_to_str(MENU_ENUM_LABEL_VALUE_MUSIC_TAB), len);
            else if (string_is_equal(playlist_file, FILE_PATH_CONTENT_VIDEO_HISTORY))
               strlcpy(s, msg_hash_to_str(MENU_ENUM_LABEL_VALUE_VIDEO_TAB), len);
            else
               fill_pathname(s, playlist_file, "", len);
         }
         /* This should never happen, but if it does just set
          * the label to the file name (it's better than nothing...) */
         else
            strlcpy(s, playlist_file, len);
      }
   }

   return 0;
}

#ifdef HAVE_CHEATS
static int action_bind_label_cheat_browse_address(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   snprintf(s, len, msg_hash_to_str(MENU_ENUM_LABEL_VALUE_CHEAT_BROWSE_MEMORY), cheat_manager_state.browse_address);
   return 0;
}
#endif

int menu_cbs_init_bind_label(menu_file_list_cbs_t *cbs,
      const char *path, const char *label, unsigned type, size_t idx)
{
   if (!cbs)
      return -1;

   BIND_ACTION_LABEL(cbs, action_bind_label_generic);

   if (cbs->enum_idx != MSG_UNKNOWN)
   {
      switch (cbs->enum_idx)
      {
         case MENU_ENUM_LABEL_LOAD_CONTENT_HISTORY:
         case MENU_ENUM_LABEL_GOTO_FAVORITES:
         case MENU_ENUM_LABEL_GOTO_IMAGES:
         case MENU_ENUM_LABEL_GOTO_MUSIC:
         case MENU_ENUM_LABEL_GOTO_VIDEO:
         case MENU_ENUM_LABEL_PLAYLIST_COLLECTION_ENTRY:
            BIND_ACTION_LABEL(cbs, action_bind_label_playlist_collection_entry);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_MANAGER_SETTINGS:
            BIND_ACTION_LABEL(cbs, action_bind_label_playlist_collection_entry);
            break;
         case MENU_ENUM_LABEL_CONTENT_SETTINGS:
            BIND_ACTION_LABEL(cbs, action_bind_label_playlist_collection_entry);
            break;
         case MENU_ENUM_LABEL_CHEAT_BROWSE_MEMORY:
#ifdef HAVE_CHEATS
            BIND_ACTION_LABEL(cbs, action_bind_label_cheat_browse_address);
#endif
            break;
         case MSG_INTERNAL_STORAGE:
            BIND_ACTION_LABEL(cbs, action_bind_label_internal_memory);
            break;
         case MSG_REMOVABLE_STORAGE:
            BIND_ACTION_LABEL(cbs, action_bind_label_removable_storage);
            break;
         case MSG_APPLICATION_DIR:
            BIND_ACTION_LABEL(cbs, action_bind_label_application_dir);
            break;
         case MSG_EXTERNAL_APPLICATION_DIR:
            BIND_ACTION_LABEL(cbs, action_bind_label_external_application_dir);
            break;
         case MENU_ENUM_LABEL_RDB_ENTRY_DETAIL:
            BIND_ACTION_LABEL(cbs, action_bind_label_rdb_entry_detail);
            break;
         default:
            break;
      }
   }

   return -1;
}
