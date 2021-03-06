/*
 *  Phusion Passenger - http://www.modrails.com/
 *  Copyright (C) 2008  Phusion
 *
 *  Phusion Passenger is a trademark of Hongli Lai & Ninh Bui.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef _PASSENGER_SPAWN_OPTIONS_H_
#define _PASSENGER_SPAWN_OPTIONS_H_

#include <string>
#include "Utils.h"

namespace Passenger {

using namespace std;

/**
 * Objects of this class contain important information for spawning operations,
 * such as which application is to be spawned. It is used by various methods,
 * such as ApplicationPool::get() and SpawnManager::spawn().
 *
 * <h2>Notes on privilege lowering support</h2>
 *
 * If <tt>lowerPrivilege</tt> is true, then it will be attempt to
 * switch the spawned application instance to the user who owns the
 * application's <tt>config/environment.rb</tt>, and to the default
 * group of that user.
 *
 * If that user doesn't exist on the system, or if that user is root,
 * then it will be attempted to switch to the username given by
 * <tt>lowestUser</tt> (and to the default group of that user).
 * If <tt>lowestUser</tt> doesn't exist either, or if switching user failed
 * (because the spawn server process does not have the privilege to do so),
 * then the application will be spawned anyway, without reporting an error.
 *
 * It goes without saying that lowering privilege is only possible if
 * the spawn server is running as root (and thus, by induction, that
 * Phusion Passenger and Apache's control process are also running as root).
 * Note that if Apache is listening on port 80, then its control process must
 * be running as root. See "doc/Security of user switching.txt" for
 * a detailed explanation.
 */
struct SpawnOptions {
	/**
	 * The root directory of the application to spawn. In case of a Ruby on Rails
	 * application, this is the folder that contains 'app/', 'public/', 'config/',
	 * etc. This must be a valid directory, but the path does not have to be absolute.
	 */
	string appRoot;
	
	/** Whether to lower the application's privileges. */
	bool lowerPrivilege;
	
	/**
	 * The user to fallback to if lowering privilege fails.
	 */
	string lowestUser;
	
	/**
	 * The RAILS_ENV/RACK_ENV environment that should be used. May not be an
	 * empty string.
	 */
	string environment;
	
	/**
	 * The spawn method to use. Either "smart" or "conservative". See the Ruby
	 * class <tt>SpawnManager</tt> for details.
	 */
	string spawnMethod;
	
	/** The application type. Either "rails", "rack" or "wsgi". */
	string appType;
	
	/**
	 * The idle timeout, in seconds, of Rails framework spawners.
	 * A timeout of 0 means that the framework spawner should never idle timeout. A timeout
	 * of -1 means that the default timeout value should be used.
	 *
	 * For more details about Rails framework spawners, please
	 * read the documentation on the Railz::FrameworkSpawner
	 * Ruby class.
	 */
	long frameworkSpawnerTimeout;
	
	/**
	 * The idle timeout, in seconds, of Rails application spawners.
	 * A timeout of 0 means that the application spawner should never idle timeout. A timeout
	 * of -1 means that the default timeout value should be used.
	 *
	 * For more details about Rails application spawners, please
	 * read the documentation on the Railz::ApplicationSpawner
	 * Ruby class.
	 */
	long appSpawnerTimeout;
	
	/**
	 * The maximum number of requests that the spawned application may process
	 * before exiting. A value of 0 means unlimited.
	 */
	unsigned long maxRequests;
	
	/**
	 * The maximum amount of memory (in MB) the spawned application may use.
	 * A value of 0 means unlimited.
	 */
	unsigned long memoryLimit;
	
	/**
	 * Creates a new SpawnOptions object with the default values filled in.
	 * One must still set appRoot manually, after having used this constructor.
	 */
	SpawnOptions() {
		lowerPrivilege = true;
		lowestUser     = "nobody";
		environment    = "production";
		spawnMethod    = "smart";
		appType        = "rails";
		frameworkSpawnerTimeout = -1;
		appSpawnerTimeout       = -1;
		maxRequests    = 0;
		memoryLimit    = 0;
	}
	
	/**
	 * Creates a new SpawnOptions object with the given values.
	 */
	SpawnOptions(const string &appRoot,
		bool lowerPrivilege       = true,
		const string &lowestUser  = "nobody",
		const string &environment = "production",
		const string &spawnMethod = "smart",
		const string &appType     = "rails",
		long frameworkSpawnerTimeout = -1,
		long appSpawnerTimeout       = -1,
		unsigned long maxRequests    = 0,
		unsigned long memoryLimit    = 0) {
		this->appRoot        = appRoot;
		this->lowerPrivilege = lowerPrivilege;
		this->lowestUser     = lowestUser;
		this->environment    = environment;
		this->spawnMethod    = spawnMethod;
		this->appType        = appType;
		this->frameworkSpawnerTimeout = frameworkSpawnerTimeout;
		this->appSpawnerTimeout       = appSpawnerTimeout;
		this->maxRequests    = maxRequests;
		this->memoryLimit    = memoryLimit;
	}
	
	/**
	 * Creates a new SpawnOptions object from the given string vector.
	 * This vector contains information that's written to by toVector().
	 *
	 * For example:
	 * @code
	 *   SpawnOptions options(...);
	 *   vector<string> vec;
	 *
	 *   vec.push_back("my");
	 *   vec.push_back("data");
	 *   options.toVector(vec);  // SpawnOptions information will start at index 2.
	 *
	 *   SpawnOptions copy(vec, 2);
	 * @endcode
	 *
	 * @param vec The vector containing spawn options information.
	 * @param startIndex The index in vec at which the information starts.
	 */
	SpawnOptions(const vector<string> &vec, unsigned int startIndex = 0) {
		appRoot        = vec[startIndex + 1];
		lowerPrivilege = vec[startIndex + 3] == "true";
		lowestUser     = vec[startIndex + 5];
		environment    = vec[startIndex + 7];
		spawnMethod    = vec[startIndex + 9];
		appType        = vec[startIndex + 11];
		frameworkSpawnerTimeout = atol(vec[startIndex + 13]);
		appSpawnerTimeout       = atol(vec[startIndex + 15]);
		maxRequests    = atol(vec[startIndex + 17]);
		memoryLimit    = atol(vec[startIndex + 19]);
	}
	
	/**
	 * Append the information in this SpawnOptions object to the given
	 * string vector. The resulting array could, for example, be used
	 * as a message to be sent to the spawn server.
	 */
	void toVector(vector<string> &vec) const {
		if (vec.capacity() < vec.size() + 10) {
			vec.reserve(vec.size() + 10);
		}
		appendKeyValue (vec, "app_root",        appRoot);
		appendKeyValue (vec, "lower_privilege", lowerPrivilege ? "true" : "false");
		appendKeyValue (vec, "lowest_user",     lowestUser);
		appendKeyValue (vec, "environment",     environment);
		appendKeyValue (vec, "spawn_method",    spawnMethod);
		appendKeyValue (vec, "app_type",        appType);
		appendKeyValue2(vec, "framework_spawner_timeout", frameworkSpawnerTimeout);
		appendKeyValue2(vec, "app_spawner_timeout",       appSpawnerTimeout);
		appendKeyValue3(vec, "max_requests",    maxRequests);
		appendKeyValue3(vec, "memory_limit",    memoryLimit);
	}

private:
	static inline void
	appendKeyValue(vector<string> &vec, const char *key, const string &value) {
		vec.push_back(key);
		vec.push_back(const_cast<string &>(value));
	}
	
	static inline void
	appendKeyValue(vector<string> &vec, const char *key, const char *value) {
		vec.push_back(key);
		vec.push_back(value);
	}
	
	static inline void
	appendKeyValue2(vector<string> &vec, const char *key, long value) {
		vec.push_back(key);
		vec.push_back(toString(value));
	}
	
	static inline void
	appendKeyValue3(vector<string> &vec, const char *key, unsigned long value) {
		vec.push_back(key);
		vec.push_back(toString(value));
	}
};

} // namespace Passenger

#endif /* _PASSENGER_SPAWN_OPTIONS_H_ */

