/**
 * @file thtmpdir.h
 * Temporary directory module.
 */
  
/* Copyright (C) 2000 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 *
 * -------------------------------------------------------------------- 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */
 
#ifndef thtmpdir_h
#define thtmpdir_h

#include <string>

/**
 * Temporary directory module.
 *
 * Creates and remove temporary directory.
 */
 
class thtmpdir {
  private:
    /**
     * @brief Helper class to switch between cwd and tmpdir.
     * Constructor will set current working directory to the temporary directory,
     * destructor vice versa.
     */
    class [[nodiscard]] tmpdir_handle
    {
      public:
        explicit tmpdir_handle(const std::string& tmp_dir);
        ~tmpdir_handle();

        /**
         * @brief Explicitly switch to the previous working directory.
         */
        void switch_from_tmpdir() noexcept;

        // copy and move disabled, we don't need them
        tmpdir_handle(const tmpdir_handle&) = delete;
        tmpdir_handle(tmpdir_handle&& other) = delete;
        tmpdir_handle& operator=(const tmpdir_handle&) = delete;
        tmpdir_handle& operator=(tmpdir_handle&& other) = delete;

      private:
        std::string prev_dir;
    };

  public:

  bool exist;  ///< ID whether temp directory exist.  
  bool tried;  ///< ID, if we've tried to create temp directory.  
  bool delete_all;  ///< ID whether to delete temporary directory.
  bool debug;  ///< ID, whether debugging mode is on.  
  std::string name = "."; ///< Name of temp dir.
  std::string file_name; ///< Name of temporary file.
  std::string tmp_remove_script; ///< Script for tmp directory deletion.

  /**
   * Creates temporary directory.
   */
   
  void create();
  
  
  /**
   * Removes temporary directory.
   */
   
  void remove();


  /**
   * Standard constructor.
   */
   
  thtmpdir();
  
  
  /**
   * Standard destructor.
   */
   
  ~thtmpdir();
  
  
  /**
   * Retrieve temporary directory name.
   */
   
  const char* get_dir_name();
  
  
  /**
   * Form valid path from temporary directory name and given file name.
   */
   
  const char* get_file_name(const char *fname);
  
  
  /**
   * Set deleting state.
   */
  
  void set_delete(bool delete_id);
  
  /**
   * Retrieve deleting state.
   */
   
  bool get_delete();
  
  
  /**
   * Turn deleting temp directory on.
   */
  
  void delete_on();


  /**
   * Turn deleting temp directory off.
   */
  
  void delete_off();

  /**
   * @brief Switch working directory to the temporary directory.
   * @return Handle object which will switch back to the previous
   * working directory in its destructor.
   */
  tmpdir_handle switch_to_tmpdir();
};

extern thtmpdir thtmp;

#endif


