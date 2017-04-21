#pragma once

#include <type_traits>
#include <typeinfo>

namespace mp
{
	// NOTE: std template aliases for accessing types or values of other templates are suffixed with _t / _v
	namespace fn
	{
		// --------------------------------------------------------------------------
		// mp::fn::transform
		// --------------------------------------------------------------------------
		// apply template meta function F to each type T in L
		template <class L, template <class...> class F> struct transform;
		template <template <class...> class L, class... Ts, template <class...> class F> struct transform<L<Ts...>, F>
		{
			using type = L<F<Ts>...>;
		};
		template <class L, template <class...> class F> using transform_t = typename transform<L, F>::type;

		// --------------------------------------------------------------------------
		// mp::fn::rename
		// --------------------------------------------------------------------------
		// rename L to F
		template <class L, template <class...> class F> struct rename;
		template <template <class...> class L, class... Ts, template <class...> class F> struct rename<L<Ts...>, F>
		{
			using type = F<Ts...>;
		};
		template <class L, template <class...> class F> using rename_t = typename rename<L, F>::type;

		// --------------------------------------------------------------------------
		// mp::fn::convert_from_to
		// Check conversion of type Ta to type Tb
		// type contains std::true_type if conversion is possible
		// --------------------------------------------------------------------------
		template <class Ta, class Tb> class convert_from_to
		{
		private:
			static Ta			   makeTa();
			static std::true_type  convert( Tb );
			static std::false_type convert( ... );

		public:
			using type = std::integral_constant<bool, std::is_same<std::true_type, decltype( convert( makeTa() ) )>::value>;
		};

		// --------------------------------------------------------------------------
		// mp::fn::is_convertible
		// Check conversion of type Ta to type Tb
		// is_convertible inherits std::integral_constant<bool>
		// --------------------------------------------------------------------------
		template <class Ta, class Tb> struct is_convertible : public std::integral_constant<bool, convert_from_to<Ta, Tb>::type::value>
		{
		};

		// --------------------------------------------------------------------------
		// mp::fn::strip
		// Removes cv modifers and reference and pointer declarations from type T
		// --------------------------------------------------------------------------
		template <class T> using strip   = std::remove_cv<typename std::remove_pointer<typename std::remove_reference<T>::type>::type>;
		template <class T> using strip_t = typename strip<T>::type;

		// --------------------------------------------------------------------------
		// mp::fn::baseclass_of
		// Check if type Ta is a baseclass of type Tb
		// --------------------------------------------------------------------------
		template <class Ta, class Tb>
		class is_baseclass_of
			: public std::integral_constant<bool, is_convertible<typename std::add_pointer<strip_t<Tb>>::type, typename std::add_pointer<strip_t<Ta>>::type>::value &&
													  !std::is_same<void, strip_t<Ta>>::value>
		{
		};

		// --------------------------------------------------------------------------
		// mp::fn::is_subclass_of
		// Check if type Ta is a subclass of of type Tb
		// --------------------------------------------------------------------------
		template <class Ta, class Tb>
		class is_subclass_of
			: public std::integral_constant<bool, is_convertible<typename std::add_pointer<strip_t<Ta>>::type, typename std::add_pointer<strip_t<Tb>>::type>::value &&
													  !std::is_same<void, strip_t<Tb>>::value>
		{
		};

		//// --------------------------------------------------------------------------
		//// wrapper collection
		//// mp::fn::is<A>::XXX<B>
		//// --------------------------------------------------------------------------
		// template <class Ta> class is
		//{
		// public:
		//	template <class Tb> class convertible_to
		//	{
		//	private:
		//		static Ta			   makeTa();
		//		static std::true_type  convert( Tb );
		//		static std::false_type convert( ... );

		//	public:
		//		using type = std::integral_constant<bool, std::is_same<std::true_type, decltype( convert( makeTa() ) )>::value>;
		//	};

		//	template <class Tb> class same_as : public std::is_same<Ta, Tb>
		//	{
		//	};

		//	template <class Tb> class baseclass_of
		//	{
		//	private:
		//		using pointer_Ta = typename std::add_pointer<typename std::remove_reference<typename std::remove_cv<Ta>::type>::type>::type;
		//		using pointer_Tb = typename std::add_pointer<typename std::remove_reference<typename std::remove_cv<Tb>::type>::type>::type;

		//	public:
		//		using type = typename convert_from_to_t<pointer_Tb, pointer_Ta>::type;
		//	};

		//	template <class Tb> class subclass_of
		//	{
		//	private:
		//	public:
		//		using pointer_Ta = typename std::add_pointer<typename std::remove_cv<typename std::remove_pointer<typename std::remove_reference<Ta>::type>::type>::type>::type;
		//		using pointer_Tb = typename std::add_pointer<typename std::remove_cv<typename std::remove_pointer<typename std::remove_reference<Tb>::type>::type>::type>::type;

		//		using type = typename convert_from_to_t<pointer_Ta, pointer_Tb>::type;
		//	};
		//};
	}
}