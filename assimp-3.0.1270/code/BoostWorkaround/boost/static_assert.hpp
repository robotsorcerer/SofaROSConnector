
#ifndef AI_BOOST_STATIC_ASSERT_INCLUDED
#define AI_BOOST_STATIC_ASSERT_INCLUDED

#ifndef MY_BOOST_STATIC_ASSERT

namespace boost {
	namespace detail {

		template <bool b>  class static_assertion_failure;
		template <>        class static_assertion_failure<true> {};
	}
}

#include <boost/assert.hpp>

#define MY_BOOST_STATIC_ASSERT(eval) \
{boost::detail::static_assertion_failure<(eval)> assert_dummy;(void)assert_dummy;}

#endif
#endif // !! AI_BOOST_STATIC_ASSERT_INCLUDED
