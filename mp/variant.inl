
// ---------------------------------------------------------------------------
template <class... Ts> typename variant<Ts...>::default_construct_func variant<Ts...>::default_constructors[ TypeCount::value ] = { &TypeHelper<Ts>::default_construct... };
template <class... Ts> typename variant<Ts...>::copy_construct_func	variant<Ts...>::copy_constructors[ TypeCount::value ]	= { &TypeHelper<Ts>::copy_construct... };
template <class... Ts> typename variant<Ts...>::destroy_func		   variant<Ts...>::destructors[ TypeCount::value ]			= { &TypeHelper<Ts>::destroy... };
template <class... Ts> typename variant<Ts...>::assign_func			   variant<Ts...>::assigners[ TypeCount::value ]			= { &TypeHelper<Ts>::assign... };

// ---------------------------------------------------------------------------
template <class... Ts> variant<Ts...>::variant()
{
	m_Value.m_TypeIndex = 0;
	default_constructors[ m_Value.m_TypeIndex ]( m_Value );
}

// ---------------------------------------------------------------------------
template <class... Ts> variant<Ts...>::variant( const variant &_rhs )
{
	m_Value.m_TypeIndex = _rhs.m_Value.m_TypeIndex;
	copy_constructors[ m_Value.m_TypeIndex ]( m_Value, reinterpret_cast<const void *>( &_rhs.m_Value.m_Storage ) );
}

// ---------------------------------------------------------------------------
template <class... Ts> template <class T> variant<Ts...>::variant( T &&_t )
{
	m_Value.m_TypeIndex = mp::tl::index_of_type_t<MyTypes, T>::value;
	copy_constructors[m_Value.m_TypeIndex](m_Value, reinterpret_cast<const void *>(&_t));
}

//// ---------------------------------------------------------------------------
// template <class... Ts>
// template <class T, class... Args> variant<Ts...>::variant( Args&&... args )
//{
//	m_Value.m_TypeIndex = mp::tl::index_of_type<MyTypes, T>::value;
//	::new (static_cast<void *>(&m_Value.m_Storage)) T(std::forward<Args>(args)...);
//}

// ---------------------------------------------------------------------------
template <class... Ts> variant<Ts...>::~variant()
{
	destructors[ m_Value.m_TypeIndex ]( m_Value );
}


// ------------------------------------------------------------------------
template <class... Ts> variant<Ts...> &variant<Ts...>::operator=(const variant<Ts...> &_rhs)
{
	if (m_Value.m_TypeIndex == _rhs.m_Value.m_TypeIndex)
	{
		assigners[m_Value.m_TypeIndex](m_Value, reinterpret_cast<const void *>(&_rhs.m_Value.m_Storage));
	}
	else
	{
		destructors[m_Value.m_TypeIndex](m_Value);
		m_Value.m_TypeIndex = _rhs.m_Value.m_TypeIndex;
		copy_constructors[m_Value.m_TypeIndex](m_Value, reinterpret_cast<const void *>(&_rhs.m_Value.m_Storage));
	}
	return *this;
}

// ------------------------------------------------------------------------
template <class... Ts> template <class T> void variant<Ts...>::set( const T &_value )
{
	if ( m_Value.m_TypeIndex == mp::tl::index_of_type<MyTypes, T>::value )
	{
		assigners[ m_Value.m_TypeIndex ]( m_Value, &_value );
	}
	else
	{
		destructors[ m_Value.m_TypeIndex ]( m_Value );
		::new ( static_cast<void *>( &m_Value.m_Storage ) ) T( _value );
	}
}

// ------------------------------------------------------------------------
template <class... Ts> template <class T> bool variant<Ts...>::get( T &_value ) const
{
	if ( mp::tl::index_of_type<MyTypes, T>::value == m_Value.m_TypeIndex )
	{
		_value = *reinterpret_cast<const T *>( &m_Value.m_Storage );
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
 template <class... Ts>
 template <class T, class... Args> void variant<Ts...>::emplace( Args&&... args )
{
	m_Value.m_TypeIndex = mp::tl::index_of_type_t<MyTypes, T>::value;
	::new (static_cast<void *>(&m_Value.m_Storage)) T(std::forward<Args>(args)...);
}
