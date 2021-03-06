/**************************************************************************
*   Copyright (C) 2004-2007 by Michael Medin <michael@medin.name>         *
*                                                                         *
*   This code is part of NSClient++ - http://trac.nakednuns.org/nscp      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#pragma once

#include <string>
#include <map>
#include <set>
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>
#include <strEx.h>
#include <settings/settings_core.hpp>
#include <nsclient/logger.hpp>

namespace settings {
	class settings_handler_impl : public settings_core {
	private:
		typedef std::map<std::string, std::string> path_map;
		typedef std::map<std::string,settings_core::path_description> reg_paths_type;
		typedef std::map<key_path_type,key_path_type> mapped_paths_type;
		typedef settings_interface::string_list string_list;
		
		instance_raw_ptr instance_;
		boost::timed_mutex instance_mutex_;
		boost::filesystem::path base_path_;
		reg_paths_type registred_paths_;
		bool ready_flag;
		bool dirty_flag;
		bool reload_flag;

	public:
		settings_handler_impl() : ready_flag(false), dirty_flag(false), reload_flag(false) {}
		~settings_handler_impl() {
			destroy_all_instances();
		}
		bool is_ready() {
			return ready_flag;
		}
		void set_ready(bool flag=true) {
			ready_flag = flag;
		}
		bool is_dirty() {
			return dirty_flag;
		}
		void set_dirty(bool flag=true) {
			dirty_flag = flag;
		}
		void set_reload(bool flag = true) {
			reload_flag = flag;
		}
		bool needs_reload() {
			return reload_flag;
		}

		//////////////////////////////////////////////////////////////////////////
		/// Set the basepath for the settings subsystem.
		/// In other words set where the settings files reside
		///
		/// @param path the path to the settings files
		///
		/// @author mickem
		void set_base(boost::filesystem::path path) {
			base_path_ = path;
		}

		//////////////////////////////////////////////////////////////////////////
		/// Get the logging interface (will receive log messages)
		///
		/// @return the logger to use
		///
		/// @author mickem
		nsclient::logging::logger_interface* get_logger() {
			return nsclient::logging::logger::get_logger();
		}

		//////////////////////////////////////////////////////////////////////////
		/// Get the basepath for the settings subsystem.
		/// In other words get where the settings files reside
		///
		/// @return the path to the settings files
		///
		/// @author mickem
		boost::filesystem::path get_base() {
			return base_path_;
		}

		settings::error_list validate();

		void house_keeping();

		instance_ptr get();
		instance_ptr get_no_wait();
		void update_defaults();
		void remove_defaults();
		void migrate(instance_ptr from, instance_ptr to) {
			if (!from || !to)
				throw new settings_exception("Source or target is null");
			from->save_to(to);
			set_primary(to->get_context());
		}
		void migrate_to(instance_ptr to) {
			migrate(get(), to);
		}
		void migrate_from(instance_ptr from) {
			migrate(from, get());
		}
		void migrate_to(std::string to) {
			instance_ptr i = create_instance(to);
			migrate_to(i);
		}
		void migrate_from(std::string from) {
			instance_ptr i = create_instance(from);
			migrate_from(i);
		}
		void migrate(std::string from, std::string to) {
			instance_ptr ifrom = create_instance(from);
			instance_ptr ito = create_instance(to);
			migrate(ifrom, ito);
		}

		//////////////////////////////////////////////////////////////////////////
		/// Register a path with the settings module.
		/// A registered key or path will be nicely documented in some of the settings files when converted.
		///
		/// @param path The path to register
		/// @param type The type of value
		/// @param title The title to use
		/// @param description the description to use
		/// @param defValue the default value
		/// @param advanced advanced options will only be included if they are changed
		///
		/// @author mickem
		void register_path(unsigned int plugin_id, std::string path, std::string title, std::string description, bool advanced, bool is_sample) {
			reg_paths_type::iterator it = registred_paths_.find(path);
			if (it == registred_paths_.end()) {
				registred_paths_[path] = path_description(plugin_id, title, description, advanced, is_sample);
			} else {
				(*it).second.update(plugin_id, title, description, advanced, is_sample);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		/// Register a key with the settings module.
		/// A registered key or path will be nicely documented in some of the settings files when converted.
		///
		/// @param path The path to register
		/// @param key The key to register
		/// @param title The title to use
		/// @param description the description to use
		/// @param defValue the default value
		/// @param advanced advanced options will only be included if they are changed
		///
		/// @author mickem
		void register_key(unsigned int plugin_id, std::string path, std::string key, settings_core::key_type type, std::string title, std::string description, std::string defValue, bool advanced, bool is_sample) {
			reg_paths_type::iterator it = registred_paths_.find(path);
			if (it == registred_paths_.end()) {
				registred_paths_[path] = path_description(plugin_id);
				registred_paths_[path].keys[key] = key_description(plugin_id, title, description, type, defValue, advanced, is_sample);
			} else {
				(*it).second.append_plugin(plugin_id);
				path_description::keys_type::iterator kit = (*it).second.keys.find(key);
				if (kit == (*it).second.keys.end()) {
					(*it).second.keys[key] = key_description(plugin_id, title, description, type, defValue, advanced, is_sample);
				} else {
					(*kit).second.append_plugin(plugin_id);
					if (!description.empty() && (*kit).second.description.empty()) {
						(*kit).second.description = description;
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		/// Get info about a registered key.
		/// Used when writing settings files.
		///
		/// @param path The path of the key
		/// @param key The key of the key
		/// @return the key description
		///
		/// @author mickem
		settings_core::key_description get_registred_key(std::string path, std::string key) {
			reg_paths_type::const_iterator cit = registred_paths_.find(path);
			if (cit != registred_paths_.end()) {
				path_description::keys_type::const_iterator cit2 = (*cit).second.keys.find(key);
				if (cit2 != (*cit).second.keys.end()) {
					settings_core::key_description ret = (*cit2).second;
					return ret;
				}
			}
			throw settings_exception("Key not found: " + path + ", " + key);
		}
		settings_core::path_description get_registred_path(const std::string &path) {
			reg_paths_type::const_iterator cit = registred_paths_.find(path);
			if (cit != registred_paths_.end()) {
				return (*cit).second;
			}
			throw settings_exception("Path not found: " + path);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Get all registered sections
		///
		/// @return a list of section paths
		///
		/// @author mickem
		string_list get_reg_sections(std::string path, bool fetch_samples) {
			string_list ret;
			BOOST_FOREACH(const reg_paths_type::value_type &v, registred_paths_) {
				if ((!v.second.is_sample || fetch_samples) && (path.empty() || boost::starts_with(v.first, path)))
					ret.push_back(v.first);
			}
			return ret;
		}
		//////////////////////////////////////////////////////////////////////////
		/// Get all keys for a registered section.
		///
		/// @param path the path to find keys under
		/// @return a list of key names
		///
		/// @author mickem
		virtual string_list get_reg_keys(std::string path, bool fetch_samples) {
			string_list ret;
			reg_paths_type::const_iterator cit = registred_paths_.find(path);
			if (cit != registred_paths_.end()) {
				BOOST_FOREACH(const path_description::keys_type::value_type &v, (*cit).second.keys) {
					if (!v.second.is_sample || fetch_samples)
						ret.push_back(v.first);
				}
				return ret;
			}
			return ret;
		}


		void set_instance(std::string key) {
			boost::unique_lock<boost::timed_mutex> mutex(instance_mutex_, boost::get_system_time() + boost::posix_time::seconds(5));
			if (!mutex.owns_lock())
				throw settings_exception("set_instance Failed to get mutext, cant get access settings");
			instance_ = create_instance(key);
			if (!instance_)
				throw settings_exception("set_instance Failed to create instance for: " + key);
			instance_->set_core(this);
		}


	private:
		void destroy_all_instances();

		virtual std::string to_string() {
			if (instance_)
				return instance_->to_string();
			return "<NULL>";
		}

	};
	typedef settings_interface::string_list string_list;

}


