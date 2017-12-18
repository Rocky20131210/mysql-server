/*
 * Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#ifndef _XPL_SHA256_PASSWORD_CACHE_H_
#define _XPL_SHA256_PASSWORD_CACHE_H_

#include <unordered_map>
#include <string>
#include <utility>

#include "plugin/x/src/xpl_performance_schema.h"
#include "plugin/x/ngs/include/ngs/thread.h"
#include "plugin/x/ngs/include/ngs/interface/sha256_password_cache_interface.h"

#include "sql/auth/i_sha2_password_common.h"

namespace xpl {

/**
  Class used for storing hashed passwords for each authenticated user. This
  allows for fast authentication.
*/
class SHA256_password_cache final
  : public ngs::SHA256_password_cache_interface {
public:
  using sha2_cache_entry_t = std::string;
  using password_cache_t = std::unordered_map<std::string, sha2_cache_entry_t>;

  SHA256_password_cache();
  SHA256_password_cache(SHA256_password_cache&) = delete;
  SHA256_password_cache& operator=(const SHA256_password_cache&) = delete;
  SHA256_password_cache(SHA256_password_cache&&) = default;
  SHA256_password_cache& operator=(SHA256_password_cache&&) = default;

  void enable() override;
  void disable() override;

  bool upsert(const std::string &user, const std::string &host,
              const std::string &value) override;
  bool remove(const std::string &user, const std::string &host) override;
  std::pair<bool, std::string> get_entry(const std::string &user,
      const std::string &host) const override;
  bool contains(const std::string &user, const std::string &host,
                const std::string &value) const override;
  std::size_t size() const override { return m_password_cache.size(); }
  void clear() override;
private:
  std::string create_key(const std::string &user,
                         const std::string &host) const;
  std::pair<bool, sha2_cache_entry_t> create_hash(
      const std::string &value) const;

  mutable ngs::RWLock m_cache_lock;
  password_cache_t m_password_cache;
  bool m_accepting_input = false;
};

}  // namespace xpl

#endif  // _XPL_SHA256_PASSWORD_CACHE_H_
