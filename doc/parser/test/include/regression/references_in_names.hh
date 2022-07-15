namespace regression {
    
    ///
    /// @brief typedef for reference
    ///
    typedef int second_referenced_type;
    
    ///
    /// @brief references in names
    ///
    /// If a reference appears in a name, this will not automatically appear as
    /// a reference in Markdown.
    /// That typedefs are not recognized as return parameters is technically 
    /// another problem though
    ///
    /// @return a referenced type
    ///
    second_referenced_type function_with_reference_in_name();
}