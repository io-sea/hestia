namespace reference {
    
    // -------------------------------------------------------------------------
    // Types for referencing later on
    // -------------------------------------------------------------------------
    
    ///
    /// @brief class for referencing
    ///
    class class_for_reference{};
    
    ///
    /// @brief function for referencing
    ///
    void function_for_reference();
    
    ///
    /// @brief variable for referencing
    ///
    int variable_for_reference;
    
    ///
    /// @brief enum for referencing
    ///
    enum enum_for_reference {
        /// first enum value
        enum_value_one, 
        /// second enum value
        enum_value_two
    };
    
    ///
    /// @brief typedef for referencing
    ///
    typedef int typedef_for_reference;
    
    // -------------------------------------------------------------------------
    // Test functions:
    // -------------------------------------------------------------------------
    
    ///
    /// @brief explicit reference to class
    ///
    /// This function uses @ref reference::class_for_reference
    void reference_to_class();    
    
    ///
    /// @brief explicit reference to function
    ///
    /// This function uses @ref reference::function_for_reference
    void reference_to_function();    
    
    ///
    /// @brief explicit reference to variable
    ///
    /// This function uses @ref reference::variable_for_reference
    void reference_to_variable();
    
    ///
    /// @brief explicit reference to enum
    ///
    /// This function uses @ref enum_for_reference
    void reference_to_enum();
    
    ///
    /// @brief explicit reference to typedef
    ///
    /// This function uses @ref reference::typedef_for_reference
    void reference_to_typedef();
    
    ///
    /// @brief implicit reference to other file
    ///
    /// This function uses @ref reference::inner_class
    void implicit_reference_to_other_file();
    
    ///
    /// @brief explicit reference to other file
    ///
    /// This function uses @ref reference::inner_class
    void explicit_reference_to_other_file();
    
    
}