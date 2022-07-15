namespace structure {

    ///
    /// @brief function without anything
    ///
    void function_without_parameters();


    ///
    /// @brief function with initializer
    ///
    void function_set_to_default() = default;


    ///
    /// @brief function with one argument
    ///
    /// @param a the one argument of this function
    ///
    void function_with_one_argument(int a);


    ///
    /// @brief function with one "in" argument
    ///
    /// @param[in] a the input argument of this function
    ///
    void function_with_in_argument(int a);

    ///
    /// @brief function with one "in" argument, that has a default value
    ///
    /// @param[in] a the input argument of this function, default value 0
    ///
    void function_with_default_value_argument(int a = 0);


    ///
    /// @brief function with one "out" argument
    ///
    /// @param[out] a the one output argument of this function
    ///
    void function_with_out_argument(int a);


    ///
    /// @brief function with two arguments
    ///
    /// @param [in] a the input argument of this function
    /// @param [inout] b the second, inout argument of this function
    ///
    void function_with_two_arguments(int a, int b);

    ///
    /// @brief function with return value
    ///
    /// @param [in] a the input argument of this function
    ///
    /// @return the return value
    ///
    int function_with_return_value(int a);

    ///
    /// @brief a const function - this is hopefully similar to override and such
    ///
    void const_function() const;

    ///
    /// @brief function with template
    ///
    /// @tparam T the template parameter of this function
    ///
    /// @param a input parameter of template type
    ///
    /// @return returns also a template type
    ///
    template<typename T>
    T function_with_template(T a);

    ///
    /// @brief function that throws an exception
    ///
    /// @exception innerException this is the exception thrown by this function
    ///
    void function_with_exception();

    ///
    /// @brief function that strange retval command
    ///
    /// @retval 0 this is one possible return value
    /// @retval 15 this is another possible return value
    ///
    /// @return return value - yes, we can have both!
    ///
    int function_with_retval();

} // namespace structure
