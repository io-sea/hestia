namespace regression {

    ///
    /// @brief this is a function type with return value and an argument
    ///
    /// @param a input argument
    ///
    /// @return returns an int
    ///
    using function_using_definition_for_copydoc = int (*) (double a);
    
    ///
    /// @brief this is the actual function that belongs to the type 
    ///        regression::function_using_definition_for_copydoc
    ///
    /// @param a implementation of an input argument
    ///
    /// @return implementation of a return type
    ///
    int function_impl_for_using_definition(double a);
    
    ///
    /// @copydoc regression::function_using_definition_for_copydoc
    typedef int (*function_typedef) (double a);
    
}