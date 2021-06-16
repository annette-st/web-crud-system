// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// NB: Modelled after Mozilla's code (originally written by Pamela Greene,
// later modified by others), but almost entirely rewritten for Chrome.
//   (netwerk/dns/src/nsEffectiveTLDService.cpp)
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Effective-TLD Service
 *
 * The Initial Developer of the Original Code is
 * Google Inc.
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Pamela Greene <pamg.bugs@gmail.com> (original author)
 *   Daniel Witte <dwitte@stanford.edu>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "plugin/filter/regdom/registry_controlled_domain.h"
#include "plugin/filter/regdom/lookup_string_in_fixed_set.h"

namespace chromium {
namespace registry_controlled_domains {



#include "plugin/filter/regdom/effective_tld_names-inc.cc"

// See make_dafsa.py for documentation of the generated dafsa byte array.

const unsigned char* g_graph = kDafsa;
size_t g_graph_length = sizeof(kDafsa);

struct MappedHostComponent {
  size_t original_begin;
  size_t original_end;

  size_t canonical_begin;
  size_t canonical_end;
};
#define DCHECK_LE(a, b) if(!(a <= b)) { throw std::runtime_error("dchak-le");}
#define DCHECK_EQ(a, b) if(!(a == b)) { throw std::runtime_error("dchak-eq");}
#define DCHECK_NE(a, b) if((a == b)) { throw std::runtime_error("dchak-eq");}
#define DCHECK(v) if(!(v)) { throw std::runtime_error(#v);}
// This version assumes we already removed leading dots from host as well as the
// last trailing dot if it had one.
size_t GetRegistryLengthInTrimmedHost(const nonstd::string_view& host,
                                      UnknownRegistryFilter unknown_filter,
                                      PrivateRegistryFilter private_filter) {
  size_t length;
  int type = LookupSuffixInReversedSet(
      g_graph, g_graph_length, private_filter == INCLUDE_PRIVATE_REGISTRIES,
      host, &length);

  DCHECK_LE(length, host.size());

  // No rule found in the registry.
  if (type == kDafsaNotFound) {
    // If we allow unknown registries, return the length of last subcomponent.
    if (unknown_filter == INCLUDE_UNKNOWN_REGISTRIES) {
      const size_t last_dot = host.find_last_of('.');
      if (last_dot != nonstd::string_view::npos)
        return host.size() - last_dot - 1;
    }
    return 0;
  }

  // Exception rules override wildcard rules when the domain is an exact
  // match, but wildcards take precedence when there's a subdomain.
  if (type & kDafsaWildcardRule) {
    // If the complete host matches, then the host is the wildcard suffix, so
    // return 0.
    if (length == host.size())
      return 0;

    DCHECK_LE(length + 2, host.size());
    DCHECK_EQ('.', host[host.size() - length - 1]);

    const size_t preceding_dot =
        host.find_last_of('.', host.size() - length - 2);

    // If no preceding dot, then the host is the registry itself, so return 0.
    if (preceding_dot == nonstd::string_view::npos)
      return 0;

    // Return suffix size plus size of subdomain.
    return host.size() - preceding_dot - 1;
  }

  if (type & kDafsaExceptionRule) {
    size_t first_dot = host.find_first_of('.', host.size() - length);
    if (first_dot == nonstd::string_view::npos) {
      // If we get here, we had an exception rule with no dots (e.g.
      // "!foo").  This would only be valid if we had a corresponding
      // wildcard rule, which would have to be "*".  But we explicitly
      // disallow that case, so this kind of rule is invalid.
      // TODO(https://crbug.com/459802): This assumes that all wildcard entries,
      // such as *.foo.invalid, also have their parent, foo.invalid, as an entry
      // on the PSL, which is why it returns the length of foo.invalid. This
      // isn't entirely correct.
      std::runtime_error("Invalid exception rule");
      return 0;
    }
    return host.length() - first_dot - 1;
  }

  DCHECK_NE(type, kDafsaNotFound);

  // If a complete match, then the host is the registry itself, so return 0.
  if (length == host.size())
    return 0;

  return length;
}

size_t GetRegistryLengthImpl(nonstd::string_view host,
                             UnknownRegistryFilter unknown_filter,
                             PrivateRegistryFilter private_filter) {
  if (host.empty())
    return std::string::npos;

  // Skip leading dots.
  const size_t host_check_begin = host.find_first_not_of('.');
  if (host_check_begin == nonstd::string_view::npos)
    return 0;  // Host is only dots.

  // A single trailing dot isn't relevant in this determination, but does need
  // to be included in the final returned length.
  size_t host_check_end = host.size();
  if (host.back() == '.')
    --host_check_end;

  size_t length = GetRegistryLengthInTrimmedHost(
      host.substr(host_check_begin, host_check_end - host_check_begin),
      unknown_filter, private_filter);

  if (length == 0)
    return 0;

  return length + host.size() - host_check_end;
}

nonstd::string_view GetDomainAndRegistryImpl(
    const nonstd::string_view& host,
    PrivateRegistryFilter private_filter) {
  DCHECK(!host.empty());

  // Find the length of the registry for this host.
  const size_t registry_length =
      GetRegistryLengthImpl(host, INCLUDE_UNKNOWN_REGISTRIES, private_filter);
  if ((registry_length == std::string::npos) || (registry_length == 0))
    return nonstd::string_view();  // No registry.
  // The "2" in this next line is 1 for the dot, plus a 1-char minimum preceding
  // subcomponent length.
  DCHECK(host.length() >= 2);
  if (registry_length > (host.length() - 2)) {
      return nonstd::string_view();
    //throw std::runtime_error("Host does not have at least one subcomponent before registry!");

  }

  // Move past the dot preceding the registry, and search for the next previous
  // dot.  Return the host from after that dot, or the whole host when there is
  // no dot.
  const size_t dot = host.rfind('.', host.length() - registry_length - 2);
  if (dot == std::string::npos)
    return host;
  return host.substr(dot + 1);
}

static bool HostIsIPAddress(const nonstd::string_view& )
{
    return false;
}

nonstd::string_view GetDomainAndRegistryAsStringPiece(
    const nonstd::string_view& host,
    PrivateRegistryFilter filter) {
  if (host.empty() || HostIsIPAddress(host))
    return nonstd::string_view();
  return GetDomainAndRegistryImpl(host, filter);
}

std::string GetDomainAndRegistry(const nonstd::string_view& host,
                                 PrivateRegistryFilter filter) {
 // url::CanonHostInfo host_info;
 // const std::string canon_host(CanonicalizeHost(host, &host_info));
 // if (canon_host.empty() || host_info.IsIPAddress())
 //   return std::string();
  return GetDomainAndRegistryImpl(host, filter).to_string();
}

}  // namespace registry_controlled_domains
}  // namespace net

