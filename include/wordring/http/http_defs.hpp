#pragma once

#include <cstdint>

namespace wordring::http
{
	enum class method_name : std::uint32_t
	{
		// HTTP
		Get = 1,
		Head,
		Post,
		Put,
		Delete,
		Connect,
		Options,
		Trace,
        // rfc-5789
        Patch,
        Purge,
		// WebDAV
        Copy,
        Lock,
        Mkcol,
        Move,
        Propfind,
        Proppatch,
        Search,
        Unlock,
        Bind,
        Rebind,
        Unbind,
        Acl,
        // subversion
        Report,
        Mkactivity,
        Checkout,
        Merge,
        // upnp
        Msearch,
        Notify,
        Subscribe,
        Unsubscribe,
        // caldav
        Mkcalendar,
        // rfc-2068, section 19.6.1.2
        Link,
        Unlink
	};
}
